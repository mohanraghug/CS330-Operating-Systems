
#include<ulib.h>


int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
    int pid;
    char buf1[20];
    char *filename="test.txt";
    int create_fd = open(filename, O_CREAT|O_RDWR, O_READ|O_WRITE);

    // printf("File objects : %d\n", get_fileobj_count());

    if (get_fileobj_count() <= 3) {
        // fail
        printf("Test failed\n");
        // printf("%d is count\n",get_fileobj_count());

        return -1;
    }


    close(create_fd);
    

    if (get_fileobj_count() > 3) {
        // pass

        printf("Test failed\n");
        // printf("%d is count\n",get_fileobj_count());
        return -1;
    }

    printf("Test passed\n");

    return 0;
}