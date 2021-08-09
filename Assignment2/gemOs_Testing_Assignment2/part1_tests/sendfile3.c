#include<ulib.h>

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
    int pid;
    char buf1[50]; // changed from 20 in sample test to 50
    char buf2[50];

    char *filename1="test1.txt";
    char *filename2="test2.txt";
    char *filename3="test3.txt";

    /// if infile has 27 characters
    // sendfile 100 bytes read
    // offset is changed by 27;

    int fdr = open(filename1, O_CREAT|O_RDWR, O_READ|O_WRITE);
    char *buf = "abcdefghijklmnopqrstuvwxyz";
    write(fdr, buf, 26+1);
    close(fdr);

    // Test case when offset is NOT NULL

    fdr = open(filename1, O_CREAT|O_RDWR, O_READ|O_WRITE);
    int fdw = open(filename3, O_CREAT|O_RDWR, O_READ|O_WRITE);

    long off = 0;
    sendfile(fdw, fdr, &off, 100);
    // close(fdw);

    if (off != 27) {
        printf("Test failed\n");
        // printf("offset is %d\n", off);
        return -1;
    }

    printf("Test passed\n");
    return 0;
}
