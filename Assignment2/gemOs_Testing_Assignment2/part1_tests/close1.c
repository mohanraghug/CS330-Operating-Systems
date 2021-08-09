
#include<ulib.h>


int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
    int pid;
    char buf1[20];
    char *filename="test.txt";
    int create_fd = open(filename, O_CREAT|O_RDWR, O_READ|O_WRITE);
    
    if (create_fd != 3) {
        printf("Test failed\n");
        return -1;
    }


    close(create_fd);
    create_fd = open(filename, O_CREAT|O_RDWR, O_READ|O_WRITE);

    if (create_fd != 3) {
        printf("Test failed\n");
        return -1;
    }

    printf("Test passed\n");

    close(create_fd);





    return 0;
}