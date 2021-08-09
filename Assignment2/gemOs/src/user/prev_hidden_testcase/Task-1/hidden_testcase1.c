//Checking error corresponding to opening a nonexisting file.Testcase should return error

#include<ulib.h>
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
    char *filename="test.txt";
    int fd = open(filename, O_READ|O_WRITE);
    if(fd < 0 )
        printf("Testcase passed\n");
    else
        printf("Testcase failed\n");

    return 0;
  }
