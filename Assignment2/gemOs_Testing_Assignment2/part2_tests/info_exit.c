#include<ulib.h>

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	int fd, pid, st;	 
	struct msg_queue_member_info info;
	
	// parent creates message queue
	fd = create_msg_queue();
	pid = fork();
	if(pid == 0){
		//child exit
	}
	else if(pid > 0){
		// wait till child exits	
		sleep(100);	
		st = get_member_info(fd, &info);
		
		if(info.member_count == 1 && info.member_pid[0] == getpid()){
			printf("Test passed\n");
		}
		else{
			printf("Test failed\n");
		}
	}
	else{
		printf("fork failed\n");
	}
	return 0;
}

