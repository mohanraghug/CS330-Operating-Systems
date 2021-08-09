//Checking basic read functionality. Ideally this testcase should pass for everyone.

#include <ulib.h>

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
    char *filename="test.txt";
    int fd = open(filename,O_CREAT|O_RDWR,O_READ|O_WRITE);

    if(fd < 0){
        printf("open failed\n");
        exit(0);
    }

    char * buff = "In scientific terms, the meanings of hominid and hominin have changed during the recent decades with advances in the discovery and study of the fossil ancestors of modern humans. The previously clear boundary between humans and apes has blurred, resulting in biologists now acknowledging the hominids as encompassing multiple species, and Homo and close relatives since the split from chimpanzees as the only hominins. There is also a distinction between anatomically modern humans and Archaic Homo sapiens, the earliest fossil members of the species.";
    int written = write(fd,buff,500);
    if(written < 0){
        printf("Write failed\n");
        exit(0);
    }
    close(fd);

    char buff1[1024];
    int fd1 = open(filename,O_READ);
    int readr1 = read(fd1,buff1,200);
    int readr2 = read(fd1,buff1,100);
    if(readr1 == 200 && readr2 == 100)
        printf("Testcase passed\n");
    else
        printf("Testcase failed\n");

    close(fd1);

    return 0;
  }
