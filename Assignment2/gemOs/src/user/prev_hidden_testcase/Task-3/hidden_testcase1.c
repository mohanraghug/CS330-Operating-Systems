
#include<ulib.h>
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
   int fd[2];
   if (pipe(fd) < 0){
       printf("Testcase failed\n");
       return 0;
   }
   int total = 0;
   char *one = "123456789";

    // Writing 9 Bytes;
   total += write(fd[1], one, 9);

    // Reading all the written bytes
   char result [18];
   for(int i =0; i< 3; i++){
       int read_val = read(fd[0], result, 3);
       total += read_val;
   }

   if(total != 18){
        printf("Testcase failed\n");
        return 0;
   }
    // Pipe is empty. We are trying to read
   int invalid_read = read(fd[0], result, 1);

   if(invalid_read <= 0)
   {
        printf("Testcase Passed\n");
        return 0;
   }

   printf("Testcase failed\n");
   return 0;
}
