#include<ulib.h>

int fn_1(int a, int b, int c, int d, int e, int f)
{
	return a+b+c+d+e+f;
}


int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	int cpid;
	long long ret = 0;
	int i, bt_count;
        struct registers reg;
	
	ret = become_debugger();
	
	cpid = fork();
	
	if(cpid < 0){
		printf("Error in fork\n");
	}
	else if(cpid == 0){
		fn_1(0, 1, 2, 3, 4, 5);
	}
	else {
		ret = set_breakpoint(fn_1);
		ret = wait_and_continue();

        
		info_registers(&reg);

		//printf("%x %x %x %x %x\n", reg.entry_rip , reg.entry_rsp , reg.rbp , reg.rax , reg.rdi);
		//printf("%x %x %x %x %x\n", reg.rsi , reg.rdx , reg.rcx , reg.r8 , reg.r9);
 
		if((reg.entry_rip == 0x100000DE4) 
		&& (reg.entry_rsp == 0x7FFFFFEC0) 
		&& (reg.rbp == 0x7FFFFFFA8)
		&& (reg.rax == 0x0)
		&& (reg.rdi == 0)
		&& (reg.rsi == 1)
		&& (reg.rdx == 2)
		&& (reg.rcx == 3)
		&& (reg.r8 == 4)
		&& (reg.r9 == 5)) 
		{
			printf("Test passed\n");
		}
		else {
					// printf("%x %x %x %x %x %x %x %x %x %x\n", reg.entry_rip
                    //              , reg.entry_rsp
                    //              , reg.rbp
                    //              , reg.rax
					// 			 , reg.rdi
					// 			 , reg.rsi
					// 			 , reg.rdx
					// 			 , reg.rcx
					// 			 , reg.r8
					// 			 , reg.r9);

			printf("Test failed\n");
		}

		// ret = wait_and_continue();
	}
	
	return 0;
}
