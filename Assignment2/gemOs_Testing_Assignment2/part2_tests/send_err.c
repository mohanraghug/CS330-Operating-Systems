#include<ulib.h>
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	int pid, cnt;	 
	struct message msg;

	// fd is NOT a msg_queue
	char *filename="test.txt";
	int fd = open(filename, O_CREAT|O_RDWR, O_READ|O_WRITE);

	pid = fork();
	if(pid == 0){
		// child must wait for the parent to send a message to it
		sleep(100);
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

		if (cnt != -EINVAL) {
			printf("Test failed\n");
		}
		else {
			printf("Test passed\n");
		}
	}
	else{
		printf("fork error\n");
	}
	return 0;
}
