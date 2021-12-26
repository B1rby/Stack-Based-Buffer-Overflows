# Stack-Based-Buffer-Overflows
![](https://img.shields.io/badge/-HackTheBox-brightgreen) ![](https://img.shields.io/github/license/B1rby/Stack-Based-Buffer-Overflows-on-Linux-x86) ![](https://img.shields.io/github/last-commit/B1rby/Stack-Based-Buffer-Overflows-on-Linux-x86) 

![Image3](https://user-images.githubusercontent.com/87600765/147373294-b205c6a9-82e0-448b-a448-e75f0f1abd0f.png)


> Stack overflow is a type of buffer overflow vulnerability. When we pour water in a glass more than its capacity the water spills or overflow, similarly when we enter data in a buffer more than its capacity the data overflows to adjacent memory location causing program to crash. This is know as buffer overflow[^1]. 

## Stack
The stack has a Last-in, First-out (LIFO) design which means that we can only `pop` the last argument we `push`ed on the stack. 

**Example:**

![stack final 1](https://user-images.githubusercontent.com/87600765/147395873-664e6c6d-b363-4d8e-add0-f69979eb7409.png)

Imagine someone want to put 20 bytes of data into a buffer that had only been allocated 8 bytes of space, that type of action is allowed, even though it will most likely cause the program to crash. We can see that the 20 bytes that we sent overwrited the other existing values. The program will crash because the fucntion will try to return to the address of eip which is `0x1234565`. But this value no longer existed. This is known as a buffer overrun or buffer overflow, since the extra 12 bytes of data will overflow and spill out of the allocated memory, overwriting whatever happens to come next. If a critical piece of data is overwritten, the program will crash.


## Steps
The Buffer Overflow attack is defined by 6 steps:
1. Crash the binary
2. Take control of `eip`/`rip`
3. Determine the lenght of our shellcode
4. Identify the bad characters
5. Generating the shellcode
6. Identification of the return adress

All these steps are **explained** [here](https://github.com/B1rby/Stack-Based-Buffer-Overflows/blob/Skill-Asessment/Skill%20Asessment.md).
## Protections 
This vulnerability can be more or less difficult to exploit depending on the security put on the binary. The securities can be:
- [Canaries](https://en.wikipedia.org/wiki/Buffer_overflow_protection)
- [Address Space Layout Randomization (ASLR)](https://en.wikipedia.org/wiki/Address_space_layout_randomization)
- [Data Execution Prevention (DEP)](https://docs.microsoft.com/en-us/windows/win32/memory/data-execution-prevention)
- [No eXecution (NX) is the equivalent of DEP in Linux](https://docs.microsoft.com/en-us/windows/win32/win7appqual/dep-nx-protection)
- [Relocation Read-Only (RELRO)](https://ctf101.org/binary-exploitation/relocation-read-only/)
- [Position Independent Executable (PIE)](https://en.wikipedia.org/wiki/Position-independent_code)

## Writeup
Writeup about the Stack-Based Buffer Overflows on Linux x86 module of HackThebox Academy. The module was made by Cry0l1t3. In this writeup you will learn how I exploit a binary with a simple stack-based buffer overflow without any bypassing to do etc. And may be learn new things about stack-based buffer overflow.

![bof logo 3](https://user-images.githubusercontent.com/87600765/146174485-607fd934-cfea-4435-aa85-fec5fcb293e0.png)

- [Skill Assessment](https://github.com/B1rby/Stack-Based-Buffer-Overflows-on-Linux-x86/blob/Skill-Asessment/Skill%20Asessment.md)
- [Stack Based Buffer Overflow](https://github.com/B1rby/Stack-Based-Buffer-Overflows-on-Linux-x86/blob/Module-Questions/Stack-Based%20Buffer%20Overflow%20Section.md)
- [Take Control of EIP](https://github.com/B1rby/Stack-Based-Buffer-Overflows-on-Linux-x86/blob/Module-Questions/Take%20Control%20of%20EIP%20Section.md)
- [Determine the lenght of shellcode](https://github.com/B1rby/Stack-Based-Buffer-Overflows-on-Linux-x86/blob/Module-Questions/Determine%20the%20Length%20for%20Shellcode%20Section.md)
- [Identification of bad characters](https://github.com/B1rby/Stack-Based-Buffer-Overflows-on-Linux-x86/blob/Module-Questions/Identification%20of%20Bad%20Characters%20Section.md)
- [Generating shellcode](https://github.com/B1rby/Stack-Based-Buffer-Overflows-on-Linux-x86/blob/Module-Questions/Generating%20Shellcode%20Section.md)
- [Reverse shell](https://github.com/B1rby/Stack-Based-Buffer-Overflows-on-Linux-x86/blob/Module-Questions/Reverse%20shell.md)

## Coming soon™
- Writeup HTB Academy Stack-Based Buffer Overflows on Windows x86
- How to bypass NX
- How to bypass ASLR
- How to bypass Canaries
- How to bypass RELRO
- How to bypass PIE
- Available on gitbook

### Sources
*A couple of images that you have seen are providing from the HTB Academy website and the buffer images were modified by me for illustrate what I were doing*

*Academy, H. T. B. (n.d.). HTB Academy : Cyber security training. Cyber Security Training : HTB Academy. Retrieved December 14, 2021, from https://academy.hackthebox.com/module/details/318*
[^1]:*Thakur, A. S. (2019, December 4). Stack overflow vulnerability. Hacker Noon. Retrieved December 14, 2021, from https://hackernoon.com/stack-overflow-vulnerability-xou2bbm*
