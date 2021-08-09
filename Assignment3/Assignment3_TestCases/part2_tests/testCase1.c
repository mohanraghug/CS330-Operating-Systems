#include<ulib.h>

int fn_2()
{
	// printf("In fn2\n");	
	return 0;
}

int fn_1()
{
	// printf("In fn1\n");	
	return 0;
}


int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	int cpid;
	long long ret = 0;
	int i, bt_count;
	
	ret = become_debugger();
	
	cpid = fork();
	
	if(cpid < 0){
		printf("Error in fork\n");
	}
	else if(cpid == 0){
		// printf("fn_1 : %x\n", fn_1);
		// printf("fn_2 : %x\n", fn_2);
        for (int i=0; i<3; i++) {
            fn_1();
            fn_2();
        }

	}
	else {
		ret = set_breakpoint(fn_1);
		ret = set_breakpoint(fn_2);
		// ret = set_breakpoint(fn_2);

        disable_breakpoint(fn_1);
        disable_breakpoint(fn_2);

        if (check_enabled(fn_2) == 1) {
			printf("Test failed\n");
			return -1;
		}

        if (check_enabled(fn_1) == 1) {
			printf("Test failed\n");
			return -1;
		}


		// ret = wait_and_continue();
		// if (ret != CHILD_EXIT) {
		// 	// printf("ret was %x", ret);
		// 	// printf("fn_2 was %x", fn_2);
		// 	printf("Test failed\n");
		// 	return -1;
		// }

		printf("Test passed\n");
	}
	
	return 0;
}
