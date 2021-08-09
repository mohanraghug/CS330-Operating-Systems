#include<ulib.h>

void* util_mmap(void * addr, int length, int prot, int flag)
{
  char* temp = mmap(addr, length, prot, flag);
  if((long)temp <= 0)
  {
    printf("Testcase Failed \n");
    exit(0);
  }
  return temp;
}

int util_munmap(void * addr, int length)
{
   int temp = munmap(addr, length);
   if(temp < 0)
   {
     printf("Testcase Failed \n");
     exit(0);
   }
   return temp;
}

int util_mprotect(void * addr, int length, int prot)
{

  int temp = mprotect(addr, length, prot);
  if(temp < 0)
  {
    printf("Testcase Failed \n");
    exit(0);
  }
  return temp;
}

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
  int pages = 4096;

  
 
  int total = 63;

  // Doing MMAP
  char * mm1= util_mmap(NULL, (pages*100), PROT_WRITE, 0);
  // vm_area count should be 1
  pmap(0);
  
  // Doing MMAP with fixed address
  for(int i =2; i < total; i=i+2)
  {
      char* vm_area_start = mm1 + (i * (pages*100));
      util_mmap(vm_area_start, pages*100, PROT_WRITE, MAP_FIXED);
  }
  // Vm_area count should be  63;
   pmap(0);

  // Doing MMAP such that End of the previous vm_area end will be expanded
  for(int i =1; i < total; i = i + 2)
  {
      char* vm_area_start = mm1 + (i * (pages*100));
      //unmapping 10 pages from front
      // printf("Requesting the address [%x]\n", (unsigned long)vm_area_start);
      util_mmap(vm_area_start, pages*10, PROT_WRITE, MAP_FIXED);
      
  }
  // Vm_area count should be  63;
  pmap(0);
  


  // Doing MMAP such that start of the next vm_area start will be expanded
  for(int i =1; i < total; i = i + 2)
  {
      char* vm_area_start = mm1 + (i * (pages*100));
      char* vm_area_back_10_pages = vm_area_start + (pages*90);
      util_mmap(vm_area_back_10_pages, pages*10, PROT_WRITE, MAP_FIXED);
  }
  // Vm_area count should be  63;
  pmap(0);


  // Doing MMAP such that all the vm_area will be merged to be one.(using hint address)
  for(int i =1; i < total; i = i + 2)
  {
       util_mmap(mm1, pages*80, PROT_WRITE, 0);
  }
  // vm_area count should be 1
  pmap(0);
  

  util_munmap(mm1, (pages*total *100));

  //Vm_area count should be  0;
  pmap(0);

return 0;
}
