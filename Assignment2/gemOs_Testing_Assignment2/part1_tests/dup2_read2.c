
#include<ulib.h>

#define LEN 1024 
#define MAX 64 

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	char c = 'a';
	int ret = 0;
	int match = 1;
	char buf_r[LEN];
	char buf_w1[LEN];
	char buf_w2[LEN];
	char *filename1="test1.txt";
	char *filename2="test2.txt";

	int fd_w1 = open(filename1, O_CREAT|O_WRONLY, O_READ|O_WRITE);
	int fd_r1 = open(filename1, O_RDONLY, O_READ);
        if((fd_w1 < 0) || (fd_r1 < 0))
        {
                printf("Test failed\n");
                return 0;
        }


	int fd_w2 = open(filename2, O_CREAT|O_WRONLY, O_READ|O_WRITE);
	int fd_r2 = open(filename2, O_RDONLY, O_READ);
        if((fd_w2 < 0) || (fd_r2 < 0))
        {
                printf("Test failed\n");
                return 0;
        }




	//Prepare data to write to files
        c = 'a';
        for(int i = 0; i < LEN; i++)
        {
                buf_w1[i] = c;
                if(c == 'z')
                        c = 'a';
                else
                        ++c;
        }

        c = 'A';
        for(int i = 0; i < LEN; i++)
        {
                buf_w2[i] = c;
                if(c == 'Z')
                        c = 'A';
                else
                        ++c;
        }

	//write data to files
        ret = write(fd_w1, buf_w1, LEN);
        //printf("write ret: %d\n", ret);
        if(ret != LEN)
	{
                printf("Test failed\n");
		return 0;
	}

        ret = write(fd_w2, buf_w2, LEN);
        //printf("write ret: %d\n", ret);
        if(ret != LEN)
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



	//read data
        ret = read(fd_r2, buf_r, MAX);
        //printf("read ret: %d\n", ret);
        //printf("buf_r: %s\n", buf_r);
        if(ret != MAX)
	{
                printf("Test failed\n");
		return 0;
	}
	


	//check whether correct data is read
        for(int i = 0; i < MAX; i++)
        {
                if(buf_r[i] != buf_w1[i])
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
