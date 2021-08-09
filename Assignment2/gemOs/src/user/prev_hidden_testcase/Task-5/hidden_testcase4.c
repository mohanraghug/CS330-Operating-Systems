
#include<ulib.h>
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
   int fd[2];
   if (pipe(fd) < 0)
   {
       printf("Testcase failed\n");
       return 0;
   }
   long sum = 0;
   long value_written = 0;
   int process_count = 500;

    // Writting 1 to 500 to pipe
   for(int i =1; i <= process_count; i++)
   {

       int temp = write(fd[1], &i, sizeof(int));
       value_written += temp;

       sum = sum + i;
   }

   int pid = fork();

   if(pid == 0)
   {
       long child_value_read = 0, child_value_written = 0, childsum = 0;
        // Reading the value and writing it to the pipe so that parent will read it
       for(int i =1; i <= process_count; i++)
       {
            int value = -1;
            int temp_read = read(fd[0], &value, sizeof(int));
            int temp_write = write(fd[1], &value, sizeof(int));
            child_value_read += temp_read;
            child_value_written += temp_write;
            childsum += value;
       }
       write(fd[1], &child_value_read, sizeof(long));
       write(fd[1], &child_value_written, sizeof(long));
       if(childsum != sum)
       {
            printf("Testcase failed\n");
       }
       sleep(1);
       exit(0);
   }

   sleep(3000);
   long test_sum = 0;
   long value_read = 0;

   // Reading the value written by the child
   for(int i =1; i<=process_count; i++)
   {
       int value = -1;
       int temp = read(fd[0], &value, sizeof(int));
       value_read += temp;

       test_sum = test_sum + value;
   }

    long child_value_read1 = 0, child_value_written1 = 0;

    read(fd[0], &child_value_read1, sizeof(long));
    read(fd[0], &child_value_written1, sizeof(long));


   long totalsize = 4*(process_count)*sizeof(int);
   long test_total_size =  value_written + value_read + child_value_read1 + child_value_written1;

   // comparing the sum and Total read and write sizes. 
   if ((sum == test_sum) && (test_total_size == totalsize))
   {
       printf("Testcase passed\n");
   } else {
       printf("Testcase failed\n");
   }
   return 0;
  }
