#include<ulib.h>

int recurse(int n)
{
	if(n == 0){
		return 0;
	}	
	return recurse(n - 1) + 1;
}

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	// check if everything works fine on recursion
	int cpid;
	long ret = 0;
	int i, bt_count, ok = 1;
	
	ret = become_debugger();
	
	cpid = fork();
	
	if(cpid < 0){
		printf("Error in fork\n");
	}
	else if(cpid == 0){
		recurse(5);	
	}
	else{
		ret = set_breakpoint(recurse);
		
		for(i = 0; i < 6; ++i){
			ret = wait_and_continue();
			// break point address must be same
			if(ret != (long)recurse)
				ok = 0;
		}
		if(ok)
			printf("Test passed\n");
		else
			printf("Test failed\n");
		// for exit
		ret = wait_and_continue();	
	}
	
	return 0;
}
