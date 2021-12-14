# Stack-Based-Buffer-Overflows-on-Linux-x86
Writeup about the Stack-Based Buffer Overflows on Linux x86 module of HackThebox Academy

Stack overflow is a type of buffer overflow vulnerability. When we pour water in a glass more than its capacity the water spills or overflow, similarly when we enter data in a buffer more than its capacity the data overflows to adjacent memory location causing program to crash. This is know as buffer overflow. 

This vulnerability can be more or less difficult to exploit depending on the security put on the binary. The securities can be:
- Canaries
- Address Space Layout Randomization (ASLR)
- Data Execution Prevention (DEP)

In this writeup you will learn how I exploit a binary with a simple stack-based buffer overflow without any bypassing to do etc. And may be learn new things about stack-based buffer overflow.

- [Skill Asessment](https://github.com/B1rby/Stack-Based-Buffer-Overflows-on-Linux-x86/blob/Skill-Asessment/Skill%20Asessment.md)
- [Stack Based Buffer Overflow](https://github.com/B1rby/Stack-Based-Buffer-Overflows-on-Linux-x86/blob/Module-Questions/Stack-Based%20Buffer%20Overflow%20Section.md)
- [Take Control of EIP](https://github.com/B1rby/Stack-Based-Buffer-Overflows-on-Linux-x86/blob/Module-Questions/Take%20Control%20of%20EIP%20Section.md)
- [Determine the lenght of shellcode](https://github.com/B1rby/Stack-Based-Buffer-Overflows-on-Linux-x86/blob/Module-Questions/Determine%20the%20Length%20for%20Shellcode%20Section.md)
- [Identification of bad characters](https://github.com/B1rby/Stack-Based-Buffer-Overflows-on-Linux-x86/blob/Module-Questions/Identification%20of%20Bad%20Characters%20Section.md)
- [Generating shellcode](https://github.com/B1rby/Stack-Based-Buffer-Overflows-on-Linux-x86/blob/Module-Questions/Generating%20Shellcode%20Section.md)
- [Reverse shell](https://github.com/B1rby/Stack-Based-Buffer-Overflows-on-Linux-x86/blob/Module-Questions/Reverse%20shell.md)


*Thakur, A. S. (2019, December 4). Stack overflow vulnerability. Hacker Noon. Retrieved December 14, 2021, from https://hackernoon.com/stack-overflow-vulnerability-xou2bbm*
