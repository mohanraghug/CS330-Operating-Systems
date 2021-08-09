#include <types.h>
#include <context.h>
#include <file.h>
#include <lib.h>
#include <serial.h>
#include <entry.h>
#include <memory.h>
#include <fs.h>
#include <kbd.h>

/************************************************************************************/
/***************************Do Not Modify below Functions****************************/
/************************************************************************************/

void free_file_object(struct file *filep)
{
	if (filep)
	{
		os_page_free(OS_DS_REG, filep);
		stats->file_objects--;
	}
}

struct file *alloc_file()
{
	struct file *file = (struct file *)os_page_alloc(OS_DS_REG);
	file->fops = (struct fileops *)(file + sizeof(struct file));
	bzero((char *)file->fops, sizeof(struct fileops));
	file->ref_count = 1;
	file->offp = 0;
	stats->file_objects++;
	return file;
}

void *alloc_memory_buffer()
{
	return os_page_alloc(OS_DS_REG);
}

void free_memory_buffer(void *ptr)
{
	os_page_free(OS_DS_REG, ptr);
}

/* STDIN,STDOUT and STDERR Handlers */

/* read call corresponding to stdin */

static int do_read_kbd(struct file *filep, char *buff, u32 count)
{
	kbd_read(buff);
	return 1;
}

/* write call corresponding to stdout */

static int do_write_console(struct file *filep, char *buff, u32 count)
{
	struct exec_context *current = get_current_ctx();
	return do_write(current, (u64)buff, (u64)count);
}

long std_close(struct file *filep)
{
	filep->ref_count--;
	if (!filep->ref_count)
		free_file_object(filep);
	return 0;
}
struct file *create_standard_IO(int type)
{
	struct file *filep = alloc_file();
	filep->type = type;
	if (type == STDIN)
		filep->mode = O_READ;
	else
		filep->mode = O_WRITE;
	if (type == STDIN)
	{
		filep->fops->read = do_read_kbd;
	}
	else
	{
		filep->fops->write = do_write_console;
	}
	filep->fops->close = std_close;
	return filep;
}

int open_standard_IO(struct exec_context *ctx, int type)
{
	int fd = type;
	struct file *filep = ctx->files[type];
	if (!filep)
	{
		filep = create_standard_IO(type);
	}
	else
	{
		filep->ref_count++;
		fd = 3;
		while (ctx->files[fd])
			fd++;
	}
	ctx->files[fd] = filep;
	return fd;
}
/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/

/* File exit handler */
void do_file_exit(struct exec_context *ctx)
{
	/*TODO the process is exiting. Adjust the refcount
	of files*/

	// invalid context
	if (ctx == NULL)
		return;

	// closing all the open files
	for (int i = 0; i < MAX_OPEN_FILES; i++)
	{
		if (ctx->files[i] != NULL)
		{
			ctx->files[i]->fops->close(ctx->files[i]);
			ctx->files[i] = NULL;
		}
	}
}

/*Regular file handlers to be written as part of the assignmemnt*/

static int do_read_regular(struct file *filep, char *buff, u32 count)
{
	/** 
	*  TODO Implementation of File Read, 
	*  You should be reading the content from File using file system read function call and fill the buf
	*  Validate the permission, file existence, Max length etc
	*  Incase of Error return valid Error code 
	**/

	// Invalid file object
	if (filep == NULL)
		return -EINVAL;

	struct inode *file_inode = filep->inode;

	// Invalid File Inode
	if (file_inode == NULL)
	{
		return -EINVAL;
	}

	// No read permission
	if ((filep->mode & O_READ) == 0)
		return -EACCES;

	int ret = flat_read(file_inode, buff, count, &(filep->offp));

	// read was not successful
	if (ret < 0)
		return ret;

	// flat_read doesnot update the offset
	filep->offp += ret;

	return ret;
}

/*write call corresponding to regular file */

static int do_write_regular(struct file *filep, char *buff, u32 count)
{
	/** 
	*   TODO Implementation of File write, 
	*   You should be writing the content from buff to File by using File system write function
	*   Validate the permission, file existence, Max length etc
	*   Incase of Error return valid Error code 
	* */

	// Invalid file object
	if (filep == NULL)
		return -EINVAL;

	struct inode *file_inode = filep->inode;

	// Invalid File Inode
	if (file_inode == NULL)
		return -EINVAL;

	// No write permission
	if ((filep->mode & O_WRITE) == 0)
		return -EACCES;

	int ret = flat_write(file_inode, buff, count, &(filep->offp));

	// Write was not successful
	if (ret < 0)
		return ret;

	// Updating the offset
	filep->offp += ret;

	return ret;
}

