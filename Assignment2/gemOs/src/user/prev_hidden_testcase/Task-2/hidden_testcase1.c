
#include<ulib.h>
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
    int input_fd;
    int output_fd;
    input_fd = dup(0);
    output_fd = dup(1);
    
    //Changing the STD OUT fd. Below line should not be printed
    dup2(input_fd, 1);
    printf("Testcase Failed\n");

    // Reseting back the STDOUT. Below line should be printed
    int val = dup2(output_fd,1);
    printf("Testcase Passed\n");
    return 0;
}
