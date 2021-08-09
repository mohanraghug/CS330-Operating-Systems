#include<ulib.h>

int fn_3()
{
	printf("");	
	return 0;
}

int fn_2()
{
	printf("");	
	fn_3();
	return 0;
}

int fn_1()
{
	printf("");	
	fn_2();
	return 0;
}


int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	int i = 0;
	int cpid = 0;
	long ret = 0;
	int exp_bt_count = 0;				//expected backtrace count
	int gen_bt_count = 0;				//generated backtrace count
	unsigned long long exp_bt_info[MAX_BACKTRACE];	//expected backtrace info
	unsigned long long gen_bt_info[MAX_BACKTRACE];	//generated backtrace info
	
	
	ret = become_debugger();
	
	cpid = fork();
	
	if(cpid < 0){
		printf("Error in fork\n");
	}
	else if(cpid == 0){
		fn_1();	
	}
	else{
		ret = set_breakpoint(fn_2);
		ret = set_breakpoint(fn_3);
		ret = disable_breakpoint(fn_2);	

		ret = wait_and_continue();
		

		//expected values
		exp_bt_count = 4;
		exp_bt_info[0] = 0x100000DA4 + 0x40;
		exp_bt_info[1] = 0x100000DDF + 0x40;
		exp_bt_info[2] = 0x100000E05 + 0x40;
		exp_bt_info[3] = 0x100000EA7 + 0x40;

		//generated values
		gen_bt_count = backtrace((void*)&gen_bt_info);

		//Test failed or passed?
		if(gen_bt_count != exp_bt_count)
		{
			printf("Test failed\n");
			return 0;
		}
		
		
		//printf("Backtrace count: %d\n", gen_bt_count);
		for(int i = 0; i < exp_bt_count; i++)
		{
			//printf("#%d exp: %x, gen: %x\n", i, exp_bt_info[i], gen_bt_info[i]);
			if(gen_bt_info[i] != exp_bt_info[i])
			{
				printf("Test failed\n");
				return 0;
			}
		
		}
		printf("Test passed\n");
		
		// for exit
		//ret = wait_and_continue();	
	}
	
	return 0;
}
