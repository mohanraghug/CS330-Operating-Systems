
#include<ulib.h>
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	int fd, pid, st, mypid, cnt = 0, i , j;	 
	struct msg_queue_member_info info;
	// parent creates message queue
	fd = create_msg_queue();
	pid = fork();
	if(pid == 0){
		// ensure that parent gets scheduled before 
		// child exit
		sleep(100);
		// check if parent has left
		mypid = getpid();
		st = get_member_info(fd, &info);
		if(st == 0 && info.member_count == 1 && info.member_pid[0] == mypid){
			printf("Test passed\n");
		}
		else{
			printf("Test failed\n");
		}
	}
	else if(pid > 0){
		msg_queue_close(fd);	
	}
	else{
		printf("Fork failed\n");
	}
	
	return 0;
}
