// altering write read.

#include<ulib.h>
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
    int flag = 0;
    char buff2[200];
    char *filename="test.txt";
    char * buff1 = "The doodle is inspired by animated discs reflecting Plateau's style with different images and themes on various device platforms";
    int create_fd = open(filename, O_CREAT|O_RDWR, O_READ|O_WRITE);

    if(create_fd < 0){
        printf("open failed\n");
        exit(0);
    }

    int read_fd = open(filename, O_RDONLY);
    int i;
    for(i=0;i<10;i++){
        write( create_fd, buff1, 128);
        read(read_fd, buff2, 128);
        buff2[128] = '\0';
        int flag = ustrcmp(buff1,buff2);
        if(flag != 0){
            printf("Testcase failed\n");
            break;
        }
    }
    if(!flag)
        printf("Testcase passed\n");
    else
        printf("Testcase failed\n");

    close(read_fd);
    close(create_fd);
    return 0;
  }
