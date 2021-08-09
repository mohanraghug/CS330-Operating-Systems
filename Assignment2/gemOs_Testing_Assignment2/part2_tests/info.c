
#include<ulib.h>
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	int fd, st, cnt = 0, i , j;	 
	struct msg_queue_member_info info;
	int pids[MAX_MEMBERS];
	
	// parent creates message queue
	fd = create_msg_queue();
	pids[0] = getpid();	
	
	for(i = 1; i < 4; ++i){
		pids[i] = fork();
		if(pids[i] == 0){
			// child sleeps for 100 timer ticks
			sleep(100);
			exit(0);	
		}
		else if(pids[i] > 0){
			// parent
		}
		else{
			printf("fork error\n");
		}
	}
		
	st = get_member_info(fd, &info);
	for(i = 0; i < 4; ++i){
		for(j = 0; j < 4; ++j){
			if(pids[i] == info.member_pid[j]){
				++cnt;
			}
		}
	}
	if(cnt == 4 && info.member_count == 4){
		printf("Test passed\n");
	}
	else{
		printf("Test failed\n");
	}

	return 0;
}
