## Call sys_execve with binsh argument

execve() executes the program referred to by pathname.  This causes the program that is currently being run by the calling process to be replaced with a new program, with newly initialized stack, heap, and (initialized and uninitialized) data segments.
In this case we are gonna exploit the program, call sys_execve and use the pathname /bin/sh to spawn a shell. 

### Security

In this case ASLR and NX will be enable. Address space layout randomization (ASLR) is a computer security technique involved in preventing exploitation of memory corruption vulnerabilities. In order to prevent an attacker from reliably jumping to, for example, a particular exploited function in memory, ASLR randomly arranges the address space positions of key data areas of a process, including the base of the executable and the positions of the stack, heap and libraries but doesn't apply in the .text (where the gadgets are located) , .data, .rodata and .bss sections.

![image](https://user-images.githubusercontent.com/87600765/175817705-2ad50828-4ac0-4d8f-91e7-f29c7968f9d1.png)

This technique is really useful when the binary is statically linked s you can't do a ret2plt or something else. 

### Practice

You can found the binary [here](https://github.com/B1rby/Art-of-Exploitation/blob/main/rop/ret2mprotect/vuln.c). To compile it use this command. Of course you need to enable ASLR with the command above.

```bash
└─# gcc vuln.c -o vuln -fno-stack-protector -no-pie -static
```

### Offset 
First of all we need to know the amount of bytes we need before overwrite RIP (Instruction Pointer) to overflow the binary and then exploit it. For this I 'am gonna use gef. I will create a pattern of 300 bytes, then I am gonna send it.

![image](https://user-images.githubusercontent.com/87600765/175960675-6d704766-b3cd-4a2c-9978-05f598a5932d.png)

To know the rip value during the segfault, we do an info frame.

![image](https://user-images.githubusercontent.com/87600765/175960832-0ba7f3e6-598f-4856-942b-d5e0fd8aae8e.png)

You can see that the binary receives a segfault because the address of rip was overwritten so the address in it is an address that doesn't exist so the program doesn't know where to go and crashes. Now let's take a look at the saved rip. saved rip is the return address so it's a callee saved register. Across calls, these registers have to have a preserved value. To saved this register the program pushes it on the stack, then the call is down and then it pops it from the stack. So right now we want the value of the saved rip register with the pattern that we sent. For this we can do an info frame. This command allows us to have information about the stack frame. As you know the stack frame is used for storing local functions variables and context during function calls. In this case the rip is stored on this stack frame because during functions call the value in rip can change.

stack level 0 = frame number in backtrace, 0 is the currently executing frame, which grows downwards, which is how the stack grows. frame at `0x7fffffffe228` = It's the starting memory address of this stack frame. rip = `0x401889` in main; saved rip = 0x6361617663616175(overflowed by the padding) = RIP which is the instruction pointer points to the next instruction to execute. In this case, the next instruction is at `0x401889`. For the saved rip is the explanation is right above. Arglist at `0x6361617463616173`, args = It's the starting address of the arguments. Locals at `0x6361617463616173` = It's the address of local variables. Previous frame's sp is `0x7fffffffe230` = It is where the previous frame´s stack pointer points to (the caller frame), at the moment of calling, it is also the starting memory address of called stack frame.

Then we just need to use `pattern offset`.

![image](https://user-images.githubusercontent.com/87600765/175982432-d7eaf7aa-5745-4452-9fbf-948a6ff68739.png)

it means that we need 280 bytes before reaching rip. So if we send 288 bytes the last 8 bytes will be in rip.


### Gadgets

Of course for this we will follow the syscall table.
![image](https://user-images.githubusercontent.com/87600765/175819535-cb7fd23e-eee9-4e95-b8e6-98a47e13d70c.png)

We will use only gadgets for it so according to the syscall table we will need to put in rax `0x3b`, in rdi the address of our string, in rsi `0x0` and in rdx `0x0`. For our rop chain we will need gadgets ending with ret (`pop rip`)of course, so we will need a `pop rax`, `pop rdi`, `pop rsi` and `pop rdx` and of course the syscall. For this I will use ropper to find the gadgets.

![image](https://user-images.githubusercontent.com/87600765/175832399-b6d9ec6e-9ea4-4af8-a384-4cc74ee797d0.png)

done ! But wait, we don't have the argument /bin/sh. How we gonna have it ? We gonna simply write `/bin/sh`. To do this we need an address where to write the string, and a gadget to put it there. Ho we do in assembly we can write our `/bin/sh` string in the `.data` section. To have the address of the data section we simply do an info files on gdb.

![image](https://user-images.githubusercontent.com/87600765/175888983-f1beb37a-a137-4664-8ba9-3081b8f7e59d.png)

As you can see there are two addresses. The first address is where the data section starts and the second one is where the data section end. To be sure we can verify if there is enough space for our string so we can substract the end address with the start address.

![image](https://user-images.githubusercontent.com/87600765/175900726-fa3bf82c-e876-4d41-a695-72d123ce19d2.png)

As expected, we clearly have enough space since we gonna use only 8 bytes for `/bin/sh`. Now how we write it there. Well, to do it we can simply pop the address of the data section in a register, pop the string /bin/sh in another register and `mov`e the register with `/bin/sh` in the location pointed to by the register having the address of the data section. So for our last gadget we need a `mov [reg], reg` gadget.

![image](https://user-images.githubusercontent.com/87600765/176020397-d38c9067-bf17-481e-acbc-1b7269869aab.png)

I am gonna use this gadget so we will need to pop the address of the data section in `rdi` and the `/bin/sh` string in `rdx`. Ok, so we can now starting writing our exploit. 

```pl
#!/usr/bin/perl

$| = 1;
$offset = "A" x 280;
$pop_rax = pack("Q", 0x0000000000446613);
$pop_rdi = pack("Q", 0x000000000045c030);
$pop_rsi = pack("Q", 0x000000000040873e);
$pop_rdx = pack("Q", 0x00000000004016eb);
$syscall = pack("Q", 0x0000000000414f8c);
$mov_ptr = pack("Q", 0x00000000004307d3);
$data_section = pack("Q", 0x4ae0e0);
$bin_sh = "/bin/sh\x00";

# write part

print $offset . $pop_rdx . $bin_sh . $pop_rdi . $data_section . $mov_ptr ;
```

Let's debug what the payload is doing. 

![image](https://user-images.githubusercontent.com/87600765/176023371-054fd3fd-de8d-4c04-9808-8a4562d08cbc.png)

So as you can see here rip is pointing to the gadget `pop rdx` which is about to be execute. and the `/bin/sh string` is in the top of the stack.

![image](https://user-images.githubusercontent.com/87600765/176023784-088b3a81-ef1f-4012-9c71-0ee732e3a972.png)

So here the string and the data section where poped of the stack. And as you can see there is the address of the mov gadget and the ret is about to be executed and since the ret is the equivalent of `pop rip` rip will point to the address of the mov gadget and it thanks to the ret after each gadgets that we can built a chain like this as I said earlier.

![image](https://user-images.githubusercontent.com/87600765/176024123-1528ddb7-cc36-4d68-a647-56e4218dc739.png)

![image](https://user-images.githubusercontent.com/87600765/176024268-fd93ae7c-c1b8-40b9-81c5-22c72e9ff305.png)


So now as you can see that after the mov gadget the `/bin/sh` is in the data section. 

### sys_execve

Now that we have our `/bin/sh` string we can simply call sys_execve with the address where is located the string. Let's do it. The final exploit is [here](https://github.com/B1rby/Art-of-Exploitation/blob/main/rop/sys_execve/exploit.pl). 

![image](https://user-images.githubusercontent.com/87600765/176140504-03802fa1-4349-4343-8558-877d4bd026c5.png)

And it worked ! I won't step it into it since it's pretty straight forward. All the arguments were popped correctly on their registers and then after the syscall, it's executing a new program which is or `/bin/bash`.

You can find the py version with pwntools [here](https://github.com/B1rby/Art-of-Exploitation/blob/main/rop/sys_execve/exploit.py).

### Sources

*Wikimedia Foundation. (2022, June 25). Address space layout randomization. Wikipedia. Retrieved June 28, 2022, from https://en.wikipedia.org/wiki/Address_space_layout_randomization*
