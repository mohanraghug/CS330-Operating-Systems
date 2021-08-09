#include<ulib.h>

//opening a file that doesnot exist.
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
    int pid;
    char buf1[20];
    char *filename="IdontExist.txt";
    int fd = open(filename, O_RDWR);
	if(fd == -1)
	{
		printf("Test passed\n");
	}
	else
	{
		printf("Test failed\n");
	}
    return 0;
}
