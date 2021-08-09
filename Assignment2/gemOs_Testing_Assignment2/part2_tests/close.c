#include<ulib.h>
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	int fd, pid, fcntb, fcnta;	 
	
	// parent creates message queue
	fd = create_msg_queue();
	pid = fork();

	if(pid == 0){
		sleep(100);
	}
	else if(pid > 0){
		fcntb = get_fileobj_count();	
		msg_queue_close(fd);
		// file should not be freed
		fcnta = get_fileobj_count();
		if(fcntb == fcnta)
			printf("Test passed\n");
		else 
			printf("Test failed\n");
	}
	else{
		printf("fork failed\n");
	}
	return 0;
}
