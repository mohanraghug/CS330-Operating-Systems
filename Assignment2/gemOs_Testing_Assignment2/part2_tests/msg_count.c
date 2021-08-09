
#include<ulib.h>
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	int fd, pid, cnt;	 
	struct message msg;
	
	// parent creates message queue
	fd = create_msg_queue();
	
	pid = fork();
	if(pid == 0){
		// child must wait for the parent to send a message to it
		while(get_msg_count(fd) == 0);
		if(get_msg_count(fd) == 1){
			printf("Test passed\n");
		}
		else{
			printf("Test failed\n");
		}
		
	}
	else if(pid > 0){
		
		// send a message to child
		msg.from_pid = getpid();	
		msg.to_pid = pid;
		msg.msg_txt[0] = 'M';
		msg.msg_txt[1] = 'S';
		msg.msg_txt[2] = 'G';
		msg.msg_txt[3] = '\0';
		
		// send message to child
		cnt = msg_queue_send(fd, &msg);
	}
	else{
		printf("fork error\n");
	}
	return 0;
}
