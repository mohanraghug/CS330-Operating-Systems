
#include<ulib.h>
void std_out_to_pipe(int parent_pipe[2])
{
      int input_fd;
      int output_fd;

      input_fd = dup(0);
      output_fd = dup(1);
      // Setting the pipe write end as STD OUT;
      dup2(parent_pipe[1], 1);
      // This should be written to pipe
      printf("Testcase Passed\n");
}

void close_pipe(int parent_pipe[2])
{
   close(parent_pipe[0]);
   close(parent_pipe[1]);
}

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
   int parent_pipe[2];
   if (pipe(parent_pipe) < 0){
       printf("Testcase failed\n");
       return 0;
   }
  // Forking
  int pid = fork();
  
  if(pid == 0){
    //Child1 again fork
    int child_pid = fork();
    if(child_pid == 0)
    {
      //Child2 wait till child 1 exits. 
      sleep(20);

      // Redirecting the STDOUT to the pipe
      std_out_to_pipe(parent_pipe);

      // Closing pipe
      close_pipe(parent_pipe);

    } else
    {
      // Closing pipe
      close_pipe(parent_pipe);
    }
    
  }else{
      // Parent sleep 
      sleep(100);

      // REad the value written by the child2
      char result[20];
      int readval = read(parent_pipe[0], result, 16);
      result[17] = '\0';
      printf("%s", result);
      close_pipe(parent_pipe);
  }
   return 0;
}