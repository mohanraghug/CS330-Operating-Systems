
#include<ulib.h>
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
   int fd[2];
   if (pipe(fd) < 0){
       printf("Testcase failed\n");
       return 0;
   }
   int pipe_size = 4096;

   char *one = "a";
   int total = 0;
   // Filling the entire pipe (4096);
   for(int i =0; i< 4096; i++){
       int written = write(fd[1], one, 1);
       if(written < 0)
       {
           printf("Testcase failed1\n");
           return 0;
       }
       total += written;
   }

   if(total != 4096){
        printf("Testcase failed2\n");
        return 0;
   }
   
   total = 0;
   char read_bytes[4200];
   // Reading 4 bytes;
   total += read(fd[0], read_bytes, 4);
   
   char *pass = "PASS";
   // Writing 4 bytes, it should start writing from the front of the pipe
   total += write(fd[1], pass, 4);

   // Reading 4092 bytes, so that read pointer will move to the front.
   total += read(fd[0], read_bytes, 4092);
  

   char result[5];
   // Reading the value from front which should be equal to "PASS"
   read(fd[0], result, 4);

   for(int i=0; i<4; i++){
       if(pass[i] != result[i]){
            printf("Testcase failed\n");
            return 0;
       }
   }

   printf("Testcase Passed\n");
   return 0;
}