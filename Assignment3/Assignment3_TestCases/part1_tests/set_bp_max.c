#include<ulib.h>

int fn_1()
{
	return 1;
}

int fn_2()
{
	return 2;
}

int fn_3()
{
	return 3;
}

int fn_4()
{
	return 4;
}

int fn_5()
{
	return 5;
}

int fn_6()
{
	return 6;
}

int fn_7()
{
	return 7;
}

int fn_8()
{
	return 8;
}

int fn_9()
{
	return 9;
}

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	// checks if error is returned on setting more than max breakpoints 	
	int cpid = 0;
	long ret = 0;
	
	ret = become_debugger();
	
	ret = set_breakpoint(fn_1);
	ret = set_breakpoint(fn_2);
	ret = set_breakpoint(fn_3);
	ret = set_breakpoint(fn_4);
	ret = set_breakpoint(fn_5);
	ret = set_breakpoint(fn_6);
	ret = set_breakpoint(fn_7);
	ret = set_breakpoint(fn_8);
	ret = set_breakpoint(fn_9);
	
	// check if -1 is returned 
	if(ret == -1){
		printf("Test passed\n");	
	}
	else{
		printf("Test failed\n");
	}

	return 0;
}
