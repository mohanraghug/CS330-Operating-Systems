#include<ulib.h>
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	int fd, pid, cnt, m, r = 0;	 
	struct message  msg;
	
	// parent creates message queue
	fd = create_msg_queue();
	
	pid = fork();
	if(pid == 0){
		// wait 
		while(get_msg_count(fd) < 32);
		if(get_msg_count(fd) == 32)
			printf("Test passed\n");
		else 
			printf("Test failed\n");
	}
	else if(pid > 0){
		
		for(m = 0; m < 32; ++m){
			// send a message to child
			msg.from_pid = getpid();	
			msg.to_pid = pid;
			msg.msg_txt[0] = 'M';
			msg.msg_txt[1] = '\0';
			cnt = msg_queue_send(fd, &msg);
		}
	}
	else{
		printf("fork error\n");
	}
	return 0;
}
