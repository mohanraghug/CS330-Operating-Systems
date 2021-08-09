#include<ulib.h>

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
    int pid;
    char buf1[50]; // changed from 20 in sample test to 50
    char buf2[50];

    char *filename1="test1.txt";
    char *filename2="test2.txt";
    char *filename3="test3.txt";

    int fdr = open(filename1, O_CREAT|O_RDWR, O_READ|O_WRITE);
    char *buf = "abcdefghijklmnopqrstuvwxyz";
    write(fdr, buf, 26+1);
    close(fdr);

    fdr = open(filename1, O_CREAT|O_RDWR, O_READ|O_WRITE);
    int fdw =   open(filename2, O_CREAT|O_RDWR, O_READ|O_WRITE);

    // Test case when offset is NULL
    sendfile(fdw, fdr, NULL, 3);
    close(fdw);


    fdw =   open(filename2, O_CREAT|O_RDWR, O_READ|O_WRITE);
    read(fdw, buf2, 3);
    if (!(buf2[0] == 'a' && buf2[1]=='b' && buf2[2] == 'c')) {
        printf("Test failed\n");
        return -1;
    }


    // offset modified correctly for fdr
    read(fdr, buf2, 3);
    if (!(buf2[0] == 'd' && buf2[1]=='e' && buf2[2] == 'f')) {
         printf("Test failed\n");
        return -1;
    }
 

    close(fdr);
    close(fdw);

    printf("Test passed\n");

    /// if infile has 3 characters
    // sendfile 5 bytes read
    // offset is changed by 3;


    return 0;
}
