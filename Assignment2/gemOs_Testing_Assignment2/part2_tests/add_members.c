#include<ulib.h>

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	int fd, pid, st, mypid, cnt = 0, i , j;	
	struct msg_queue_member_info info;
	int pids[MAX_MEMBERS];
	
	// parent creates message queue
	fd = create_msg_queue();
	pids[0] = getpid();
	
	pid = fork();
	if(pid == 0){
		// ensure that parent gets scheduled before 
		// child exit
		sleep(100);
	}
	else if(pid > 0){
		// parent
		st = get_member_info(fd, &info);
		pids[1] = pid;
		for(i = 0; i < 2; ++i){
			for(j = 0; j < 2; ++j){
				if(pids[i] == info.member_pid[j]){
					++cnt;
				}
			}
		}
		if(cnt == 2 && info.member_count == 2){
			printf("Test passed\n");
		}
		else{
			printf("Test failed\n");
		}
	}
	else{
		printf("Fork failed\n");
	}
	return 0;
}
