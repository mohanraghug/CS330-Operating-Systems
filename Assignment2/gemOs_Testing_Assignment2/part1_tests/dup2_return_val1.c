
#include<ulib.h>

#define LEN 4096

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	int pid;
	char buf_r[4096];
	char buf_w[4096];
	char *filename="test.txt";
	int fd_w = open(filename, O_CREAT|O_WRONLY, O_READ|O_WRITE);
	int fd_r = open(filename, O_RDONLY, O_READ);
        if((fd_w < 0) || (fd_r < 0))
        {
                printf("Test failed\n");
                return 0;
        }

	int fd_dup2 = dup2(10, 5);
	//printf("fd_dup2: %d\n", fd_dup2);

	if(fd_dup2 == -EINVAL)
	{
		printf("Test passed\n");
	}	
	else
	{
		printf("Test failed\n");
	}

	return 0;
}
