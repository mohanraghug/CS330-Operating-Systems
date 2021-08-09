//Checking error corresponding file mode.Testcase should return error

#include<ulib.h>
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
    char *filename="test.txt";
    int fd = open(filename,O_CREAT|O_READ,O_READ);
    int fd1= open(filename,O_WRITE);
    
    if(fd1<0)
        printf("Testcase Passed\n");
    else
        printf("Testcase failed\n");

    close(fd);
    return 0;
  }
