
#include<ulib.h>
// Check if file is a msg_queue 
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	int fd;	 
	
	fd = create_msg_queue();
	if (!is_msg_queue(fd)) {
		printf("Test failed\n");
		return -1;
	}
	else{
		printf("Test passed\n");
	}
	return 0;

}
