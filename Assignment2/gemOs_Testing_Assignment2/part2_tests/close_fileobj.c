#include<ulib.h>
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	int fd1, fd2, fd3, open_count, close_count, init_count;	 
	
	init_count = get_fileobj_count();

	// create message queue
	fd1 = create_msg_queue();
	fd2 = create_msg_queue();
	fd3 = create_msg_queue();
	
	open_count = get_fileobj_count();	
	
	msg_queue_close(fd1);
	msg_queue_close(fd2);
	msg_queue_close(fd3);
	
	close_count = get_fileobj_count();	
	
	if(
		init_count + 3 == open_count &&
		close_count == init_count
	)
		printf("Tests passed\n");
	else
		printf("Tests failed\n");
	return 0;
}
