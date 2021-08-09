#include <debug.h>
#include <context.h>
#include <entry.h>
#include <lib.h>
#include <memory.h>

/*****************************HELPERS******************************************/

/* 
 * allocate the struct which contains information about debugger
 *
 */
struct debug_info *alloc_debug_info()
{
	struct debug_info *info = (struct debug_info *)os_alloc(sizeof(struct debug_info));
	if (info)
		bzero((char *)info, sizeof(struct debug_info));
	return info;
}

/*
 * frees a debug_info struct 
 */
void free_debug_info(struct debug_info *ptr)
{
	if (ptr)
		os_free((void *)ptr, sizeof(struct debug_info));
}

/*
 * allocates memory to store registers structure
 */
struct registers *alloc_regs()
{
	struct registers *info = (struct registers *)os_alloc(sizeof(struct registers));
	if (info)
		bzero((char *)info, sizeof(struct registers));
	return info;
}

/*
 * frees an allocated registers struct
 */
void free_regs(struct registers *ptr)
{
	if (ptr)
		os_free((void *)ptr, sizeof(struct registers));
}

/* 
 * allocate a node for breakpoint list 
 * which contains information about breakpoint
 */
struct breakpoint_info *alloc_breakpoint_info()
{
	struct breakpoint_info *info = (struct breakpoint_info *)os_alloc(
		sizeof(struct breakpoint_info));
	if (info)
		bzero((char *)info, sizeof(struct breakpoint_info));
	return info;
}

/*
 * frees a node of breakpoint list
 */
void free_breakpoint_info(struct breakpoint_info *ptr)
{
	if (ptr)
		os_free((void *)ptr, sizeof(struct breakpoint_info));
}

/*
 * Fork handler.
 * The child context doesnt need the debug info
 * Set it to NULL
 * The child must go to sleep( ie move to WAIT state)
 * It will be made ready when the debugger calls wait_and_continue
 */
void debugger_on_fork(struct exec_context *child_ctx)
{
	struct exec_context *ctx = get_current_ctx();
	ctx->dbg->cpid = child_ctx->pid;
	child_ctx->dbg = NULL;
	child_ctx->state = WAITING;
}

/******************************************************************************/

/* This is the int 0x3 handler
 * Hit from the childs context
 */
long int3_handler(struct exec_context *ctx)
{

	// Invalid exec_context
	if (!ctx)
		return -1;

	// Address of the instruction which triggered int3 interrupt
	u64 addr = ctx->regs.entry_rip - 1;

	// getting the parent_ctx
	struct exec_context *parent_ctx = get_ctx_by_pid(ctx->ppid);

	// invalid parent context
	if (!parent_ctx || !parent_ctx->dbg)
		return -1;

	// head of the linked list of breakpoints in the parent
	struct breakpoint_info *head = parent_ctx->dbg->head;

	while (head != NULL)
	{
		if (head->addr == addr)
		{
			break;
		}
		head = head->next;
	}

	// breakpoint not present at this addr. DO nothing.
	if (head == NULL)
		return -1;

	// placing the return value in rax register of the parent since execution resumes in user mode.
	parent_ctx->regs.rax = ctx->regs.entry_rip - 1;
	parent_ctx->state = READY;

	// executing push %rbp
	ctx->regs.entry_rsp -= 8;
	*(u64 *)ctx->regs.entry_rsp = ctx->regs.rbp;

	// changing the ctx state to WAITING
	ctx->state = WAITING;

	// Storing back trace info
	u64 prev_rbp = ctx->regs.rbp;
	u64 ra = ctx->regs.entry_rsp + 8;
	u64 rip = (ctx->regs.entry_rip) - 1;
	int i = 0;
	while (rip != END_ADDR)
	{
		parent_ctx->dbg->bt_info[i] = rip;

		u64 nrip, nra, nrbp;
		nrip = *((u64 *)ra);
		nra = (prev_rbp + 8);
		nrbp = *((u64 *)prev_rbp);

		prev_rbp = nrbp;
		rip = nrip;
		ra = nra;
		i += 1;
	}
	parent_ctx->dbg->bt_size = i;

	// scheduling the parent
	schedule(parent_ctx);

	return 0;
}

/*
 * Exit handler.
 * Called on exit of Debugger and Debuggee 
 */
void debugger_on_exit(struct exec_context *ctx)
{
	if (ctx->dbg)
	{
		// parent is exitting
		struct breakpoint_info *head = ctx->dbg->head;
		while (head)
		{
			struct breakpoint_info *nxt = head->next;
			free_breakpoint_info(head);
			head = nxt;
		}
		free_debug_info(ctx->dbg);
	}
	else
	{
		// child is exitting
		struct exec_context *parent_ctx = get_ctx_by_pid(ctx->ppid);
		parent_ctx->regs.rax = CHILD_EXIT;
		parent_ctx->state = READY;
	}
}

