

#include<ulib.h>
// #include <string.h>


int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	int pid, ret;
	char buf1[20];
	char *filename="test_seek.txt";
	int create_fd = open(filename, O_CREAT|O_RDWR, O_READ|O_WRITE);
	char *buf = "abcdefghijklmnopqrstuvwxyz";

	write(create_fd, buf, 26+1); // +1 for \0
	close(create_fd);

	create_fd = open(filename, O_CREAT|O_RDWR, O_READ|O_WRITE);
	ret = lseek(create_fd, 10, SEEK_END);

	if (ret != -EINVAL) {
		printf("Test failed\n");
		return -1;
	}
	else{
		printf("Test passed\n");
	}	
	return 0;
}
