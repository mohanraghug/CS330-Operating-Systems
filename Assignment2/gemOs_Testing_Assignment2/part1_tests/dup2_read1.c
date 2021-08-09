
#include<ulib.h>

#define LEN 4096
#define MAX 8

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	char c = 'a';
	int ret = 0;
	int match = 1;
	char buf_r[4096];
	char buf_w[4096];
	char *filename="test.txt";
	int fd_w = open(filename, O_CREAT|O_WRONLY, O_READ|O_WRITE);
	int fd_r1 = open(filename, O_RDONLY, O_READ);
	int fd_r2 = fd_r1 + 1;
        if((fd_w < 0) || (fd_r1 < 0))
        {
                printf("Test failed\n");
                return 0;
        }



	//Prepare data to write to file
        c = 'a';
        for(int i = 0; i < LEN; i++)
        {
                buf_w[i] = c;
                if(c == 'z')
                        c = 'a';
                else
                        ++c;
        }
	//write data to file
        ret = write(fd_w, buf_w, 3*MAX);
        //printf("write ret: %d\n", ret);
        if(ret != 3*MAX)
	{
                printf("Test failed\n");
		return 0;
	}


	//duplicate read related file descriptor
	int fd_dup2 = dup2(fd_r1, fd_r2);
	if(fd_dup2 != fd_r2)
	{
		printf("Test failed\n");
		return 0;
	}



	//read using both file descriptors
        ret = read(fd_r1, buf_r, MAX);
        //printf("read ret: %d\n", ret);
        //printf("buf_r: %s\n", buf_r);
	if(ret != MAX)
	{
		//printf("Test failed\n");
		return 0;
	}

        ret = read(fd_r2, buf_r+MAX, MAX);
        //printf("read ret: %d\n", ret);
        //printf("buf_r: %s\n", buf_r);
	if(ret != MAX)
	{
		printf("Test failed\n");
		return 0;
	}

        ret = read(fd_r1, buf_r+MAX+MAX, MAX);
        //printf("read ret: %d\n", ret);
        //printf("buf_r: %s\n", buf_r);
	if(ret != MAX)
	{
		printf("Test failed\n");
		return 0;
	}


	//check whether correct data is read
        for(int i = 0; i < 3*MAX; i++)
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
