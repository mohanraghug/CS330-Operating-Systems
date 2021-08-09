// checks if a msg queue is opened
#include<ulib.h>
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	int st;	 
	
	st = msg_queue_close(0);
	if(st == 0)
		printf("Tests failed\n");
	else
		printf("Tests passed\n");
	return 0;
}
