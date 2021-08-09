
#include<ulib.h>
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
  int fd[2];
   if (pipe(fd) < 0)
   {
       printf("Testcase failed\n");
       return 0;
   }

   int copy_read = dup(fd[0]);
   int copy_read2 = dup2(copy_read, 12);

   if(copy_read < 0 && copy_read2 <0)
   {
       printf("Testcase failed\n");
       return 0;
   }

   int copy_write = dup(fd[1]);
   int copy_write2 = dup2(copy_write, 13);

   if(copy_write < 0 && copy_write2 <0)
   {
       printf("Testcase failed\n");
       return 0;
   }


   long sum = 0;
   long value_written = 0;
   int process_count = 500;
   for(int i =1; i <= process_count; i++)
   {
       int choose_fd = -1;
       int index = i % 3;
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

   int pid = fork();

   if(pid == 0)
   {
       long child_value_read = 0, child_value_written = 0, childsum = 0;
       for(int i =1; i <= process_count; i++)
       {
            int choose_write_fd = -1, choose_read_fd = -1;
            int index = i % 3;
            if(index == 0)
            {
                choose_write_fd = fd[1];
                choose_read_fd = fd[0];
            } else if (index == 1)
            {
                choose_write_fd = copy_write;
                choose_read_fd = copy_read;
            } else
            {
                choose_write_fd = copy_write2;
                choose_read_fd = copy_read2;
            }
            int value = -1;
            int temp_read = read(choose_read_fd, &value, sizeof(int));
            int temp_write = write(choose_write_fd, &value, sizeof(int));
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
   for(int i =1; i<=process_count; i++)
   {
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
       int value = -1;
       int temp = read(choose_fd, &value, sizeof(int));
       value_read += temp;

       test_sum = test_sum + value;
   }

    long child_value_read1 = 0, child_value_written1 = 0;

    read(fd[0], &child_value_read1, sizeof(long));
    read(fd[0], &child_value_written1, sizeof(long));


   long totalsize = 4*(process_count)*sizeof(int);
   long test_total_size =  value_written + value_read + child_value_read1 + child_value_written1;

   if ((sum == test_sum) && (test_total_size == totalsize))
   {
       printf("Testcase passed\n");
   } else {
       printf("Testcase failed\n");
   }
   return 0;
  }
