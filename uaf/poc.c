#include <stdio.h>
#include <string.h>

int main(int argc, const char *argv[])
{
    char *p1 = (char*) malloc(sizeof(char) * 10);
    memcpy(p1, "hello", 10);
    printf("p1 addr = %x, p1 value:%s\n", p1,p1 );
    free(p1);

    char *p2 = (char*) malloc(sizeof(char) * 20);
    memcpy(p2, "world", 20);
    printf("p2 addr = %x, p2 value:%s\n", p2,p2 );
    free(p2);
    return 0;
}