/*
 * called from debuggers context
 * initializes debugger state
 */
int do_become_debugger(struct exec_context *ctx)
{
	// invalid context
	if (!ctx)
		return -1;

	ctx->dbg = alloc_debug_info();

	// debug_info allocation failed
	if (!ctx->dbg)
		return -1;

	// breakpoint_counter to allocate numbers to breakpoints
	ctx->dbg->breakpoint_counter = 0;

	// for backtrace info
	ctx->dbg->bt_size = 0;

	return 0;
}

/*
 * called from debuggers context
 */
int do_set_breakpoint(struct exec_context *ctx, void *addr)
{

	// invalid context checks
	if (!ctx || !ctx->dbg)
		return -1;

	// head of the current bp list
	struct breakpoint_info *head = ctx->dbg->head;

	// getting the debuggee's exec context.
	struct exec_context *child_ctx = get_ctx_by_pid(ctx->dbg->cpid);

	// invalid debuggee exec_context
	if (child_ctx == NULL)
		return -1;

	if (head == NULL)
	{
		// creating a new bp_info for the newly created bp
		struct breakpoint_info *new_breakpoint = alloc_breakpoint_info();

		// breakpoint allocation failed
		if (!new_breakpoint)
			return -1;

		// pointing the head of dbg bp list to the newly created breakpoint
		ctx->dbg->head = new_breakpoint;
		new_breakpoint->addr = (u64)addr;
		new_breakpoint->next = NULL;
		new_breakpoint->num = ctx->dbg->breakpoint_counter + 1;
		new_breakpoint->status = 1;
	}
	else
	{
		struct breakpoint_info *prev = NULL;
		int cnt = 0;
		while (head != NULL)
		{
			if (head->addr == (u64)addr)
			{
				*(u8 *)addr = INT3_OPCODE;
				head->status = 1;
				return 0;
			}
			prev = head;
			head = head->next;
			cnt += 1;
		}

		// max breakpoints already set
		if (cnt >= MAX_BREAKPOINTS)
			return -1;

		// creating a new breakpoint
		struct breakpoint_info *new_breakpoint = alloc_breakpoint_info();

		// breakpoint allocation failed
		if (!new_breakpoint)
			return -1;

		// adding the new_breakpoint to the breakpoint list and setting fields appropriately.
		prev->next = new_breakpoint;
		new_breakpoint->addr = (u64)addr;
		new_breakpoint->next = NULL;
		new_breakpoint->status = 1;
		new_breakpoint->num = ctx->dbg->breakpoint_counter + 1;
	}

	ctx->dbg->breakpoint_counter += 1;

	// modifying the address to trigger int3 interrupt
	*(u8 *)addr = INT3_OPCODE;

	return 0;
}

/*
 * called from debuggers context
 */
int do_remove_breakpoint(struct exec_context *ctx, void *addr)
{
	// invalid debugger context
	if (!ctx || !ctx->dbg)
		return -1;

	// getting the head of breakpoint list
	struct breakpoint_info *head = ctx->dbg->head;
	if (head == NULL)
	{
		return -1;
	}
	else
	{
		struct breakpoint_info *prev = NULL;

		// finding the breakpoint with given addr.
		while (head != NULL)
		{
			if (head->addr == (u64)addr)
			{
				break;
			}
			prev = head;
			head = head->next;
		}

		// no breakpoint present with given addr.
		if (head == NULL)
			return -1;

		// revert the changes done in code segment.
		*(u8 *)(head->addr) = PUSHRBP_OPCODE;

		if (prev == NULL)
		{
			ctx->dbg->head = head->next;
			free_breakpoint_info(head);
		}
		else
		{
			prev->next = head->next;
			free_breakpoint_info(head);
		}
	}

	return 0;
}

/*
 * called from debuggers context
 */
int do_enable_breakpoint(struct exec_context *ctx, void *addr)
{
	// invalid debugger context check
	if (!ctx || !ctx->dbg)
		return -1;

	// getting the head of breakpoint list
	struct breakpoint_info *head = ctx->dbg->head;
	if (head == NULL)
	{
		// no breakpoints present.
		return -1;
	}
	else
	{
		struct breakpoint_info *prev = NULL;

		// finding the breakpoint with given address.
		while (head != NULL)
		{
			if (head->addr == (u64)addr)
			{
				break;
			}
			prev = head;
			head = head->next;
		}

		// breakpoint with given address does not exist in the list.
		if (head == NULL)
			return -1;

		// getting debuggee's exec_context
		struct exec_context *child_ctx = get_ctx_by_pid(ctx->dbg->cpid);

		*(u8 *)(head->addr) = INT3_OPCODE;
		head->status = 1;
	}

	return 0;
}

