# Pointers in C and Assembly and How works memory
## Chapter 1 Pointers in C
### Part 1 Pointers in Assembly

In Assembly, we can treat our registers as pointers and for this, the syntax is really simple. Pointers in assembly are really simple. It's when you register contain
an existing address in memory and that this address is pointing to some value. Let's take a look at a simple NASM (Netwide Assembler) Program.
```nasm
global _start

section .text 
_start:
    push rbp
    mov rbp, rsp
    sub rsp, 0x20
    mov rax, 0x10
    mov rdi , rax
    mov QWORD [rbp-8], 0x10
    mov DWORD [rbp-0xc], 0x11
    mov BYTE  [rbp-0xd], 0x12
    leave
_exit:
    mov rax, 0x3c
    xor rdi, rdi
    syscall
 ```
 
 We gonna talk about the function prologue and epilogue later in depth but I am setting up a stack frame on the stack to allocate spaces for my values. 
 
 ![image](https://user-images.githubusercontent.com/87600765/185065189-8a8ab5ce-8bf5-49cd-8e9d-878d787841f9.png)

This our stack after setting up our stack frame so we can allocate memory below the base pointer and above the stack pointer. (If you don't understand, don't worry
I just want to show the concept of assembly pointers so I managing my stack as I want). After the `mov eax, 10` and the `mov rdi, rax`. This is what happens and you know it.

![image](https://user-images.githubusercontent.com/87600765/185066594-2154fcfa-b5e6-481d-a65e-892e5f4ff283.png)

It's simply move the value of rax, into rdi. Since the value of rax was 0x10, it moves the value 0x10 in rdi. In a pseudo code in c it would be like this

```
rax = rdi;
```
Ok that's cool. Let's take a look at our base pointer now. You can see that it's value is very special. It's an address as you can see that pointing to the value of
0x0. 

```nasm
$rbp   : 0x007fffffffe498  →  0x0000000000000000
````  
Well it means that it's a pointer because this register contains an address. If you do a `mov rax, rbp` as you guessed it will move the address and **not the value 0x0**. When you use the brackets `[]` it's for dereferencing this pointer. it means that we won't play with the address that rbp contains but with it's value that it is poiting to which is 0x0. When you play with the brackets you need to specify a size as I did. For example the first move is `mov QWORD [rbp-8], 0x10`. I know that QWORD is 4 word so 64 bits -> 8 bytes. So I need to substract the base pointer of 8 bytes.. It means that it's gonna move the value `0x0000000000000010` in rbp-8 (
**and not the address because we are dereferencing our pointer**). 

![image](https://user-images.githubusercontent.com/87600765/185069714-f5265425-fc2f-4f69-9b4b-ebaf9c0d2f81.png)

![image](https://user-images.githubusercontent.com/87600765/185069837-c33ba659-fdc0-492c-93b8-0e05dc6aa7c6.png)

g means giant which is 8 bytes.

Ok then I move into `[rbp-0xc]` (-8-4 = 12 = 0xc) a value of a size of a `DWORD` =  2 words = 32 bits = 4 bytes. So my value is only gonna use 4 bytes of space.

![image](https://user-images.githubusercontent.com/87600765/185070480-71d7bc82-12a8-4a60-befb-7d4279e794f5.png)

As you can see my value is on the `middle`. It's only use 4 bytes of space. 

![image](https://user-images.githubusercontent.com/87600765/185070702-c0f6cf40-95ad-478a-80e6-52236ba96b9d.png)

Then we move in the memory location pointed to by `rbp-0xd` (modify the value in `[rbp-0xd]`) one byte which is 0x13. Since it's one byte I am using (rbp-8-4-1 = 13
= 0xd). If we move this value let's see what happen.

![image](https://user-images.githubusercontent.com/87600765/185071410-0d2a5947-3465-43f6-8105-5e01b046c21c.png)

![image](https://user-images.githubusercontent.com/87600765/185071530-c0ac2f3a-89c3-43b3-9742-ffc34e8c1f6a.png)

As you can see is only using 1 byte of space. I am gonna talk about the leave instruction later but if I would define it simply, it's just put the stack at his original state. It's restoring the stack from what I did

![image](https://user-images.githubusercontent.com/87600765/185072180-6e2d0d0d-6485-4a4c-a825-fc9b8b3c0ca7.png)


Ok I hope you understand all of this because it's gonnna be essential for understand the initialization of variables in c
in assembly.

### Part 2 Introduction to pointers in C

Below, is a really simple representation of the computer memory or more specifically the Random Access Memory or RAM. This is the memory that we talk about in the 
context of program execution. 

![image](https://user-images.githubusercontent.com/87600765/184961364-e45f88ee-f84d-42d7-8577-88b37f4f94f5.png)
![image](https://user-images.githubusercontent.com/87600765/185072897-7cdc9d7c-6c3a-4c36-88f0-7cff612c1da5.png)



Let's say that each segment of this scheme is 1 byte. Each byte has this memory address. The address at the bottom is the lowest memory address. and this address 
will increase while we go up. Lets say that the address of the first segment `from the bottom` is 0x1. it means that the second address will be 0x2 etc. During the 
program execution you may declare some variables like this for example.

```c
int main(){
    int a;
    int b;
    return 0;
}
```

Well you can try to find the address of the variable but you won't find it. Why ? because your compiler is smart and know how to optimize  your program. Since you are not using the
variable a and b and that you didn't initialize them they won't exist. So remember if you declare variables like this but that you are not using there won't have memory 
allocated for these variables. 

But if it's not the case, well 4 bytes will be allocated on the stack since it's an integer and the value will be assigned to it. For example let's consider this program.
```c
int main(){
    int a = 23;
    int b = 14;
    char c = 'A';
    return 0;
}
```
This time the variables will be on the stack so space will be allocated for them. Let's take look to the assembly code used for initialized them.

```nasm
mov DWORD PTR [rbp-0x8], 0x17
mov DWORD PTR [rbp-0x4], 0xe
mov BYTE PTR [rbp-0x9], 0x41
``` 

Well the code is simple. The first line simply
say to move the value of 0x17 (23 in hex)
in the memory location pointed to by the address of
rbp-8. And the seconde line move the value of
0xe (14 in hex) in the memory location pointed to by 
the address of rbp-4.  And the third line
move one bytes which is 0x41 (A in hex) in the
memory location pointed to by rbp-9. But why 9. Well 
We initialize 2 integers so 4 + 4 = 8 bytes. 
And we initialize one char so its 1 byte. So we
allocated 9 bytes. As you see when the initialization is done
they specify the size of our variable. Since integers are 4 bytes
it's gonna use the `DWORD PTR []` syntax to use only 4 bytes because
a double word is 32 bits so 4 bytes. For the character since the size is only 8 bits so 1 bytes it's specify the size and use `BYTE PTR`.

You may be wondering why I have put 0xe at 
the bottom of the stack so at the higher
address where as 0xe is initialize to b after
0x17 is initialize to A. Well it's because 
a has a higher value than b. So because of 
my GCC (it mays not be the same for you)
the stack will put the lower value to the higher 
address.  This is because of memory alignment
for optimization but we won't talk about this
in this blog. 

And since the stack grows down it's 
gonna put 0x17 at the address-4 from the 
address of 0xe. 

It means that value at the top of the
stack has a lower memory address than the 
one at the bottom because the stack grows
down unlike the heap that we will talk later.

Let's use abstract address for it. if we push
0xe on the stack it will have for example the
address of 0x9, then if we push 0x17 on it 
it will have a value of 0x5 because since 0xe 
is an integer we need to have an address of 
0x9-4 so 0x5. Then if we push  0x41 which is A
in his ASCII value it will have the address of
0x1 because 0x17 is an integer and not a char
so 4 bytes long so we need to remove 4 from 
again 4 from the bottom address 0x9 which 
means that we need to do 0x9-8 which is 0x1. 

Also I talked about gcc and optimization earlier. 
If you use the flag `-03`with gcc, memory won't be
allocated for them on the stack because there 
are useless even if you initialized them but we 
won't talk about optimization here. If you want 
to know about these flags  you can check [here](https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html).

All of that is I think really interesting but the
question is: Can we operate with these address
variables , can we do things with them like 
arithmetic operations etc. Well we can do so. 
All of this is the purpose of a pointer. 

### Part 2. How pointers are managed in memory?  

Well, to make it the simple as possible pointers
are just variables that hold the address of 
another variable. The 2 particular syntax that 
we use for manage pointers are the `*` use to 
declare or dereference a pointer (we will
talk about this later) and the `&` use to have
the address of a variable. So now let's
consider this code.

```c
int main(){
    int a = 23;
    int b = 14;
    int* c = &b; // or int *c 
    return 0;
}
```

The fourth line is easy to understand. We 
declare the pointer to an integer with the
asterisk and with the ampersand you assign
the memory address to the pointer c. All of 
this means that now C has the memory address
b which has the value of 14. So it's pointing to
b. Let's take a look at this assembly code here.

```nasm
mov DWORD PTR [rbp-0x14], 0x17 ; DWORD = 4 bytes
mov DWORD PTR [rbp-0x18], 0xe
lea rax, [rbp-0x18]
mov QWORD PTR [rbp-0x10], rax  ; QWORD = 8 bytes
```

So you can see that for the variable `a` 
we move the value of 23 (0x17) in the memory 
location pointed to by `rbp-0x14`. Then the 
value of 0xe is moved to the memory location
pointed by r`bp-0x18` because an integer is 4 
bytes long. LEA stands for Load Effective Address.
It means that the address of rbp-0x18 which is
the address of `b` is gonna be load in the RAX 
register. It means that the RAX register holds 
the address of the variable `b`. And then RAX is 
moved to the memory location pointed to by 
`rbp-0x10`. It means that it moves the address
of `rbp-0x18` in the pointer `c`. Also you probably 
realized that the pointer is located to the
`rbp-0x10` And that the variable a is located to
rbp-0x14. So you probably guessed it, 
a pointer integer is 4 bytes long. Now let's take
a look to the scheme below.

Note: You may be wondering why the variables don't 
start at rbp-0x4. In fact, before our program starts
it sets up some binary securities. It's specifically 
set up the canary here but this is another subject,
we won't talk about that in this lesson. (it depends of your machine for me it does this
but it can changes). It's kind of weird that it setting up a canary but it's ok.

![image](https://user-images.githubusercontent.com/87600765/185079450-5f1c33fc-9593-4d7f-9582-23ea7228282c.png)

So the pointers points to b, so it holds
the value of 0x0 which is the address of 
b which contains the value of 14. Now let's
consider this code.
```c
int main(){
    int a = 23;
    int b = 14;
    int* c = &b; 

    printf("The value holds by c is: %p\n", c);
    printf("The value holds by b is: %d\n", b);
    printf("The value holds by a is: %d\n", a);
    printf("\n");
    printf("The address of b is: %p\n", &b);
    printf("The address of a is: %p\n", &a);  
    printf("The address of c is: %p\n", &c);  
    printf("Dereferencing c result to: %d\n", *c);
    return 0;
}
``` 

![image](https://user-images.githubusercontent.com/87600765/185080130-385c235b-02ab-469e-bc8c-5e3af04970f4.png)

The format specifier `%p` is for printing the
address. As you can see if we print the 
address of the other variables you can clearly
see that `c` has the address of `b`. 

The last printf you can see that I have put
an asterisk before `c`. The asterisk in this 
case is use to dereference the pointer. It 
means that that it won't show the address
that it contains but the value that it is 
pointing to which is 14.

Now let's play with pointers. Specifically, 
pointers arithmetic.

```c
int main(){
    int a = 23;
    int b = 14;
    int* c = &b; 
    printf("Address in c is: %p\n", c);
    printf("Dereferencing c result to: %d\n",*c);
    printf("Address in c is: %p\n", c+1);
    printf("Dereferencing c result to: : %d\n",*(c + 1));
    return 0;
}
```
![image](https://user-images.githubusercontent.com/87600765/185080244-dc957ec2-3db4-4b79-82e3-843a9ae5c4e5.png)

In the third printf if you do a `c+1`, the 
address will increase to 4 bytes since
an integer is 4 bytes long. 

```nasm
mov rax, QWORD PTR [rbp-0x10]
add rax, 0x4
```

As you can see, in a low level context the 
address contains in `c` is moved in rax and 
then 4 bytes are add to rax and then it 
continues to do some operations and call
printf. 

What happened for the `*(c + 1)` operations. In
a high level context the operation is similar 
to this.

```c
c++;
printf("Dereferencing c result to: : %d\n",*c );
```

It's simply increment the address of `c` by 1
which means by 4 bytes since it's an integer.
And In the stack were moved (**not pushed**) before the 
variable `a`. So the address of `b` is the 
equivalent of `(the address of a) - 4`. So if
you add 4 bytes to the address, `c` will
have the address of the variable `a` and
if you dereference the address well you have 
the integer 23 since the value of the 
variable `a` is 23. 

```nasm
mov rax, QWORD PTR [rbp-0x10]
add rax, 0x4
mov eax, DWORD PTR [rax]
```

So as earlier the address in `c` is moved to
rax and then 4 bytes are added to `rax` so
the address holds by  `c` is no longer the
address of `b` but of `a` . Then it moved the
value pointed to by `rax` which is 23 in `eax`.

### Part 3. Pointer type and void pointer

You are may be wondering why pointers 
don't have a generic type since they
are just stored addresses. Well, 
remember that we can dereference a 
pointer so the pointer needs to have the
the data type of the value it is pointing to.
We can also typecast our pointer to change
it's data type. Let's take a look in a high level
context to the code here.

```c
int main(){
    int a = 23; 
    int* b = &a;
    printf("The address of b is %p\n", b);
    printf("Dereferencing b result to: : %d\n",*b);
    char* c = (char*) b;
    printf("The address of b is %p\n", c);
    printf("Dereferencing b result to: : %d\n",*c);
    return 0;
}
```

![image](https://user-images.githubusercontent.com/87600765/185094264-bd3a0093-e490-48b2-8dc8-51dc7c635b31.png)

So as you can see we print the address contains in `b`
and then we dereference it to see the value there. 
Then we initialize a pointer and typecasting it into a 
char. So it's not a pointer to integer anymore but
a pointer to a char. Then as you can see it has the 
same address which is normal (we gonna see that after
the low level explanation) and then it's printing exactly
the same value. Let's change the value of a to 
`1020022`. I explain bellow why when when we dereference
the character it shows `118`.

![image](https://user-images.githubusercontent.com/87600765/185094543-5c4696e3-9605-4c63-a6bf-44a028a7a481.png)

Let's examine it now in a low-level context. When
the variable a was initialized with the value of `0x17`
we know that the integer is 4 bytes. The value of
0x17 represents in base 2 the value:

![image](https://user-images.githubusercontent.com/87600765/185094708-c9035d1d-22d7-4854-a2cc-bee400d1d830.png)

When the program will dereference the pointer and 
print the value, the program will look for those 4
bytes. You need to know that a character pointer 
is set exactly as an integer pointer. In fact a 
character and an integer pointer is exactly the 
same size which is 4 bytes in a x86_64 
architecture because they are storing addresses.
So the data type of a pointer doesn't change 
the initialization if it but it will only be on the 
printf that the data type will be take in account
in a low level view. So this is how the typecasting 
is done.   

```nasm
mov rax, QWORD PTR [rbp-0x8] ; dereferencing rbp-0x8 and move the value into rax pseudo code: rax = *(rbp-0x8)
mov QWORD PTR [rbp-0x10], rax ; mov rax in the memory location pointed to by rbp-0x10 pseudo code: *(rbp-0x10) = rax
```

Just some clarifications to understand what's going 
on here. rbp-0x8 is the address of the pointer `b`. 
and rbp-0x10 is the address of the pointer `c`. So 
the address pointed to by rbp-0x8 is moved into 
rax. Then the address of the pointer `b` is moved to 
rax which points to 1020022. then the value in rax 
is moved into the address pointed to by rbp-0x10. And 
now our pointer is alive and have the same address 
as the pointer `b`. But why when we print the value 
pointed to by `c`  we have 118. Well remember a 
character is only 1 byte. This the representation 
of 1020022 in binary. 

![image](https://user-images.githubusercontent.com/87600765/185095531-6edd1057-33da-41ef-b539-a912907a31ca.png)

So since a character is only 1 byte, it's gonna
print the value 01110110 which is 128 in base 2.


In C we have also another type of pointer call
the void pointer. This pointer can accepts
any date types but you won't be able to 
dereference a void pointer.
```c
int main(){
    int a = 23;
    void* p = &a;
    printf("p holds the address:%p\n", p+1);
    printf("p holds the address:%p\n", p);
    return 0;
}
```

![image](https://user-images.githubusercontent.com/87600765/185095927-f5846dd7-b51b-4539-ad5a-5405b042c6a5.png)


But you can still do arithmetic with a void 
pointer but if you try to dereference it
you will have of course a compilation error.

### Part 4: Double Pointer

A double pointer is in fact not as difficult
as you expect. It simply a pointer that 
doesn't hold the address of a normal 
variable but holds the address of another
pointer. So to make it more easier a double
pointer is a pointer to pointer so it's a 
pointer which have the address of another 
pointer. Let's take a look to this c program. 

```c
int main(){
    int a = 23;
    int* b = &a;
    int** c = &b;
    int ***d = &c;
    printf("address of a is %p\n", &a);
    printf("address of b is %p\n", &b);
    printf("address of c is %p\n\n", &c);
    printf("value of a is %d\n", a);
    printf("address in b is %p\n", b);
    printf("address in c is %p\n", c);
    printf("address in d is %p\n\n", d);
    printf("dereferencing b results to: %d\n", *b);
    printf("dereferencing c results to: %d\n", **c);
    printf("dereferencing d results to: %d\n", ***d);
    return 0;
}
```

![image](https://user-images.githubusercontent.com/87600765/185096149-f20f67cd-f307-4c71-84b6-1ad90a0048bf.png)

The code is pretty straight forward and the 
actions for initialize the pointers are the same
as earlier. It's gonna do a LEA (Load Effective
Address ) of the pointer in rax and then mov
the address in a block of memory allocated for
the next pointer. So `b` will have the address of `a`, 
then `c` will have the address of `b` and then d will
have the address of `c`. 

![image](https://user-images.githubusercontent.com/87600765/185096738-45a9644c-5f53-453d-9da6-a19c39aa3d44.png)
![image](https://user-images.githubusercontent.com/87600765/185096793-1f5b6d43-33c1-45b8-b009-b5fd7df1abf0.png)

### Part 5. Pointer as function argument

Let's consider this c program as a demo.

```c
#include <stdio.h>
#include <stdlib.h>

void calculator(int result){ 
    result = result + 20;
}

int main(){
    char buffer[100];
    printf("Welcome in a simple calculator\n");
    printf("Please put 1 number: ");
    fgets(buffer, 100, stdin); 
    int result = atoi(buffer);
    calculator(result);
    printf("The result is: %d", result);
    return 0;
}
```

First of all, we have a `calculator` function that takes
as an argument an integer. Then we have our main
function with a buffer array of 100 bytes. Then we 
use the `fgets` function to take the input of our user
and we passed as an argument our buffer array of
100 bytes. Then we initialize an integer variable 
named result with the `atoi` function and we passed
as an argument our buffer and we call our calculator
function and we passed as an argument or result
which is an integer thanks to the `atoi` function. After
it gets back to main and print the result. Now if you
have some basics in c you obviously know what 
will happen. Oh I forgot, in the `calculator` function
the integers passed in is added up to 20. Now let's
run it with the value 13.

![image](https://user-images.githubusercontent.com/87600765/185097374-7ea9aeb4-aa01-4ffe-b4f8-b81811955f84.png)


Well that was expected. In a high-level context the variable `result` in the main function and the variable result in the 
calculator function are different because they are local variables. So when you passed in integer result from the main
function to the calculator function a new local variable result gets created in the calculator function. So if you add 20
to the local variable `result` in calculator it won't add 20 to the local variable `result` in the main function.  

Now let's debug our program and see what really happen.

![image](https://user-images.githubusercontent.com/87600765/185099694-4ac9eb25-aa68-4e0f-a90d-13c4a5f5df7b.png)

I have set a couple breakpoints here. Now let's run the program from my first breakpoint because we have a lot of things to say. We gonna see what is the concept
of stack frames and how they are setting up, the concept of function prologue and epilogue and talk clearly about what the stack and base pointer do.

#### What is a stack frame ?

When a **function is called in memory**, a stack frame is created for this function. The stack frames contains the saved callee register (return address), arguments and the **local variables**.

![image](https://user-images.githubusercontent.com/87600765/185103143-a9ac90dc-346f-4960-b6ab-a6b18307971e.png)

In fact according to the calling convention, when a function is called 



