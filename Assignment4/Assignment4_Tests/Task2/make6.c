#include<ulib.h>

// checks force_prot = 1
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{

	int pageSize = 4096;
	u32 MB = 1 << 20;

	u64 aligned = 0x180400000; // 2MB aligned address

	char *addr1 = mmap((void *)aligned, 1 * MB, PROT_READ, 0);
	char *addr2 = mmap((void *)addr1 + (1 * MB), 1*MB, PROT_WRITE, 0);
	char *addr3 = mmap((void *)addr2 + (1 * MB), 1*MB, PROT_READ, 0);
	char *addr4 = mmap((void *)addr3 + (1 * MB), 1*MB, PROT_WRITE, 0);

	pmap(1);

	long ret = make_hugepage(addr1, 4 * MB , PROT_READ, 1);  	
	// hugepages must be created
	pmap(1);
	return 0;
}
