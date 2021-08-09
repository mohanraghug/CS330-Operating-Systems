
#include<ulib.h>
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	int fd, pid, st, mypid, ppid;	 
	struct msg_queue_member_info info;
	struct message msg;
	
	// parent creates message queue
	fd = create_msg_queue();
	ppid = getpid();	
	pid = fork();
	if(pid == 0){
		// block the parent
		st = msg_queue_block(fd, ppid);
		// ensure that child doesnt exit
		sleep(200);
	}
	else if(pid > 0){
		// wait for child to block
		sleep(50);
		
		// send a message to child
		msg.from_pid = getpid();	
		msg.to_pid = pid;
		msg.msg_txt[0] = 'B';
		msg.msg_txt[1] = '\0';
		st = msg_queue_send(fd, &msg);

		if(st == -EINVAL)
			printf("Test passed\n");
		else
			printf("Test failed\n");
	}
	else{
		printf("fork error\n");
	}
	return 0;
}
