
#include<ulib.h>
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
   int parent_pipe[2];
   if (pipe(parent_pipe) < 0){
       printf("Testcase failed\n");
       return 0;
   }
   char *buf = "Testcase Passed";
   write(parent_pipe[1], buf, 15);

  int pid = fork();
  
  if(pid == 0){
    /* Will be just creating, duping parent pipe. After that we will be closing all the pipe and dupped fd */
    int child[2], read_fd, write_fd;
    if (pipe(child) < 0){
        printf("Testcase failed\n");
        return 0;
    }
    read_fd = dup(child[0]);
    write_fd = dup(parent_pipe[1]);
    // closing dupped fd
    close(read_fd);
    close(write_fd);

    //closing child pipe
    close(child[0]);
    close(child[1]);

    //closing parent pipe
    close(parent_pipe[0]);
    close(parent_pipe[1]);
    exit(0);
  }else{

   sleep(50);
   // Reading the written value
   char result[16];
   read(parent_pipe[0], result, 15);
   result[16] = 0;
   printf("%s", result);
  }
   return 0;
}