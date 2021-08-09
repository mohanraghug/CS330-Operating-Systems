
#include<ulib.h>
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
    int input_fd = 0;
    int output_fd = 1;

    // Invalid fd , dup2 should return -1
    int invalid_dup2 = dup2(15999, 0);
    if(!(invalid_dup2 < 0)) 
    {
        printf("Testcase Failed\n");
        return 0;
    }

    // Setting new fd in 15 and 5
    int val_15 = dup2(output_fd,15);
    int val_5 = dup2(input_fd,5);

    for(int i =0; i< 11; i++)
    {
        int dupedval = dup(0);

        // dup should look for new fd which are free. (0 to 2), 5, 15 are already taken
        if (((dupedval >= 0 && dupedval <= 2) || dupedval == 5 || dupedval == 15 ||dupedval <0))
        {
            printf("Testcase Failed\n");
            return 0;
        }
        printf(" %d \n", dupedval);
    }
    // Total 16 fd are already been alloacted. Below fd should result i
    int invalid_dup = dup(input_fd);

    if(!(invalid_dup<0))
    {
        printf("Testcase Failed %d \n", invalid_dup);
        return 0;
    }
    printf("Testcase Passed \n");
    return 0;
}
