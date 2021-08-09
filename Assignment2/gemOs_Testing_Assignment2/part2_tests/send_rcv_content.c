#include<ulib.h>
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	int fd, pid, cnt;	 
	struct message  msg1, msg2;
	
	// parent creates message queue
	fd = create_msg_queue();
	
	pid = fork();
	if(pid == 0){
		// wait for first msg
		while(get_msg_count(fd) == 0);
		cnt = msg_queue_rcv(fd, &msg1);
		
		// wait for second msg
		while(get_msg_count(fd) == 0);
		cnt = msg_queue_rcv(fd, &msg2);
		if((ustrcmp(msg1.msg_txt, "M1") == 0)
		&& (ustrcmp(msg2.msg_txt, "M2") == 0)
		)
			printf("Tests passed\n");
		else
			printf("Tests failed\n");
	}
	else if(pid > 0){
		
		// send a message to child
		msg1.from_pid = getpid();	
		msg1.to_pid = pid;
		msg1.msg_txt[0] = 'M';
		msg1.msg_txt[1] = '1';
		msg1.msg_txt[2] = '\0';
		
		// send message to child
		cnt = msg_queue_send(fd, &msg1);
		
		// send a message to child
		msg2.from_pid = getpid();	
		msg2.to_pid = pid;
		msg2.msg_txt[0] = 'M';
		msg2.msg_txt[1] = '2';
		msg2.msg_txt[2] = '\0';
		
		cnt = msg_queue_send(fd, &msg2);

	}
	else{
		printf("fork error\n");// Put test failed here?
	}
	return 0;
}
