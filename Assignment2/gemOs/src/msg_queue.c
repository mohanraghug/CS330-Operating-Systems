#include <msg_queue.h>
#include <context.h>
#include <memory.h>
#include <file.h>
#include <lib.h>
#include <entry.h>

/************************************************************************************/
/***************************Do Not Modify below Functions****************************/
/************************************************************************************/

struct msg_queue_info *alloc_msg_queue_info()
{
	struct msg_queue_info *info;
	info = (struct msg_queue_info *)os_page_alloc(OS_DS_REG);

	if (!info)
	{
		return NULL;
	}
	return info;
}

void free_msg_queue_info(struct msg_queue_info *q)
{
	os_page_free(OS_DS_REG, q);
}

struct message *alloc_buffer()
{
	struct message *buff;
	buff = (struct message *)os_page_alloc(OS_DS_REG);
	if (!buff)
		return NULL;
	return buff;
}

void free_msg_queue_buffer(struct message *b)
{
	os_page_free(OS_DS_REG, b);
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/

int do_create_msg_queue(struct exec_context *ctx)
{
	/** 
	 * TODO Implement functionality to
	 * create a message queue
	 **/

	// Invalid Context
	if (ctx == NULL)
		return -EINVAL;

	// Finding a free file Descriptor
	int fd = 3;
	while (fd < MAX_OPEN_FILES && ctx->files[fd] != NULL)
		fd++;

	// Max files opened already
	if (fd == MAX_OPEN_FILES)
		return -EOTHERS;

	// Allocating file object for Message Queue
	struct file *fp = alloc_file();

	// allocation failed
	if (fp == NULL)
		return -ENOMEM;

	// Allocating message queue
	struct msg_queue_info *msg_q = alloc_msg_queue_info();

	// Allocation failed
	if (msg_q == NULL)
		return -ENOMEM;

	// Allocating message buffer
	msg_q->msg_buffer = alloc_buffer();

	// Allocation failed
	if (msg_q->msg_buffer == NULL)
		return -ENOMEM;

	// Setting Default Values for the fields in msg_q
	msg_q->member_count = 1;
	msg_q->member_pid[0] = ctx->pid;
	msg_q->cur_size = 0;
	for (int i = 0; i < MAX_MEMBERS; i++)
	{
		for (int j = 0; j < MAX_MEMBERS; j++)
		{
			msg_q->blocked[i][j] = 0;
		}
	}

	// setting the values for the fields of file object
	fp->type = MSG_QUEUE;
	fp->ref_count = 1;
	fp->inode = NULL;
	fp->fops = NULL;
	fp->msg_queue = msg_q;
	fp->pipe = NULL;

	ctx->files[fd] = fp;

	return fd;
}

int do_msg_queue_rcv(struct exec_context *ctx, struct file *filep, struct message *msg)
{
	/** 
	 * TODO Implement functionality to
	 * recieve a message
	 **/

	// Checking validity of ctx and filep
	if (ctx == NULL || filep == NULL || filep->type != MSG_QUEUE)
		return -EINVAL;

	// Message is invalid
	if (msg == NULL)
		return -EINVAL;

	// Invalid Message Queue
	if (filep->msg_queue == NULL)
		return -EINVAL;

	// checking if the receiving process is part of the member list
	char ok = 0;
	for (int i = 0; i < filep->msg_queue->member_count; i++)
	{
		if (filep->msg_queue->member_pid[i] == ctx->pid)
		{
			ok = 1;
			break;
		}
	}

	/*dprintk("Debugging in msg_queue_rcv.......\n");

	dprintk("Members of the Message Queue : \n");
	for (int i = 0; i < filep->msg_queue->member_count; i++)
	{
		dprintk("%d ", filep->msg_queue->member_pid[i]);
	}
	dprintk("\n");

	dprintk("Messages beforre receive : \n");

	for (int i = 0; i < filep->msg_queue->cur_size; i++)
	{
		dprintk("i : %d ,", i);
		struct message foo = filep->msg_queue->msg_buffer[i];
		dprintk("from_pid : %d ,", foo.from_pid);
		dprintk("to_pid : %d, ", foo.to_pid);
		dprintk("msg_txt : %s \n", foo.msg_txt);
	}*/

	// The calling process is not a member of Message Queue
	if (!ok)
		return -EINVAL;

	for (int i = 0; i < filep->msg_queue->cur_size; i++)
	{
		int from_pid = filep->msg_queue->msg_buffer[i].from_pid;
		int to_pid = filep->msg_queue->msg_buffer[i].to_pid;

		if (to_pid == ctx->pid)
		{
			// Filling the fields of the message
			msg->from_pid = from_pid;
			msg->to_pid = to_pid;
			for (int j = 0; j < MAX_TXT_SIZE; j++)
			{
				msg->msg_txt[j] = filep->msg_queue->msg_buffer[i].msg_txt[j];
			}

			// removing the message from message queue
			for (int j = i; j < filep->msg_queue->cur_size - 1; j++)
			{
				filep->msg_queue->msg_buffer[j] = filep->msg_queue->msg_buffer[j + 1];
			}
			filep->msg_queue->cur_size -= 1;

			/*
			dprintk("Messages after receive : \n");

			for (int i = 0; i < filep->msg_queue->cur_size; i++)
			{
				dprintk("i : %d ,", i);
				struct message foo = filep->msg_queue->msg_buffer[i];
				dprintk("from_pid : %d ,", foo.from_pid);
				dprintk("to_pid : %d, ", foo.to_pid);
				dprintk("msg_txt : %s \n", foo.msg_txt);
			}*/
			

			return 1;
		}
	}

	return 0;
}

int do_msg_queue_send(struct exec_context *ctx, struct file *filep, struct message *msg)
{
	/** 
	 * TODO Implement functionality to
	 * send a message
	 **/

	// Checking validity of ctx and filep
	if (ctx == NULL || filep == NULL || filep->type != MSG_QUEUE)
		return -EINVAL;

	// Invalid Message
	if (msg == NULL)
		return -EINVAL;

	// Invalid Message Queue
	if (filep->msg_queue == NULL)
		return -EINVAL;

	int ind1 = -1;
	for (u32 i = 0; i < filep->msg_queue->member_count; i++)
	{
		if (filep->msg_queue->member_pid[i] == msg->from_pid)
		{
			ind1 = i;
			break;
		}
	}

	// Sender is not a part of message queue
	if (ind1 == -1 || ctx->pid != msg->from_pid)
	{
		return -EINVAL;
	}

	/*
	dprintk("In send Message : \n");
	dprintk("to_pid == %d\n", msg->to_pid);
	*/

	if (msg->to_pid == BROADCAST_PID)
	{
		int ans = 0;
		for (u32 i = 0; i < filep->msg_queue->member_count; i++)
		{
			if (filep->msg_queue->member_pid[i] != ctx->pid)
			{
				if (filep->msg_queue->blocked[ind1][i])
					continue;

				// Adding a new message to the buffer
				filep->msg_queue->msg_buffer[filep->msg_queue->cur_size].from_pid = msg->from_pid;
				filep->msg_queue->msg_buffer[filep->msg_queue->cur_size].to_pid = filep->msg_queue->member_pid[i];
				for (int j = 0; j < MAX_TXT_SIZE; j++)
					filep->msg_queue->msg_buffer[filep->msg_queue->cur_size].msg_txt[j] = msg->msg_txt[j];

				// Updating size of buffer and return value
				filep->msg_queue->cur_size += 1;
				ans += 1;
			}
		}
		return ans;
	}
	else
	{
		// Finding the index of the member to send message
		int ind2 = -1;
		for (int i = 0; i < filep->msg_queue->member_count; i++)
		{
			if (filep->msg_queue->member_pid[i] == msg->to_pid)
			{
				ind2 = i;
				break;
			}
		}
		/*
		dprintk("In direct case ... \n");
		dprintk("ind2 : %d \n", ind2);
		*/

		// receiver not found in message queue
		if (ind2 == -1)
			return -EINVAL;

		// receiver blocked you from sending messages
		if (filep->msg_queue->blocked[ind1][ind2])
			return -EINVAL;

		filep->msg_queue->msg_buffer[filep->msg_queue->cur_size].from_pid = msg->from_pid;
		filep->msg_queue->msg_buffer[filep->msg_queue->cur_size].to_pid = msg->to_pid;
		for (int j = 0; j < MAX_TXT_SIZE; j++)
			filep->msg_queue->msg_buffer[filep->msg_queue->cur_size].msg_txt[j] = msg->msg_txt[j];
		filep->msg_queue->cur_size += 1;

		return 1;
	}

	return -EINVAL;
}

void do_add_child_to_msg_queue(struct exec_context *child_ctx)
{
	/** 
	 * TODO Implementation of fork handler 
	 **/

	// Invalid Child Context
	if (child_ctx == NULL)
		return;

	for (int fd = 0; fd < MAX_OPEN_FILES; fd++)
	{
		if (child_ctx->files[fd] != NULL && child_ctx->files[fd]->type == MSG_QUEUE)
		{
			struct file *fp = child_ctx->files[fd];
			fp->ref_count += 1;
			fp->msg_queue->member_pid[fp->msg_queue->member_count] = child_ctx->pid;
			fp->msg_queue->member_count += 1;
		}
	}
}

void do_msg_queue_cleanup(struct exec_context *ctx)
{
	/** 
	 * TODO Implementation of exit handler 
	 **/

	// Invalid Context
	if (ctx == NULL)
		return;

	for (int fd = 0; fd < MAX_OPEN_FILES; fd++)
	{
		if (ctx->files[fd] != NULL && ctx->files[fd]->type == MSG_QUEUE)
		{
			do_msg_queue_close(ctx, fd);
			ctx->files[fd] = NULL;
		}
	}
}

int do_msg_queue_get_member_info(struct exec_context *ctx, struct file *filep, struct msg_queue_member_info *info)
{
	/** 
	 * TODO Implementation of exit handler 
	 **/

	// Checking validity of ctx and filep
	if (ctx == NULL || filep == NULL || filep->type != MSG_QUEUE)
		return -EINVAL;

	// Invalid Message Queue
	if (filep->msg_queue == NULL)
		return -EINVAL;

	// invalid info
	if (info == NULL)
		return -EINVAL;

	info->member_count = filep->msg_queue->member_count;
	for (u32 i = 0; i < info->member_count; i++)
	{
		info->member_pid[i] = filep->msg_queue->member_pid[i];
	}

	return 0;
}

int do_get_msg_count(struct exec_context *ctx, struct file *filep)
{
	/** 
	 * TODO Implement functionality to
	 * return pending message count to calling process
	 **/

	// Invalid context or filep
	if (ctx == NULL || filep == NULL || filep->type != MSG_QUEUE)
		return -EINVAL;

	// Invalid Message queue
	if (filep->msg_queue == NULL)
		return -EINVAL;

	int ans = 0;
	for (u32 i = 0; i < filep->msg_queue->cur_size; i += 1)
	{
		if (filep->msg_queue->msg_buffer[i].to_pid == ctx->pid)
		{
			ans++;
		}
	}

	return ans;
}

int do_msg_queue_block(struct exec_context *ctx, struct file *filep, int pid)
{
	/** 
	 * TODO Implement functionality to
	 * block messages from another process 
	 **/

	// Invalid context or filep
	if (ctx == NULL || filep == NULL || filep->type != MSG_QUEUE)
		return -EINVAL;

	// Invalid Message Queue
	if (filep->msg_queue == NULL)
		return -EINVAL;

	// Checking whether both pids belong to message queue
	int to = -1, from = -1;
	for (int i = 0; i < filep->msg_queue->member_count; i++)
	{
		if (filep->msg_queue->member_pid[i] == ctx->pid)
		{
			to = i;
		}
		if (filep->msg_queue->member_pid[i] == pid)
		{
			from = i;
		}
	}

	// atleast one of them do not belong to message queue
	if (from == -1 || to == -1)
		return -EINVAL;

	// blocking messages from pid to ctx->pid
	filep->msg_queue->blocked[from][to] = 1;

	return 0;
}

int do_msg_queue_close(struct exec_context *ctx, int fd)
{
	/** 
	 * TODO Implement functionality to
	 * remove the calling process from the message queue 
	 **/

	// Invalid Context
	if (ctx == NULL)
		return -EINVAL;

	// Invalid fds or files
	if (fd < 0 || fd >= MAX_OPEN_FILES || ctx->files[fd] == NULL || ctx->files[fd]->type != MSG_QUEUE)
		return -EINVAL;

	// Checking if the cur process is a member of message queue or not
	int ind = -1;
	for (int i = 0; ctx->files[fd]->msg_queue->member_count; i++)
	{
		if (ctx->files[fd]->msg_queue->member_pid[i] == ctx->pid)
		{
			ind = i;
			break;
		}
	}

	// cur pid does not belong to the message queue
	if (ind == -1)
		return -EINVAL;

	// In close Array

	/*
	dprintk("Debugging in Close of pid : %d......\n", ctx->pid);

	dprintk("members_count : %d\n", ctx->files[fd]->msg_queue->member_count);

	for (int i = 0; i < ctx->files[fd]->msg_queue->member_count; i++)
	{
		dprintk("i : %d, pid : %d \n", i, ctx->files[fd]->msg_queue->member_pid[i]);
	}

	dprintk("Blocked pairs : \n");

	for (int i = 0; i < MAX_MEMBERS; i++)
	{
		for (int j = 0; j < MAX_MEMBERS; j++)
		{
			if (ctx->files[fd]->msg_queue->blocked[i][j])
			{
				dprintk("%d Blocked messages from %d\n", ctx->files[fd]->msg_queue->member_pid[j], ctx->files[fd]->msg_queue->member_pid[i]);
			}
		}
	}
	*/

	// Updating blocked array
	char new_blocked[MAX_MEMBERS][MAX_MEMBERS];

	for (int i = 0; i < MAX_MEMBERS; i++)
	{
		for (int j = 0; j < MAX_MEMBERS; j++)
		{
			new_blocked[i][j] = 0;
		}
	}

	for (int i = 0; i < MAX_MEMBERS; i++)
	{
		for (int j = 0; j < MAX_MEMBERS; j++)
		{
			if (i == ind || j == ind)
				continue;
			if (j > ind && i > ind)
			{
				new_blocked[i - 1][j - 1] = ctx->files[fd]->msg_queue->blocked[i][j];
			}
			else if (i > ind)
			{
				new_blocked[i - 1][j] = ctx->files[fd]->msg_queue->blocked[i][j];
			}
			else if (j > ind)
			{
				new_blocked[i][j - 1] = ctx->files[fd]->msg_queue->blocked[i][j];
			}
			else
			{
				new_blocked[i][j] = ctx->files[fd]->msg_queue->blocked[i][j];
			}
		}
	}

	for (int i = 0; i < MAX_MEMBERS; i++)
	{
		for (int j = 0; j < MAX_MEMBERS; j++)
		{
			ctx->files[fd]->msg_queue->blocked[i][j] = new_blocked[i][j];
		}
	}

	for (int j = ind; j < ctx->files[fd]->msg_queue->member_count - 1; j++)
	{
		ctx->files[fd]->msg_queue->member_pid[j] = ctx->files[fd]->msg_queue->member_pid[j + 1];
	}

	ctx->files[fd]->msg_queue->member_count -= 1;

	/*
	dprintk("After members_count : %d\n", ctx->files[fd]->msg_queue->member_count);

	for (int i = 0; i < ctx->files[fd]->msg_queue->member_count; i++)
	{
		dprintk("i : %d, pid : %d \n", i, ctx->files[fd]->msg_queue->member_pid[i]);
	}

	dprintk("Blocked pairs : \n");

	for (int i = 0; i < MAX_MEMBERS; i++)
	{
		for (int j = 0; j < MAX_MEMBERS; j++)
		{
			if (ctx->files[fd]->msg_queue->blocked[i][j])
			{
				dprintk("%d Blocked messages from %d\n", ctx->files[fd]->msg_queue->member_pid[j], ctx->files[fd]->msg_queue->member_pid[i]);
			}
		}
	}
	*/

	if (ctx->files[fd]->msg_queue->member_count == 0)
	{
		free_msg_queue_buffer(ctx->files[fd]->msg_queue->msg_buffer);
		free_msg_queue_info(ctx->files[fd]->msg_queue);
		free_file_object(ctx->files[fd]);
	}

	ctx->files[fd] = NULL;

	return 0;
}
