
#include<ulib.h>
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
   int fd[2];
   if (pipe(fd) < 0)
   {
       printf("Testcase failed\n");
       return 0;
   }
   u64 sum = 0;
   u64 value_written = 0;
   int process_count = 500;

   // Writting 1 to 500 to pipe
   for(int i =1; i <= process_count; i++)
   {
       
       int temp = write(fd[1], &i, sizeof(int));
       value_written += temp; 

       sum = sum + i;
   }
   u64 test_sum = 0;
   u64 value_read = 0;

    // Reading the written values from the pipe and sum it up
   for(int i =1; i<=process_count; i++)
   {
       int value = -1;
       int temp = read(fd[0], &value, sizeof(int));

       value_read += temp;

       test_sum = test_sum + value; 
   }
   u64 totalsize = 2*(process_count)*sizeof(int);
   u64 test_total_size =  value_written + value_read;

    // comparing the sum and Total read and write sizes.
   if ((sum == test_sum) && (test_total_size == totalsize))
   {
       printf("Testcase passed\n");
   } else {
       printf("Testcase failed\n");
   }
   return 0;
}