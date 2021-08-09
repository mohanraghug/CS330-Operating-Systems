
#include<ulib.h>

#define LEN 20

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	int pid = -1;;
	int ret = -1;
	char buf_r[LEN];
	char *buf_w = "Hello, I am file!";
	char *filename="test.txt";

	int fd_w = open(filename, O_CREAT|O_WRONLY, O_READ|O_WRITE);
	int fd_r = open(filename, O_RDONLY, O_READ);
        if((fd_w < 0) || (fd_r < 0))
        {
                printf("Test failed\n");
                return 0;
        }


	ret = write(fd_w, buf_w, 0);
	//printf("write ret: %d\n", ret);
	if(ret == 0)
		printf("Test passed\n");
	else
		printf("Test failed\n");

	return 0;
}
