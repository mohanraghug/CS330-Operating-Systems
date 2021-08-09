
#include<ulib.h>
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
   int fd[2];
   if (pipe(fd) < 0){
       printf("Testcase failed\n");
       return 0;
   }
    int input_fd;
    int output_fd;

    input_fd = dup(0);
    output_fd = dup(1);
    
    // Setting the pipe write end as STD OUT;
    dup2(fd[1], 1);
    
    // This should be written to pipe
    printf("Testcase Passed\n");

    char result[20];

    int readval = read(fd[0], result, 16);

    result[17] = '\0';

    // Reseting the Default STDOUT
    int val = dup2(output_fd,1);
    if(readval > 0)
    {
        printf("%s", result);
    } else
    {
         printf("Testcase failed\n");
    }
   return 0;
}
