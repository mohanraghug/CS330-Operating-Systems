
#include<ulib.h>
// no file descriptor available for msg_queue creation
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	char *filename="test.txt";
	int fd;

	for (int i=3; i<16; i++) {
		fd = open(filename, O_WRONLY|O_CREAT, O_WRITE);
	}

	fd = create_msg_queue();

	if (fd >= 0) {
		printf("Test failed\n");
		return -1;
	}

	printf("Test passed\n");
	return 0;

}
