#include <sys/types.h>
#include <sys/ptrace.h>
#include <stdio.h>

int main(int argc, const char *argv[])
{
    
    ptrace(PT_DENY_ATTACH, 0, NULL, 0);
    printf("hello world, you can't debug me");
    return 0;
}
