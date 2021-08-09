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

    // Test case when offset is NOT NULL

    fdr = open(filename1, O_CREAT|O_RDWR, O_READ|O_WRITE);
    int fdw =   open(filename3, O_CREAT|O_RDWR, O_READ|O_WRITE);

    long off = 5;
    sendfile(fdw, fdr, &off, 3);
    close(fdw);

    fdw =   open(filename3, O_CREAT|O_RDWR, O_READ|O_WRITE);

    read(fdw, buf2, 3);

    if (!(buf2[0] == 'f' && buf2[1]=='g' && buf2[2] == 'h')) {
        printf("Test failed\n");
        // printf("---\n");
        // printf("%c %c %c\n", buf2[0], buf2[1], buf2[2]);
        // printf("---\n");
        return -1;
    }

    if (off != 8) {
        printf("Test failed\n");
        return -1;
    }

    

    // is the offset modified for fdr (this is incorrect to do)

    read(fdr, buf2, 3);
    if (!(buf2[0] == 'a' && buf2[1]=='b' && buf2[2] == 'c')) {
        printf("Test failed\n");
        return -1;
    }
    close(fdr);


    printf("Test passed\n");
    return 0;


    /// if infile has 3 characters
    // sendfile 5 bytes read
    // offset is changed by 3;


    // return 0;
}
