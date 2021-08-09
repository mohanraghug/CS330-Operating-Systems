#include<ulib.h>

void* util_mmap(void * addr, int length, int prot, int flag)
{
  char* temp = mmap(addr, length, prot, 0);
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

  // Creating Bulk pages;
  char * mm1= util_mmap(NULL, (pages*100*total), PROT_WRITE, 0);
  // vm_area count should be 1
  pmap(0);
  
  // Changing PROTECTION for every 100 alternate pages 
  for(int i =0; i < total; i++)
  {
      int prot = i % 2 == 0? PROT_WRITE : PROT_READ;
      char* vm_area_start = mm1 + (i * (pages*100));
      util_mprotect(vm_area_start, pages*100, prot);
  }
  // Vm_area count should be  63;
   pmap(0);

  // Changing protection 10 PAGES FROM FRONT. So that End of the previous vm_area end will be expanded
  for(int i =1; i < total; i = i + 2)
  {
      char* vm_area_start = mm1 + (i * (pages*100));
      //unmapping 10 pages from front
      util_mprotect(vm_area_start, pages*10, PROT_WRITE);
  }
  // Vm_area count should be  63;
  pmap(0);


  // Changing protection 10 PAGES FROM Back. So that End of the next vm_area start will be expanded
  for(int i =1; i < total; i = i + 2)
  {
      char* vm_area_start = mm1 + (i * (pages*100));
      char* vm_area_back_10_pages = vm_area_start + (pages*90);
      util_mprotect(vm_area_back_10_pages, pages*10, PROT_WRITE);
  }
  // Vm_area count should be  63;
  pmap(0);


  // Changing protection in the center of vm_area, so that vm_Area will be splitted
  for(int i =1; i < total; i = i + 2)
  {
        char* vm_area_start = mm1 + (i * (pages*100)) + pages*20;
        util_mprotect(vm_area_start, pages*10, PROT_WRITE);
  }
  // Vm_area count should be  125;
  pmap(0);


  // Will merge all the vm_area (PROT_READ) which was splitted earlier.
  for(int i =1; i < total;  i=i+2)
  {
     char* vm_area_start = mm1 + (i * (pages*100)) + pages*20;
     util_mprotect(vm_area_start, pages*10, PROT_READ);
  }
  // Vm_area count should be  63;
  pmap(0);


  // Changing the PROT_READ to PROT_WRITE. So that all the vm_area will be merged to be one.
  for(int i =1; i < total; i = i + 2)
  {
      char* vm_area_start = mm1 + (i * (pages*100 ));
      char * read_area_begining = vm_area_start + (pages*10 );
      //unmapping 10 pages from front
      util_mprotect(read_area_begining, pages*80, PROT_WRITE);
  }
  // vm_area count should be 1
   pmap(0);


  util_munmap(mm1, (pages*total *100));

  //Vm_area count should be  0;
  pmap(0);

return 0;
}
