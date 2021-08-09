#include<ulib.h>
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	int st;	 
	struct msg_queue_member_info info;
	
	st = get_member_info(0, &info);
	if(st != -EINVAL)
		printf("Tests failed\n");
	else
		printf("Tests passed\n");
	return 0;
}
