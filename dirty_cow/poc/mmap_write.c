#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <stdint.h>

void *map;
int f;
struct stat st;
char *name;
 
void write_test()
{
  madvise(map,100,MADV_DONTNEED);
  printf("madvise \n");

  int f=open("/proc/self/mem",O_RDWR);
  int i,c=0;
  lseek(f,(uintptr_t) map,SEEK_SET);
  c+=write(f,"hello world",strlen("hello world"));
  printf("procselfmem %d\n\n", c);
}
 
 
int main(int argc,char *argv[])
{
  //test for legal write
  f=open(argv[1],O_RDWR);
  fstat(f,&st);
  name=argv[1];
  map=mmap(NULL,st.st_size,PROT_WRITE,MAP_SHARED,f,0);
  printf("mmap %ld\n\n",(uintptr_t) map);
  write_test();
  return 0;
}

