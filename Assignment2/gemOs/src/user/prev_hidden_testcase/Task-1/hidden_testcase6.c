//Check whether file offset is maintained properly for multiple writes.

#include<ulib.h>

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
    char *filename="test.txt";
    int create_fd = open(filename, O_CREAT|O_RDWR, O_READ|O_WRITE);

    if(create_fd < 0){
        printf("open failed\n");
        exit(0);
    }
    char *buff1 = "Hello, Man!!!";
    char *buff2 = "Plateau was born in Brussels.[3] His father, born in Tournai, was a talented flower painter.[4] At the age of six the younger Plateau was already able to read, making him a child prodigy in those times. While attending the primary schools, he was particularly impressed by a lesson of physics: enchanted by the experiments he observed, he vowed to discover their secrets someday.[3] Plateau spent his school holidays in Marche-Les-Dames, with his uncle and his family; his cousin and playfellow was Auguste Payen, who later became an architect and the principal designer of the Belgian railways.[3] At the age of fourteen he lost his father and mother; the trauma caused by this loss made him fall ill.On 27 August 1840 Plateau married Augustine–Thérèse–Aimée–Fanny Clavareau:[6] they had a son a year later.[7] His daughter Alice Plateau married Gustave Van der Mensbrugghe [nl] in 1871, who became his collaborator and later his first biographer.Fascinated by the persistence of luminous impressions on the retina, Plateau performed an experiment in which he gazed directly into the sun for 25 seconds. He lost his eyesight later in his life, and attributed the loss to this experiment. However, this may not be the case, and he may have instead suffered from chronic uveitis.Plateau became a foreign member of the Royal Netherlands Academy of Arts and Sciences in 1872.Plateau died in Ghent in 1883.";
   
    write(create_fd,buff1,13);
    write(create_fd,buff2,100);

    close(create_fd);
        
    char buff3[20];
    int read_fd = open(filename, O_READ);
    read(read_fd,buff3,13);
    read(read_fd,(void *)(&buff3[13]),7);
    buff3[20] = '\0';
    close(read_fd);
    if(!ustrcmp(buff3,"Hello, Man!!!Plateau"))
        printf("Testcase Passed\n");
    else
        printf("Testcase failed\n");
    return 0;
  }
