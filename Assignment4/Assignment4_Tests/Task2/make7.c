#include<ulib.h>

// checks merging of hugepages
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{

	int pageSize = 4096;
	u32 MB = 1 << 20;
	
	// 2MB aligned address
	u64 aligned = 0x180400000; 
	
	char* paddr = mmap((void *) aligned, 2 * MB, PROT_READ|PROT_WRITE, 0);
	char* hpaddr1 = (char *) make_hugepage(paddr, 2 * MB , PROT_READ|PROT_WRITE, 0);

	paddr = mmap(hpaddr1 + 4 * MB, 2 * MB, PROT_READ|PROT_WRITE, 0);
	char* hpaddr2 = (char *) make_hugepage(paddr, 2*MB , PROT_READ|PROT_WRITE, 0);
	
	// Non contiguous huge pages shouldn't be merged.
	// there must be 2 regions HP, HP
	pmap(1); 
	return 0;
}
