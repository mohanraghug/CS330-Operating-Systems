#include <ulib.h>

int fn_2(int i);
int fn_1(int i);
int fn_2(int i)
{
    if (i == 0)
        return 0;
    printf("In fn2 i = %d\n", i);
    return fn_1(i - 1);
}

int fn_1(int i)
{
    if (i == 0)
        return 0;
    printf("In fn1 i = %d\n", i);
    return fn_2(i - 1);
}

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
    int cpid;
    long ret = 0;
    int i, bt_count;

    ret = become_debugger();

    cpid = fork();

    if (cpid < 0)
    {
        printf("Error in fork\n");
    }
    else if (cpid == 0)
    {
        printf("fn_1 : %x\n", fn_1);
        printf("fn_2 : %x\n", fn_2);
        fn_1(0);
        fn_2(0);
        fn_1(4);
    }
    else
    {
        ret = set_breakpoint(fn_1);
        ret = set_breakpoint(fn_2);

        // fn_1
        ret = wait_and_continue();
        printf("Breakpoint hit at : %x\n", ret);

        // fn_2
        ret = wait_and_continue();
        printf("Breakpoint hit at : %x\n", ret);

        // fn_1 4
        ret = wait_and_continue();
        printf("Breakpoint hit at : %x\n", ret);

        // fn_2 3
        ret = wait_and_continue();
        printf("Breakpoint hit at : %x\n", ret);

        // fn_1 2
        ret = wait_and_continue();
        printf("Breakpoint hit at : %x\n", ret);

        // fn_2 1
        ret = wait_and_continue();
        printf("Breakpoint hit at : %x\n", ret);

        // fn_1 0
        ret = wait_and_continue();
        printf("Breakpoint hit at : %x\n", ret);

        // for exit
        ret = wait_and_continue();
        printf("Child exit return : %x\n", ret);
    }

    return 0;
}