/*
 * called from debuggers context
 */
int do_disable_breakpoint(struct exec_context *ctx, void *addr)
{
	// invalid debugger context check
	if (!ctx || !ctx->dbg)
		return -1;

	// getting the head of breakpoint list
	struct breakpoint_info *head = ctx->dbg->head;

	if (head == NULL)
	{
		// No breakpoints present
		return -1;
	}
	else
	{
		struct breakpoint_info *prev = NULL;

		// finding the breakpoint with given address.
		while (head != NULL)
		{
			if (head->addr == (u64)addr)
			{
				break;
			}
			prev = head;
			head = head->next;
		}

		// breakpoint with given address not present in the list.
		if (head == NULL)
			return -1;

		// reverting the changes in code segment.
		*(u8 *)(head->addr) = PUSHRBP_OPCODE;
		head->status = 0;
	}

	return 0;
}

/*
 * called from debuggers context
 */
int do_info_breakpoints(struct exec_context *ctx, struct breakpoint *bp)
{
	// invalid debugger context check
	if (!ctx || !ctx->dbg)
		return -1;

	// getting the head of the breakpoint list
	struct breakpoint_info *head = ctx->dbg->head;

	// filling in bp with breakpoint info
	int i = 0;
	while (head)
	{
		bp[i].addr = head->addr;
		bp[i].num = head->num;
		bp[i].status = head->status;
		head = head->next;
		i += 1;
	}

	return i;
}

/*
 * called from debuggers context
 */
int do_info_registers(struct exec_context *ctx, struct registers *regs)
{
	// invalid debugger context check
	if (!ctx || !ctx->dbg)
		return -1;

	// getting the debuggee context
	struct exec_context *child_ctx = get_ctx_by_pid(ctx->dbg->cpid);

	// invalid debuggee context check
	if (child_ctx == NULL)
		return -1;

	// copying the register values
	regs->entry_cs = child_ctx->regs.entry_cs;
	regs->entry_rflags = child_ctx->regs.entry_rflags;
	regs->entry_rip = child_ctx->regs.entry_rip - 1;
	regs->entry_rsp = child_ctx->regs.entry_rsp + 8;
	regs->entry_ss = child_ctx->regs.entry_ss;
	regs->r10 = child_ctx->regs.r10;
	regs->r11 = child_ctx->regs.r11;
	regs->r12 = child_ctx->regs.r12;
	regs->r13 = child_ctx->regs.r13;
	regs->r14 = child_ctx->regs.r14;
	regs->r15 = child_ctx->regs.r15;
	regs->r8 = child_ctx->regs.r8;
	regs->r9 = child_ctx->regs.r9;
	regs->rax = child_ctx->regs.rax;
	regs->rbp = child_ctx->regs.rbp;
	regs->rbx = child_ctx->regs.rbx;
	regs->rcx = child_ctx->regs.rcx;
	regs->rdi = child_ctx->regs.rdi;
	regs->rdx = child_ctx->regs.rdx;
	regs->rsi = child_ctx->regs.rsi;

	return 0;
}

/* 
 * Called from debuggers context
 */
int do_backtrace(struct exec_context *ctx, u64 bt_buf)
{
	// invalid debugger context check
	if (!ctx || !ctx->dbg)
		return -1;

	// filling in bt_buf by using already stored bt_info
	for (int i = 0; i < ctx->dbg->bt_size; i++)
	{
		*((u64 *)(bt_buf) + i) = ctx->dbg->bt_info[i];
	}

	return ctx->dbg->bt_size;
}

/*
 * When the debugger calls wait
 * it must move to WAITING state 
 * and its child must move to READY state
 */

s64 do_wait_and_continue(struct exec_context *ctx)
{
	// invalid debugger context check
	if (ctx == NULL || ctx->dbg == NULL)
		return -1;

	// getting the debuggee exec_context
	struct exec_context *child_ctx = get_ctx_by_pid(ctx->dbg->cpid);

	// invalid debuggee context check
	if (child_ctx == NULL)
		return -1;

	// changing debugger's state to waiting and debuggee's state to ready.
	ctx->state = WAITING;
	child_ctx->state = READY;

	// scheduling the debuggee
	schedule(child_ctx);

	return -1;
}
