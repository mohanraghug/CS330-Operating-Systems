#include<ulib.h>
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	int cnt;	 
	struct message msg;

	// fd is NOT a msg_queue
	char *filename="test.txt";
	int fd = open(filename, O_CREAT|O_RDWR, O_READ|O_WRITE);

	cnt = msg_queue_rcv(fd, &msg);

	if (cnt != -EINVAL) {
		printf("Test failed\n");
	}
	else {
		printf("Test passed\n");
	}

	return 0;
}
