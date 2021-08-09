
#include<ulib.h>
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
   int fd[2];
   if (pipe(fd) < 0)
   {
       printf("Testcase failed\n");
       return 0;
   }
   // Duping the read ends
   int copy_read = dup(fd[0]);
   int copy_read2 = dup2(copy_read, 12);

   if(copy_read < 0 && copy_read2 <0)
   {
       printf("Testcase failed\n");
       return 0;
   }

    // Duping the write ends
   int copy_write = dup(fd[1]);
   int copy_write2 = dup2(copy_write, 13);

   if(copy_write < 0 && copy_write2 <0)
   {
       printf("Testcase failed\n");
       return 0;
   }

   u64 sum = 0;
   u64 value_written = 0;
   int process_count = 500;
   for(int i =1; i <= process_count; i++)
   {
       int choose_fd = -1;
       int index = i % 3;

       // Writing in each dup write ends
       if(index == 0)
       {
           choose_fd = fd[1];
       } else if (index == 1)
       {
           choose_fd = copy_write;
       } else
       {
           choose_fd = copy_write2;
       }
       
       
       int temp = write(choose_fd, &i, sizeof(int));
       value_written += temp; 

       sum = sum + i;
   }
   u64 test_sum = 0;
   u64 value_read = 0;

   for(int i =1; i<=process_count; i++)
   {
       int value = -1;

       int choose_fd = -1;
       int index = i % 3;
       if(index == 0)
       {
           choose_fd = fd[0];
       } else if (index == 1)
       {
           choose_fd = copy_read;
       } else
       {
           choose_fd = copy_read2;
       }
       // Reading the written values and sum it up
       int temp = read(choose_fd, &value, sizeof(int));

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