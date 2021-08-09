

// tests create_msg_queue
// msg_queue_snd
// msg_queue_rcv

#include<ulib.h>
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	int fd, pid, cnt;	 
	struct msg_queue_member_info info;
	struct message msg;
	
	// parent creates message queue
	fd = create_msg_queue();
	
	pid = fork();
	if(pid == 0){
		// child must wait for the parent to send a message to it
		while(get_msg_count(fd) == 0);
		
		// read message from parent
		cnt = msg_queue_rcv(fd, &msg);
		printf("Child recieved message: %s\n", msg.msg_txt);
		
		// send a ACK to paren 
		msg.to_pid = msg.from_pid; //get parents pid
		msg.from_pid = getpid();	
		msg.msg_txt[0] = 'A';
		msg.msg_txt[1] = 'C';
		msg.msg_txt[2] = 'K';
		msg.msg_txt[3] = '\0';
		cnt = msg_queue_send(fd, &msg);
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

		// wait for childs ACK
		while(get_msg_count(fd) == 0);
		cnt = msg_queue_rcv(fd, &msg);
		printf("Parent got ACK: %s\n", msg.msg_txt);

	}
	else{
		printf("fork error\n");
	}
	return 0;
}


// tests create_msg_queue
// msg_queue_snd
// msg_queue_rcv
/*
#include <ulib.h>
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	int fd, pid, cnt;
	struct msg_queue_member_info info;
	struct message msg;

	// parent creates message queue
	fd = create_msg_queue();

	for (int i = 0; i < 3;i++){
		pid = fork();
		if(!pid){
			while (get_msg_count(fd) == 0)
				;

			// read message from parent
			cnt = msg_queue_rcv(fd, &msg);
			printf("Child %d recieved message: %s\n",i+1,msg.msg_txt);

			int ppid = msg.from_pid;

			// send a ACK to paren
			msg.to_pid = msg.from_pid; //get parents pid
			msg.from_pid = getpid();
			msg.msg_txt[0] = 'A';
			msg.msg_txt[1] = 'C';
			msg.msg_txt[2] = 'K';
			msg.msg_txt[3] = ('0'+i);
			msg.msg_txt[4] = '\0';
			cnt = msg_queue_send(fd, &msg);
			return 0;
		}
	}

	sleep(100);
	// send a message to child
	msg.from_pid = getpid();
	msg.to_pid = 0xffffffff;
	msg.msg_txt[0] = 'M';
	msg.msg_txt[1] = 'S';
	msg.msg_txt[2] = 'G';
	msg.msg_txt[3] = '\0';

	// send message to child
	cnt = msg_queue_send(fd, &msg);

	for (int i = 0; i < 3;i++){
		while (get_msg_count(fd) == 0)
			;
		cnt = msg_queue_rcv(fd, &msg);
		printf("Parent got ACK: %s\n", msg.msg_txt);
	}

	return 0;
}

/*
		pid = fork();
	if (pid == 0)
	{
		// child must wait for the parent to send a message to it
		while (get_msg_count(fd) == 0)
			;

		// read message from parent
		cnt = msg_queue_rcv(fd, &msg);
		printf("Child recieved message: %s\n", msg.msg_txt);

		int ppid = msg.from_pid;

		// send a ACK to paren
		msg.to_pid = msg.from_pid; //get parents pid
		msg.from_pid = getpid();
		msg.msg_txt[0] = 'A';
		msg.msg_txt[1] = 'C';
		msg.msg_txt[2] = 'K';
		msg.msg_txt[3] = '0';
		msg.msg_txt[4] = '\0';
		cnt = msg_queue_send(fd, &msg);

		// send a ACK to paren
		// send a ACK to paren
		msg.to_pid = ppid; //get parents pid
		msg.from_pid = getpid();
		msg.msg_txt[0] = 'A';
		msg.msg_txt[1] = 'C';
		msg.msg_txt[2] = 'K';
		msg.msg_txt[3] = '1';
		msg.msg_txt[4] = '\0';
		cnt = msg_queue_send(fd, &msg);

		// send a ACK to paren
		// send a ACK to paren
		msg.to_pid = ppid; //get parents pid
		msg.from_pid = getpid();
		msg.msg_txt[0] = 'A';
		msg.msg_txt[1] = 'C';
		msg.msg_txt[2] = 'K';
		msg.msg_txt[3] = '2';
		msg.msg_txt[4] = '\0';
		cnt = msg_queue_send(fd, &msg);

		// send a ACK to paren
		// send a ACK to paren
		msg.to_pid = ppid; //get parents pid
		msg.from_pid = getpid();
		msg.msg_txt[0] = 'A';
		msg.msg_txt[1] = 'C';
		msg.msg_txt[2] = 'K';
		msg.msg_txt[3] = '3';
		msg.msg_txt[4] = '\0';
		cnt = msg_queue_send(fd, &msg);

		// send a ACK to paren
		msg.to_pid = ppid; //get parents pid
		msg.from_pid = getpid();
		msg.msg_txt[0] = 'A';
		msg.msg_txt[1] = 'C';
		msg.msg_txt[2] = 'K';
		msg.msg_txt[3] = '4';
		msg.msg_txt[4] = '\0';
		cnt = msg_queue_send(fd, &msg);
		sleep(10);
	}
	else if (pid > 0)
	{

		int spid = fork();

		if (spid == 0)
		{
			while (get_msg_count(fd) == 0)
				;
			cnt = msg_queue_rcv(fd, &msg);
			printf("Child2 recieved message: %s\n", msg.msg_txt);

			int ppid = msg.from_pid;

			// send a ACK to paren
			msg.to_pid = 0; //get parents pid
			msg.from_pid = getpid();
			msg.msg_txt[0] = 'C';
			msg.msg_txt[1] = 'C';
			msg.msg_txt[2] = 'K';
			msg.msg_txt[3] = '0';
			msg.msg_txt[4] = '\0';
			cnt = msg_queue_send(fd, &msg);

			msg.to_pid = 0xffffffff; //get parents pid
			msg.from_pid = getpid();
			msg.msg_txt[0] = 'C';
			msg.msg_txt[1] = 'C';
			msg.msg_txt[2] = 'K';
			msg.msg_txt[3] = '1';
			msg.msg_txt[4] = '\0';
			cnt = msg_queue_send(fd, &msg);
			int st = msg_queue_block(fd, ppid);
			sleep(100);
		}
		else
		{

			// send a message to child
			msg.from_pid = getpid();
			msg.to_pid = 0xffffffff;
			msg.msg_txt[0] = 'M';
			msg.msg_txt[1] = 'S';
			msg.msg_txt[2] = 'G';
			msg.msg_txt[3] = '\0';

			// send message to child
			cnt = msg_queue_send(fd, &msg);

			// wait for childs ACK
			while (get_msg_count(fd) == 0)
				;
			cnt = msg_queue_rcv(fd, &msg);

			cnt = msg_queue_rcv(fd, &msg);

			cnt = msg_queue_rcv(fd, &msg);

			cnt = msg_queue_rcv(fd, &msg);

			sleep(100);
			msg.from_pid = getpid();
			msg.to_pid = 0xffffffff;
			msg.msg_txt[0] = 'M';
			msg.msg_txt[1] = 'S';
			msg.msg_txt[2] = 'G';
			msg.msg_txt[3] = '\0';
			cnt = msg_queue_send(fd, &msg);

			cnt = msg_queue_rcv(fd, &msg);

			printf("Parent got ACK: %s\n", msg.msg_txt);
		}
	}
	else
	{
		printf("fork error\n");
	}
	*/
