#include<ulib.h>

// munmap with normal and huge pages


int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
    u32 KB = 1 << 10;
    u32 MB = 1 << 20;
    u64 aligned = 0x180400000; // 2MB aligned address
	
    char* paddr = mmap((void *)aligned, 2*MB, PROT_READ|PROT_WRITE, 0);
    pmap(1); // vm area 1

    char* hpaddr1 = (char *) make_hugepage(paddr, 2*MB , PROT_READ|PROT_WRITE, 0);
    pmap(1); // vm area 1

    // hpaddr1[0] = 'A';

    paddr = mmap(hpaddr1 + 4*MB, 2*MB, PROT_READ|PROT_WRITE, 0);
    pmap(1); // vm area 2

    munmap((void *)aligned + 4*KB, 6*MB - 4*KB);

    pmap(1); // vm area 0


}
