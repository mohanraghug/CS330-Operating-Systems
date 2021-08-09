#include<ulib.h>

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
    int pid;
    char buf1[20];
    char *filename="test.txt";
    int fd = 9999;
	int largest_fd = -1;


	while(fd>=0)
	{
		fd = open(filename, O_WRONLY|O_CREAT, O_WRITE);
		if(fd >= 0)
			largest_fd = fd;
		
		//printf("fd: %d\n", fd);
	}
	if(largest_fd == 15)
	{
		printf("Test passed\n");
	}
	else
	{
		printf("Test failed\n");
	}


    return 0;
}
