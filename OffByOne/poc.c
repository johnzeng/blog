#include <stdio.h>
#include <string.h>

void hello1(const char* input)
{
    char target[16];
    strcpy(target, input);
    printf(target);
}

void hello2(){
    printf("hello any way hahahahahahah");
}

int main(int argc, const char *argv[])
{
    if(strlen(argv[1]) > 16){
        return 1;
    }
    hello1(argv[1]);
    return 0;
}
