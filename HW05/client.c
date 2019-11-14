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
    int fd1,fd2,fd3; 
    int id;
    int client;
    int server_time;
    char  myfifo[20] = "myServer"; 
    char  myfifo2[20] = "myClient";
    char  myfifo3[20] = "trasnfer";
    mkfifo(myfifo3, 0666); 
  
    char str1[80], str2[80]; 

    signal(SIGINT,  sig_handler);
    signal(SIGUSR1, sig_handler);
    signal(SIGKILL, sig_handler);
    signal(SIGSTOP, sig_handler);

    
    if (argc != 2)
    {
        fprintf(stderr,"Usage: %s Client_Number\n", argv[0]);       
        //exit(1);
    }
    if (argc == 2)
    {
        client=atoi(argv[1]);
    }

    fd3 = open(myfifo3, O_WRONLY);
    sprintf(str1,"%d",client); 
    write(fd3, str1, strlen(str1)+1);
    close(fd3);

    fd3 = open(myfifo3, O_RDONLY);
    read(fd3, str2, 80); 
    //printf("%s\n", str2);
    server_time = atoi(str2);
    printf("%d",server_time);
    close(fd3);
    

    for(int i=0;i<client;++i)
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

                fd2 = open(myfifo2,O_WRONLY); 
                //fgets(str2, 80, stdin); 
                id=getpid();
                sprintf(str2,"%d",id);
                write(fd2, str2, strlen(str2)+1); 
                close(fd2);


                fd1 = open(myfifo,O_RDONLY); 
                read(fd1, str1, 80); 
                printf("%s\n", str1); 
                close(fd1);    
            }
        } 
        else 
        {
            printf("parent");
        }
    }
    return 0; 
} 