// checking whether file close is correct

#include<ulib.h>
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
    char *filename="test.txt";
    int create_fd = open(filename, O_CREAT|O_RDWR, O_READ|O_WRITE);

    if (create_fd < 0){
        printf("open failed\n");
        exit(0);
    }
    char *buf = "Hello, I am file!";
    write(create_fd, buf, 17);
    close(create_fd);
    int written = write(create_fd,buf,17);
    if(written < 0)
        printf("Testcase Passed\n");
    else
        printf("Testcase failed\n");

    return 0;
  }
