#include<ulib.h>

int fn_1()
{
	int a = 10, b = 20, c;
	c = a + b;
	return c;
}

int fn_2()
{
	int a = 1, b = 2, c;
	c = a + b;
	return c;
}

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	// checks if the address space is correctly modified	
	int cpid = 0;
	long ret = 0;
	char *addr_fn_1 = (char *)fn_1;	
	char *addr_fn_2 = (char *)fn_2;
	char int3_opcode = 0xcc;
	
	ret = become_debugger();
	
	ret = set_breakpoint(fn_1);
	ret = set_breakpoint(fn_2);
	// check if the list is modified 
	if(*addr_fn_1 == int3_opcode && *addr_fn_2 == int3_opcode){
		printf("Test passed\n");	
	}
	else{
		printf("Test failed\n");
	}

	return 0;
}
