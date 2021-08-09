/* This test case is to check do file exit implementation. child exits and parent closes files, stats should give three */

#include<ulib.h>
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
    int pid;
    char *filename="alan.txt";
    int create_fd = open(filename, O_CREAT|O_RDWR, O_READ|O_WRITE);
    char *buf = "Turing was born in Maida Vale,London,while his father,Julius Mathison Turing (1873–1947), was on leave from his position with the Indian Civil Service (ICS) at Chatrapur, then in the Madras Presidency and presently in Odisha state, in India.[17][18] Turing's father was the son of a clergyman, the Rev. John Robert Turing, from a Scottish family of merchants that had been based in the Netherlands and included a baronet. Turing's mother, Julius' wife, was Ethel Sara Turing (née Stoney 1881–1976),[7] daughter of Edward Waller Stoney, chief engineer of the Madras Railways. The Stoneys were a Protestant Anglo-Irish gentry family from both County Tipperary and County Longford, while Ethel herself had spent much of her childhood in County Clare";
    
    int written = write(create_fd, buf, 750);

    if(written < 0){
        printf("Write failed\n");
        exit(0);
    }
    
    close(create_fd);

    int read_fd = open(filename,O_READ);    
        
    pid = fork();

    if(pid){
        sleep(10);
        close(read_fd);
    }
    else{
        exit(0);
    }

    if(get_stats() == 3)
        printf("Testcase Passed\n");
    else
        printf("Testcase failed\n");

    return 0;
  }
