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

  int total = 11;

  // Creating Bulk pages with PROT READ;
  char * mm1= util_mmap(NULL, (pages*10*total), PROT_READ, MAP_POPULATE);

  
  // vm_area count should be 1
  pmap(0);
  
  //Alernative changing access and unmapping for every 10 pages;

  for(int i =0; i < total; i++)
  {
      char* vm_area_start = mm1 + (i * (pages*10));
      if(i%2 != 0)
      {
        util_munmap(vm_area_start, pages*10);
      } else
      {
        util_mprotect(vm_area_start, pages*10, PROT_WRITE);
      }
      
  }
  // Vm_area count should be  6 and all will be PROT_WRITE (Physical pages already mapped for this area);
   pmap(0);
   
   // Merging all the vm_area. But below request vm_area have physical pages
   // 5 vm_area without physical pages
   for(int i =1; i< total; i = i+2)
   {
      char * mm1= util_mmap(mm1, (pages*10), PROT_WRITE, 0);
   }
  
  // Vm_area count should be  1
  pmap(0);

  // Accessing all the location. 
  for(int i =0 ; i < (pages*10*total); i++)
  {
      mm1[i] = 'X';
  }
  // Vm_area count will be 1 and Page fault count should be 50 (5 vm_area lazyly allocated and each has 10 pages)
  pmap(0);


  util_mprotect(mm1, (pages*10*total), PROT_READ);

  for(int i =0 ; i < (pages*10*total); i++)
  {
     if( mm1[i] != 'X')
     {
        printf("Testcase Failed\n");
        return 0;
     }
  }
  // Vm_area count will be 1 and Page fault count should be 50
  pmap(0);
  //util_munmap(mm1, (pages*10*total));

  // will result in fault as vm_area protection is read only.
  mm1[0] = 'X';
  printf("Test case Failed\n");

  return 0;
}