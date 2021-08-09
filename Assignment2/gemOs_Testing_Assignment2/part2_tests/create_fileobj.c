
#include<ulib.h>
// Tests if files are allocated
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	int fd1, fd2, fd3;

	fd1 = create_msg_queue();
	fd2 = create_msg_queue();
	fd3 = create_msg_queue();

	if (get_fileobj_count() < 6) {
		printf("Test failed\n");
		return -1;
	}
	else{
		printf("Test passed\n");
	}

	return 0;

}
