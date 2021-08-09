#include<ulib.h>

int fn_1()
{
	int a = 10, b = 20, c;
	c = a + b;
	return c;
}

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	// checks if the breakpoint list is correctly set
	// on setting same breakpoint twice	
	int cpid = 0;
	long ret = 0;
	int ok = 1;	
	ret = become_debugger();
	
	ret = set_breakpoint(fn_1);
	if(count_breakpoints(fn_1) != 1){
		ok = 0;
	}
	
	ret = set_breakpoint(fn_1);
	if(count_breakpoints(fn_1) != 1){
		ok = 0;
	}
	
	if(ok)
		printf("Test passed\n");
	else 
		printf("Test failed\n");
	return 0;
}
