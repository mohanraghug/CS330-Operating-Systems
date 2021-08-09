//testcase to check file open, write, dup2, fork, lseek SEEK_SET.

#include<ulib.h>
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
    char buff1[1405];
    char *filename="test.txt";
    int create_fd = open(filename, O_CREAT|O_RDWR, O_READ|O_WRITE);
    if(create_fd < 0){
        printf("file open failed\n");
        exit(0);
    }
    char *buff = "Plateau was born in Brussels.His father, born in Tournai, was a talented flower painter.At the age of six the younger Plateau was already able to read, making him a child prodigy in those times. While attending the primary schools, he was particularly impressed by a lesson of physics: enchanted by the experiments he observed, he vowed to discover their secrets someday.[3] Plateau spent his school holidays in Marche-Les-Dames, with his uncle and his family; his cousin and playfellow was Auguste Payen, who later became an architect and the principal designer of the Belgian railways.[3] At the age of fourteen he lost his father and mother; the trauma caused by this loss made him fall ill.On 27 August 1840 Plateau married Augustine–Thérèse–Aimée–Fanny Clavareau:[6] they had a son a year later.[7] His daughter Alice Plateau married Gustave Van der Mensbrugghe [nl] in 1871, who became his collaborator and later his first biographer.Fascinated by the persistence of luminous impressions on the retina, Plateau performed an experiment in which he gazed directly into the sun for 25 seconds. He lost his eyesight later in his life, and attributed the loss to this experiment. However, this may not be the case, and he may have instead suffered from chronic uveitis.Plateau became a foreign member of the Royal Netherlands Academy of Arts and Sciences in 1872.Plateau died in Ghent in 1883.";
    
    int written = write(create_fd,buff,1401);

    if(written < 0){
        printf("file write failed\n");
        exit(0);
    }

    int dup_fd = dup2(create_fd,4);
    
    if(dup_fd < 0){
        printf("dup failed\n");
        exit(0);
    }

    int pid = fork();
    if(pid){
        char buff[40];
        close(create_fd);
        int offp = lseek(dup_fd,0,SEEK_SET);
        int readr = read(dup_fd,buff,29);
        if(readr < 0){
            printf("read failed\n");
            exit(0);
        }
        buff[29] = '\0';
        close(dup_fd);
    }
    else{
        sleep(10);
        char buff[60];
        close(dup_fd);
        int readr = read(create_fd,buff,59);
        
        if(readr < 0){
            printf("read failed\n");
            exit(0);
        }
        buff[59] = '\0';
        if(!ustrcmp(buff,"His father, born in Tournai, was a talented flower painter."))
            printf("Testcase Passed\n");
        else
            printf("Test Failed\n");

        close(create_fd);
    }
        return 0;
  }
