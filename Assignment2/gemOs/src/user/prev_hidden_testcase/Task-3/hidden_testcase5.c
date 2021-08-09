
#include<ulib.h>
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
  int fd[12];
  // Creating size pipes
   for(int i = 0; i< 12; i = i+2)
   {
           if (pipe((&fd[i])) < 0)
           {

                   printf("Testcase failed\n");
                   return 0;
           }
   }

   u64 sum = 0;
   u64 value_written = 0;
   int process_count = 5;

   int write_end = 1;
   // Writing the value Write end of the six pipes.
   for(int i =1; i <= process_count; i++)
   {
       int temp = write(fd[write_end], &i, sizeof(int));
       value_written += temp;
       sum = sum + i;

       write_end += 2;
       if(write_end >= 12)
       {
           write_end = 1;
       }
   }

   u64 test_sum = 0;
   u64 value_read = 0;

   int read_end = 0;
   // Reading the value from the read end of six pipes and sum the value
   for(int i =1; i<=process_count; i++)
   {
       int value = -1;
       int temp = read(fd[read_end], &value, sizeof(int));

       value_read += temp;

       test_sum = test_sum + value;
       read_end += 2;
       if(read_end >= 12)
       {
           read_end = 0;
       }
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