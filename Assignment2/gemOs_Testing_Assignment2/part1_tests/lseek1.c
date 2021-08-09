#include<ulib.h>
// #include <string.h>


int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	int pid;
	char buf1[20];
	char *filename="test_seek.txt";
	int create_fd = open(filename, O_CREAT|O_RDWR, O_READ|O_WRITE);
	char *buf = "abcdefghijklmnopqrstuvwxyz";

	write(create_fd, buf, 26+1); // +1 for \0

    /* Test 3 whence options */
    /* SEEK SET, SEEK CUR, SEEK END */

    lseek(create_fd, 1, SEEK_SET);
    // Read 1 byte 'b'
    read(create_fd, buf1, 1);
    buf1[1] = '\0';

    if (buf1[0] != 'b') {
        //fail
        printf("Test failed\n");
        return -1;
    }

    printf("Test passed\n");
	return 0;
}
