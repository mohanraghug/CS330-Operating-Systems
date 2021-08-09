/* checking whether file mode is set correctly while opening the file*/

#include<ulib.h>
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
    char *filename="test.txt";
    int create_fd = open(filename, O_CREAT|O_READ, O_READ);
    if(create_fd < 0){
        printf("Open failed\n");
        exit(0);
    }
    close(create_fd);
    int write_fd = open(filename,O_RDWR);
    if(write_fd < 0)
        printf("Testcase Passed\n");
    else
        printf("Testcase failed\n");

    return 0;
  }
