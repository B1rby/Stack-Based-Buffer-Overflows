# Stack-Based-Buffer-Overflows-on-Windows-x86
![bof logo](https://user-images.githubusercontent.com/87600765/146046468-9592857f-9733-4c34-b3af-03ad023208a0.png)
Writeup about the Stack-Based Buffer Overflows on Linux x86 module of HackThebox Academy

Stack overflow is a type of buffer overflow vulnerability. When we pour water in a glass more than its capacity the water spills or overflow, similarly when we enter data in a buffer more than its capacity the data overflows to adjacent memory location causing program to crash. This is know as buffer overflow. 

This vulnerability can be more or less difficult to exploit depending on the security put on the binary. The securities can be:
- Canaries
- Address Space Layout Randomization (ASLR)
- Data Execution Prevention (DEP)

In this writeup you will learn how I exploit a binary with a simple stack-based buffer overflow without any bypassing to do etc. And may be learn new things about stack-based buffer overflow.
*Thakur, A. S. (2019, December 4). Stack overflow vulnerability. Hacker Noon. Retrieved December 14, 2021, from https://hackernoon.com/stack-overflow-vulnerability-xou2bbm*
