#include <stdio.h>
#include <string.h>

int main(int argc, char **argv){

    char buffer[260];
    int len, i;
    printf("Welcome in Birby Enterprise\n");
    printf("To join the company, please register with your full name: ");
    gets(buffer);
    len = strlen(buffer);
    printf("Welcome %s in Birby Enterprise\n", buffer);

    return 0;

}
