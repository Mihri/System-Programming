#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <signal.h>
void sig_handler(int signo)/*signals*/
{
  if(signo == SIGINT || signo == SIGUSR1 || signo == SIGKILL || signo == SIGSTOP)
      printf("Exit condition: due to signal no#%d\n",signo);
  exit(0);
}
int main(int argc, char *argv[]) 
{ 
    int i;
    int time;
    int number,money,client_process;
    int fd,fd2,fd3; 
    char  myfifo[20] = "myServer"; 
    char  myfifo2[20] = "myClient";
    char  myfifo3[20] = "trasnfer"; 

    mkfifo(myfifo, 0666);
    mkfifo(myfifo3, 0666);
    char arr1[80], arr2[80];

    signal(SIGINT,  sig_handler);
    signal(SIGUSR1, sig_handler);
    signal(SIGKILL, sig_handler);
    signal(SIGSTOP, sig_handler);


    if (argc != 2)
    {
        fprintf(stderr,"Usage: %s Time\n", argv[1]); 
        //exit(1);
    }
    if (argc == 2)
    {
        time=atoi(argv[1]);
    }


    fd3 = open(myfifo3, O_RDONLY); 
    read(fd3, arr1, sizeof(arr1));
    client_process = atoi(arr1);
    printf("%d",client_process);
    close(fd3);


    fd3 = open(myfifo3, O_WRONLY);
    sprintf(arr2,"%d",time); 
    write(fd3, arr2, strlen(arr2)+1);
    close(fd3);



    for(int i=0;i<4;++i)
    {
        pid_t childpid;
        childpid=fork();
        
        if( childpid <0 ) 
        {
            printf("Fork failed\n");
            //exit(0); 
        }
        else if ( childpid == 0 ) 
        {
            printf("child\n");
            while (1) 
            { 
                fd2 = open(myfifo2, O_RDONLY); 
                read(fd2, arr1, sizeof(arr1)); 
                /*printf("User2: %s\n", arr1); */
                number = atoi(arr1);

                printf("Client id: %d\n", number); 
                close(fd2);
                
                fd = open(myfifo, O_WRONLY); 
                /*fgets(arr2, 80, stdin);*/
                money = rand() % 100 + 1;
                sprintf(arr2,"Musteri %d %d lira aldi :)",number,money); 
                write(fd, arr2, strlen(arr2)+1); 
                close(fd); 
            
                
            } 
        } 
        else 
        {
            printf("parent");
        }
    }

    return 0; 
} 