long do_file_close(struct file *filep)
{
	/** TODO Implementation of file close  
	*   Adjust the ref_count, free file object if needed
	*   Incase of Error return valid Error code 
	*/

	// Checking the file pointer and ref_count
	if (filep == NULL || filep->ref_count <= 0)
		return -EINVAL;

	// Decreasing the ref_count
	filep->ref_count -= 1;

	if (filep->ref_count == 0)
	{
		free_file_object(filep);
	}

	return 0;
}

static long do_lseek_regular(struct file *filep, long offset, int whence)
{
	/** 
	*   TODO Implementation of lseek 
	*   Set, Adjust the ofset based on the whence
	*   Incase of Error return valid Error code 
	* */

	// Invalid file pointer
	if (filep == NULL)
		return -EINVAL;

	// Check this ?

	if (whence == SEEK_SET)
	{
		// Invalid offset
		if (offset < 0 || offset >= FILE_SIZE)
			return -EINVAL;

		// Modifying file offset
		filep->offp = offset;

		return offset;
	}
	else if (whence == SEEK_CUR)
	{
		long noffset = filep->offp + offset;

		// Invalid offset
		if (noffset < 0 || noffset >= FILE_SIZE)
		{
			return -EINVAL;
		}

		// Modifying file offset
		filep->offp = noffset;

		return noffset;
	}
	else if (whence == SEEK_END)
	{
		long noffset = filep->inode->file_size + offset;

		// Invalid offset
		if (noffset < 0 || noffset >= FILE_SIZE)
		{
			return -EINVAL;
		}

		// Modifying file offset
		filep->offp = noffset;

		return noffset;
	}
	else
	{
		return -EINVAL;
	}
}

extern int do_regular_file_open(struct exec_context *ctx, char *filename, u64 flags, u64 mode)
{

	/**  
	*  TODO Implementation of file open, 
	*  You should be creating file(use the alloc_file function to creat file), 
	*  To create or Get inode use File system function calls, 
	*  Handle mode and flags 
	*  Validate file existence, Max File count is 16, Max Size is 4KB, etc
	*  Incase of Error return valid Error code 
	* */
	int ret_fd = -EINVAL;

	struct inode *file_inode = lookup_inode(filename);

	// flags should contain only one of these READ,WRITE,CREATE
	if ((flags | (O_READ | O_WRITE | O_RDWR | O_CREAT)) != (O_READ | O_WRITE | O_RDWR | O_CREAT))
		return -EINVAL;

	// flags should be non_zero
	if (flags == 0)
		return -EINVAL;

	if (file_inode == NULL)
	{
		// create bit must be on
		if ((flags & O_CREAT) == 0)
			return -EINVAL;

		// mode should be one of these
		if ((mode | (O_EXEC | O_READ | O_WRITE)) != (O_EXEC | O_READ | O_WRITE))
			return -EINVAL;

		// mode should be non zero
		if (mode == 0)
			return -EINVAL;

		file_inode = create_inode(filename, mode);

		// memory error
		if (file_inode == NULL)
			return -ENOMEM;
	}

	// if file already exists O_CREAT has no effect so flipping it to zero
	if ((flags & O_CREAT) != 0)
		flags ^= O_CREAT;

	// Checking if the flags is a subset of the file's permissions
	if ((file_inode->mode & flags) != flags)
		return -EACCES;

	struct file *obj = alloc_file();

	// memory error
	if (obj == NULL)
		return -ENOMEM;

	// Finding a free fd for this file
	for (int i = 3; i < MAX_OPEN_FILES; i++)
	{
		if (ctx->files[i] == NULL)
		{
			ctx->files[i] = obj;
			ret_fd = i;
			break;
		}
	}

	// no free fd -> return error
	if (ret_fd < 0)
	{
		return -EOTHERS;
	}

	// setting the fields of new file object appropriately
	obj->type = REGULAR;
	obj->mode = flags;
	obj->ref_count = 1;
	obj->inode = file_inode;
	obj->fops->close = do_file_close;
	obj->fops->lseek = do_lseek_regular;
	obj->fops->read = do_read_regular;
	obj->fops->write = do_write_regular;
	obj->offp = 0;
	obj->msg_queue = NULL;
	obj->pipe = NULL;

	return ret_fd;
}

