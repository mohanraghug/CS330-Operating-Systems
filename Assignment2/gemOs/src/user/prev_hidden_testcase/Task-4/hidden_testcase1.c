
#include<ulib.h>
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
   int fd[2], read_fd, write_fd;
   if (pipe(fd) < 0){
       printf("Testcase failed\n");
       return 0;
   }
   // Duping the Read and write ends of the pipe
   read_fd = dup(fd[0]);
   write_fd = dup(fd[1]);
   
   // Closing the read end and dupped write end of the pipe
   close(fd[0]);
   close(write_fd);

   char *buf = "Testcase Passed";

   // Trying to write on the closed dupped write end. Which is invalid
   int invalid_write =  write(write_fd, buf, 15);
   if(invalid_write >= 0)
   {
       printf("Testcase failed\n");
       return 0;
   }
   // Writing to the valid write end the pipe
   write(fd[1], buf, 15);


   char result[16];

    // Trying to read from the closed read end of the pipe. Which is invalid
   int invalid_read =  read(fd[0], result, 15);
   if(invalid_read >= 0)
   {
       printf("Testcase failed\n");
       return 0;
   }

   // Reading it from the valid read end of the pipe
   read(read_fd, result, 15);

   result[16] = 0;

   printf("%s", result);
   return 0;
}
