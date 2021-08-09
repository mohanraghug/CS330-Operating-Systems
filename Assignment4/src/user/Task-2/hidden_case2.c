#include<ulib.h>

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
  int pages = 4096;
  int x;
  char * mm1 = mmap(NULL, pages, PROT_WRITE, MAP_POPULATE);
  if((long)mm1 < 0)
  {
    printf("Testcase failed\n");
    return 1;
  }
  mm1[0] = 'B';
  if(mm1[0] != 'B')
  {
    printf("Testcase failed\n");
    return 1;
  }
  // vm_area count should be 1 and page fault should be 0;
  pmap(0);


  int val1 = munmap((void*)mm1, pages);
  if(val1 < 0)
  {
    printf("Test case failed \n");
    return 1;
  }

  // Should be a seg fault. IF PTE and TLB is invalidated;
  mm1[0] = 'B';
  printf("Testcase failed\n");
  return 0;
}
