#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <signal.h>
static void signal_handler(int sig);
struct package
{
    pid_t pid;
    char pathName[1024];
    pid_t sendpid;
    int threadPoolsize;
};
struct package Clientx;
FILE *fptr;

int main(int argc, char const *argv[]) 
{ 
    int server_fd, serv_sock, valread; 
    struct sockaddr_in address;
    struct sigaction act; 
    int opt = 1; 
    int addrlen = sizeof(address); 
    char buffer[1024] = {0};  
       
    fptr=fopen("Server_Output.log","a+");
    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s [directory] [threadPoolSize] [portnumber] \n", argv[0]);
        fprintf(stderr, "\n\n-Sample");
        fprintf(stderr, "\n./biBakBOXServer A 50 1995\n");
        return 1;
    }
    
    int port_number = atoi(argv[3]);
    strcpy(Clientx.pathName,argv[1]);
    Clientx.threadPoolsize = atoi(argv[2]);
    act.sa_handler = signal_handler;
    act.sa_flags = 0;
    if ((sigemptyset(&act.sa_mask) == -1) || (sigaction(SIGINT, &act, NULL) == -1) || (sigaction(SIGPIPE, &act, NULL) == -1)) 
    {
        perror("Signal set error!");
        fprintf(fptr,"Signal set error!");
        exit(EXIT_FAILURE);
    }
    /*
     sigset_t block_mask;
    sigemptyset(&block_mask);
    sigaddset(&block_mask, SIGPIPE);
    pthread_sigmask(SIG_BLOCK, &block_mask, NULL);*/

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        fprintf(fptr,"socket failed");
        exit(EXIT_FAILURE); 
    } 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    { 
        perror("setsockopt");
        fprintf(fptr,"setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons(  port_number ); 
       
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) 
    { 
        perror("bind failed"); 
        fprintf(fptr,"bind failed");
        exit(EXIT_FAILURE); 
    } 
    if (listen(server_fd, 3) < 0) 
    { 
        perror("listen"); 
        fprintf(fptr,"listen");
        exit(EXIT_FAILURE); 
    } 

    while(1)
    {
        if ((serv_sock = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) 
        { 
            perror("accept");
            fprintf(fptr,"accept"); 
            exit(EXIT_FAILURE); 
        } 

        printf("Client pathName  %s\n ",Clientx.pathName);
        fprintf(fptr,"Client pathName  %s\n ",Clientx.pathName);

        printf("Threadpool size %d\n ",Clientx.threadPoolsize);
        fprintf(fptr,"Threadpool size %d\n ",Clientx.threadPoolsize);

        r_write(serv_sock,&Clientx.pathName,sizeof(char)*1024);
        r_write(serv_sock,&Clientx.threadPoolsize,sizeof(int));
        r_read(serv_sock,&Clientx.pid,sizeof(int));

        //printf("Clientx.threadPoolsize %d\n ",Clientx.threadPoolsize);
        //fprintf(fptr,"Clientx.threadPoolsize %d\n ",Clientx.threadPoolsize);

    }
    close(serv_sock);
    fclose(fptr);

    return 0; 
} 

static void signal_handler(int sig) 
{
    if(sig==SIGINT)
    {
        fprintf(stderr,"CTRL-C Signal Handled!\n");
        fprintf(fptr,"CTRL-C Signal Handled!\n");
        kill(Clientx.pid,SIGINT);
    }
    exit(sig);
}