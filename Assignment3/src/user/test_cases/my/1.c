#include <ulib.h>


void fn1(){

    long long int cnt = 0;
    
    for (int i = 0; i < 10000; i++)
    {
        cnt += i;
    }

    printf("cnt is %x\n", cnt);

}

int assert(int x){
    if(x == 0) {
        printf("Assert Failed !!\n");
        exit(0);
    }
    return 1;
}

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{

    int cpid;
    u64 ret = 0;
    int i, bt_count;
    unsigned long long bt_info[MAX_BACKTRACE];

    ret = become_debugger();

    cpid = fork();

    if(cpid == 0){
        fn1();
        fn1();
    }
    else{
        ret = set_breakpoint(fn1);
        assert(ret == 0);

        ret = wait_and_continue();
        assert(ret == (u64)fn1);

        ret = disable_breakpoint(fn1);
        assert(ret == 0);

        // exit
        ret = wait_and_continue();
        assert(ret == CHILD_EXIT);
    }

    return 0;
}
