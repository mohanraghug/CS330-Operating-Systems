#include<ulib.h>

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	int fd, pid, st, mypid, cnt = 0, i , j, ch;	
	struct msg_queue_member_info info;
	int pids[MAX_MEMBERS];
	
	// parent creates message queue
	fd = create_msg_queue();
	
	pids[0] = getpid();
	for(ch = 0; ch < 3; ++ch){
		pid = fork();
		if(pid == 0){
			sleep(100);
			break;
		}
		else if(pid > 0){
			pids[ch + 1] = pid;
		}
	}

	if(ch == 3){
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
	}
	return 0;
}
