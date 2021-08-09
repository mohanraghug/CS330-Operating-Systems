
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
	int fd_w1 = open(filename, O_CREAT|O_WRONLY, O_READ|O_WRITE);
	int fd_w2 = dup2(fd_w1, fd_w1+1);
	int fd_r = open(filename, O_RDONLY, O_READ);
        if((fd_w1 < 0) || (fd_w2 < 0) || (fd_r < 0))
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
        ret = write(fd_w1, buf_w, MAX);
        //printf("write ret: %d\n", ret);
	if(ret != MAX)
	{
		printf("Test failed\n");
		return 0;
	}

        ret = write(fd_w2, buf_w+MAX, MAX);
        //printf("write ret: %d\n", ret);
	if(ret != MAX)
	{
		printf("Test failed\n");
		return 0;
	}

        ret = write(fd_w1, buf_w+MAX+MAX, MAX);
        //printf("write ret: %d\n", ret);
	if(ret != MAX)
	{
		printf("Test failed\n");
		return 0;
	}



	//read 
        ret = read(fd_r, buf_r, 3*MAX);
        //printf("read ret: %d\n", ret);
        //printf("buf_r: %s\n", buf_r);
	if(ret != 3*MAX)
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
