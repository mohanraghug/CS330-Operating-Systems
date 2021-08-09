
#include<ulib.h>
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	int fd, pid, st, ch, ppid;	 
	struct message msg;
	
	// parent creates message queue
	fd = create_msg_queue();
	// parents pid	
	ppid = getpid();	
	
	for(ch = 0; ch < 2; ++ch){
		pid = fork();
		if(pid == 0){
			// block parent
			st = msg_queue_block(fd, ppid);
			// dont exit till parent tries to send	
			sleep(200);
			break;
		}
	}
	// parent
	if(ch == 2){
		// ensure that everybody blocks	
		sleep(100);	
		// send a bcast message
		msg.from_pid = ppid;	
		msg.to_pid = BROADCAST_PID;
		msg.msg_txt[0] = 'B';
		msg.msg_txt[1] = '\0';
		st = msg_queue_send(fd, &msg);

		if(st == 0)
			printf("Test passed\n");
		else
			printf("Test failed\n");
	}
	return 0;
}
