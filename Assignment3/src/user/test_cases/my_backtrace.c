#include <ulib.h>

void fn_1(int x)
{
    printf("x = %d\n", x);
    if (x == 0)
        return;
    fn_1(x - 1);
}

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
    int cpid;
    long ret = 0;
    int i, bt_count;
    unsigned long long bt_info[MAX_BACKTRACE];

    ret = become_debugger();

    cpid = fork();

    if (cpid < 0)
    {
        printf("Error in fork\n");
    }
    else if (cpid == 0)
    {
        fn_1(4);
    }
    else
    {
        ret = set_breakpoint(fn_1);

        for (int i = 4; i >= 0; i--)
        {
            ret = wait_and_continue();

            printf("BACKTRACE INFORMATION\n");
            bt_count = backtrace((void *)&bt_info);

            printf("Backtrace count: %d\n", bt_count);
            for (int i = 0; i < bt_count; i++)
            {
                printf("#%d %x\n", i, bt_info[i]);
            }
        }

        // for exit
        ret = wait_and_continue();
    }

    return 0;
}
