
#include<ulib.h>
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	int fd, pid, st1, st2;	 
	struct msg_queue_member_info info;
	struct message msg;
	
	// parent creates message queue
	fd = create_msg_queue();
	
	pid = fork();
	if(pid == 0){
		sleep(100);
	}
	else if(pid > 0){

		st1 = msg_queue_block(fd, pid);
		st2 = msg_queue_block(fd, 100);

		if(st1 == 0 && st2 == -EINVAL)
			printf("Test passed\n");
		else
			printf("Test failed\n");
	}
	else{
		printf("fork error\n");
	}
	return 0;
}
