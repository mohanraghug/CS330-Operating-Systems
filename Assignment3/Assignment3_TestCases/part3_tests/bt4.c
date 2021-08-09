#include<ulib.h>


int fn_2()
{
	printf("");
	return 0;
}

int fn_1(int a)
{
	printf("");	
	if(a == (MAX_BACKTRACE-2))
	{
		fn_2();
		return 0;
	}
	fn_1(a+1);
	printf("");
	return 0;
}

int fn_3()
{
	printf("");
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
		fn_1(1);	
		fn_3();
	}
	else{
		ret = set_breakpoint(fn_2);
		ret = set_breakpoint(fn_3);
		ret = wait_and_continue();
		
		/////////////////
		//1st breakpoint
		/////////////////

		//expected values
		exp_bt_count = MAX_BACKTRACE;
		exp_bt_info[0] = 0x100000DA4 + 0x40;
		exp_bt_info[1] = 0x100000DEC + 0x40;
		for(int i = 2; i < MAX_BACKTRACE-1; i++)
		{
			exp_bt_info[i] = 0x100000E00 + 0x40;
		}
		exp_bt_info[MAX_BACKTRACE-1] = 0x100000ECF + 0x40;

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



		//resume debuggee
		ret = wait_and_continue();	

		/////////////////
		//2nd breakpoint
		/////////////////


		//expected values
		exp_bt_count = 2;
		exp_bt_info[0] = 0x100000E18 + 0x40;
		exp_bt_info[1] = 0x100000ED9 + 0x40;

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
		//ret = wait_and_continue();	
	}
	
	return 0;
}
