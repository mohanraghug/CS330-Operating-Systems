#include<ulib.h>

int fn_1()
{
	int a = 10, b = 20;
	return a + b;
}


int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	// check if break points are hit
	int cpid;
	long ret = 0;
	int i, ok = 1;
		
	ret = become_debugger();
	
	cpid = fork();
	
	if(cpid < 0){
		printf("Error in fork\n");
	}
	else if(cpid == 0){
		fn_1();
	}
	else{
		// check return value of wait_and_continue on child exit
		ret = set_breakpoint(fn_1);
		
		// fn_1 
		ret = wait_and_continue();
		
		// for exit
		ret = wait_and_continue();	
		if(ret == CHILD_EXIT)
			printf("Test passed\n");
		else
			printf("Test failed\n");
	}
	
	return 0;
}
