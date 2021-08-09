#include<ulib.h>

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
    int pid;
    char buf1[20];
    char *filename="test.txt";
    int fd = open(filename, O_RDONLY|O_CREAT, O_READ);
	//printf("fd: %d\n", fd);
	if(fd == 3)
	{
		printf("Test passed\n");
	}
	else
	{
		printf("Test failed\n");
	}

    return 0;
}
