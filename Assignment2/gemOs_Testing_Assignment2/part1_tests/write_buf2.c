
#include<ulib.h>

#define LEN 4096 
#define MAX 4096

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	char c = 'a';
	int pid = -1;;
	int ret = -1;
	int match = 1;
	char buf_r[4096];
	//char *buf_w = "Hello, I am file!";
	char buf_w[LEN];
	char *filename="test.txt";


	int fd_w = open(filename, O_CREAT|O_WRONLY, O_READ|O_WRITE);
	int fd_r = open(filename, O_RDONLY, O_READ);
        if((fd_w < 0) || (fd_r < 0))
        {
                printf("Test failed\n");
                return 0;
        }


	c = 'a';
	for(int i = 0; i < LEN; i++)
	{
		buf_w[i] = c;	
		if(c == 'z')
			c = 'a';
		else
			++c;
	}

	ret = write(fd_w, buf_w, MAX);
	//printf("write ret: %d\n", ret);
	if(ret != MAX)
	{
		printf("Test failed\n");
		return 0;
	}

	ret = read(fd_r, buf_r, MAX);
	//printf("read ret: %d\n", ret);
	if(ret != MAX)
	{
		printf("Test failed\n");
		return 0;
	}


	for(int i = 0; i < MAX; i++)
	{
		if(buf_r[i] != buf_w[i])
		{
			match = 0;
			break;
		}
	}

	if(match)
		printf("Test passed\n");
	else
		printf("Test failed\n");

	return 0;
}
