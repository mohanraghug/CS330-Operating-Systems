
#include<ulib.h>
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
   int parent_pipe[2];
   if (pipe(parent_pipe) < 0){
       printf("Testcase failed\n");
       return 0;
   }
   // Writing to the pipe
   char *buf = "Testcase Passed";
   write(parent_pipe[1], buf, 15);

  int pid = fork();
  
  if(pid == 0){
    // Wait till parent exit and close the pipe and 
    //then try to read it from the pipe
    sleep(30);
    
    char result[16];
    read(parent_pipe[0], result, 15);
    result[16] = 0;
    printf("%s", result);
    exit(0);
  }else{

    close(parent_pipe[0]);
    close(parent_pipe[1]);
     
  }
   return 0;
}