/**
 * Implementation dup 2 system call;
 */
int fd_dup2(struct exec_context *current, int oldfd, int newfd)
{
	/** 
	*  TODO Implementation of the dup2 
	*  Incase of Error return valid Error code 
	**/

	// Invalid Context
	if (current == NULL)
		return -EINVAL;

	// Checking validity of the file_descriptors
	if (oldfd < 0 || newfd < 0 || oldfd >= MAX_OPEN_FILES || newfd >= MAX_OPEN_FILES)
	{
		return -EINVAL;
	}

	// Checking if oldfd is open or not
	if (current->files[oldfd] == NULL)
	{
		return -EINVAL;
	}

	struct file *file_obj = current->files[oldfd];

	// if new_fd is open closing new_fd
	if (current->files[newfd] != NULL)
		do_file_close(current->files[newfd]);

	current->files[newfd] = file_obj;
	file_obj->ref_count += 1;

	return newfd;
}

int do_sendfile(struct exec_context *ctx, int outfd, int infd, long *offset, int count)
{
	/** 
	*  TODO Implementation of the sendfile 
	*  Incase of Error return valid Error code 
	**/

	// invalid context
	if (ctx == NULL)
		return -EINVAL;
	
	// invalid file descriptors
	if (infd < 0 || outfd < 0 || infd >= MAX_OPEN_FILES || outfd >= MAX_OPEN_FILES)
		return -EINVAL;
	
	// if infd or outfd is not opened
	if (ctx->files[infd] == NULL || ctx->files[outfd] == NULL)
		return -EINVAL;
	
	// no read permissions for infd
	if ((ctx->files[infd]->mode & O_READ) == 0)
		return -EACCES;
	
	// no write permissions for outfd
	if ((ctx->files[outfd]->mode & O_WRITE) == 0)
		return -EACCES;

	if(count == 0)
		return 0;

	if (offset == NULL)
	{
		// file offset must be changed
		struct file *filep = ctx->files[infd];

		long old_offset = filep->offp;

		// allocating memory for read buffer
		char *buff = alloc_memory_buffer();

		// memory allocation failed
		if(buff == NULL)
			return -ENOMEM;

		int nread = do_read_regular(filep, buff, count);

		// Unable to read form infd
		if (nread <= 0)
		{
			filep->offp = old_offset;
			free_memory_buffer(buff);
			return -EOTHERS;
		}

		struct file *out = ctx->files[outfd];

		// Write as many bytes as possible to the outfd
		int nwrite = do_write_regular(out, buff, 1);

		if (nwrite <= 0)
		{
			filep->offp = old_offset;
			free_memory_buffer(buff);
			return 0;
		}

		int written_bytes = 1;

		while(written_bytes < nread && nwrite == 1){
			nwrite = do_write_regular(out, buff+written_bytes, 1);
			if(nwrite == 1) written_bytes += 1;
		}

		int rem = nread - written_bytes;

		// Update the offset of the infd according to number of bytes written
		filep->offp -= rem;

		// free the allocated memory for buffer
		free_memory_buffer(buff);

		return written_bytes;
	}
	else
	{
		// file offset must be changed
		struct file *filep = ctx->files[infd];

		long old_offset = filep->offp;

		filep->offp = *offset;

		// allocating memory for read buffer
		char *buff = alloc_memory_buffer();

		// memory allocation failed
		if (buff == NULL)
			return -ENOMEM;

		int nread = do_read_regular(filep, buff, count);

		// Unable to read form infd
		if (nread <= 0)
		{
			filep->offp = old_offset;
			free_memory_buffer(buff);
			return -EOTHERS;
		}
		struct file *out = ctx->files[outfd];

		// Write as many bytes as possible to the outfd
		int nwrite = do_write_regular(out, buff, 1);
		
		if(nwrite <= 0) {
			filep->offp = old_offset;
			free_memory_buffer(buff);
			return 0;
		}

		int written_bytes = 1;

		while (written_bytes < nread && nwrite == 1)
		{
			nwrite = do_write_regular(out, buff + written_bytes, 1);
			written_bytes += 1;
		}

		int rem = nread - written_bytes;

		// restoring the original offset of infd
		*offset = filep->offp - rem;
		filep->offp = old_offset;

		// free the allocated memory for buffer
		free_memory_buffer(buff);

		return written_bytes;
	}

	return -EINVAL;
}
