#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include "restart.h"
#include <sys/socket.h> 
#include <netinet/in.h> 

#define MAXNAME 50
#define R_FLAGS O_RDONLY
#define W_FLAGS (O_WRONLY | O_CREAT)
#define W_PERMS (S_IRUSR | S_IWUSR)
FILE *ptr;

typedef struct 
{
    int args[3];
    pthread_t tid;
}copy_t;

struct package
{    
    char pathName[1024];
    pid_t sendpid;
    int threadPoolsize;
};
pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;
struct package Client;
pid_t client_pid; 
int *bytesp;
copy_t *copies;
int error;
char filename[MAXNAME];
int i;
int numcopiers;
int totalbytes = 0;
char target[100];
static void signal_handler(int sig);
struct sigaction act;

void *copyfilepass(void *arg)
{
    int *argint;
    argint = (int *)arg;
    argint[2] = copyfile(argint[0], argint[1]);
    r_close(argint[0]);
    r_close(argint[1]);
    return argint + 2;
}

int foo(char *find)
{
    DIR *directory;
    struct dirent *direntp;
    char mycwd[PATH_MAX];
    char mydest[PATH_MAX];
    struct stat st;
     
    if ((directory = opendir(find))== NULL)
    {
       fprintf(stderr,"Source directory acilamadi veya bulunamadi.\n");
       fprintf(ptr,"Source directory acilamadi veya bulunamadi.\n");
       exit(-1);
    }

    /*
    sigset_t block_mask;
    sigemptyset(&block_mask);
    sigaddset(&block_mask, SIGINT);
    sigaddset(&block_mask, SIGUSR1);
    pthread_sigmask(SIG_BLOCK, &block_mask, NULL);*/


        while((direntp = readdir(directory)) != NULL)
        {

            if(strcmp(direntp->d_name,".") != 0 && strcmp(direntp->d_name,"..") != 0 && direntp->d_name[strlen(direntp->d_name) - 1] != '~' )
            {
                sprintf(mycwd,"%s/%s", find, direntp->d_name);

                if (lstat(mycwd, &st) != 0)
                    continue;

                if(S_ISDIR(st.st_mode))
                {
                    foo(mycwd);
                }
                else 
                {
                    if ((copies = (copy_t *)calloc(Client.threadPoolsize, sizeof(copy_t))) == NULL) 
                    {
                        perror("Failed to allocate copier space");
                        fprintf(ptr,"Failed to allocate copier space\n ");
                    }
                    for (i = 0; i < Client.threadPoolsize; i++) 
                    {
                        copies[i].tid = pthread_self();
                        if (snprintf(filename, MAXNAME, "%s", mycwd) == MAXNAME) 
                        {
                            fprintf(stderr, "Input filename %s.%d too long", mycwd, i + 1);
                            fprintf(ptr,"Input filename %s.%d too long", mycwd, i + 1);
                            continue;
                        }
                        if ((copies[i].args[0] = open(filename, R_FLAGS)) == -1) 
                        {
                            fprintf(stderr, "Failed to open source file %s: %s\n",filename, strerror(errno));
                            fprintf(ptr,"Failed to open source file %s:\n",filename);
                            continue;
                        }
                        sprintf(mydest,"%s/%s", Client.pathName , direntp->d_name);

                        if (snprintf(filename, MAXNAME, "%s", mydest) == MAXNAME) 
                        {
                            fprintf(stderr, "Output filename %s.%d too long", target, i + 1);
                            fprintf(ptr,"Output filename %s.%d too long", target, i + 1);
                            continue;
                        }
                        if ((copies[i].args[1] = open(filename, W_FLAGS, W_PERMS)) == -1) 
                        {
                            fprintf(stderr, "Failed to open destination file %s: %s\n",filename, strerror(errno));
                            fprintf(ptr,"Failed to open destination file %s:\n",filename);
                            continue;
                        }
                        if (error = pthread_create((&copies[i].tid), NULL,copyfilepass, copies[i].args)) 
                        {
                            fprintf(stderr, "Failed to create thread %d: %s\n", i + 1,strerror(error));
                            fprintf(ptr,"Failed to create thread %d:\n", i + 1);

                            copies[i].tid = pthread_self();
                        }
                    }
                    for (i = 0; i < Client.threadPoolsize; i++) 
                    {
                        if (pthread_equal(copies[i].tid, pthread_self()))
                        continue;

                        if (error = pthread_join(copies[i].tid, (void**)&bytesp)) 
                        {
                            fprintf(stderr, "Failed to join thread %d\n", i);
                            fprintf(ptr,"Failed to join thread %d\n", i);

                            continue;
                        }
                        if (bytesp == NULL) 
                        {
                            fprintf(stderr, "Thread %d failed to return status\n", i);
                            fprintf(ptr,"Thread %d failed to return status\n", i);
                            continue;
                        }
                        printf("Thread %d copied %d bytes from %s to %s\n",i, *bytesp, mycwd, target);
                        fprintf(ptr,"Thread %d copied %d bytes from %s to %s\n",i, *bytesp, mycwd, target);
                        totalbytes += *bytesp;
                    }
                    printf("Total bytes copied = %d\n", totalbytes);
                    fprintf(ptr,"Total bytes copied = %d\n", totalbytes);
                }
            }
        }
    return 0;

}

int main(int argc, char *argv[]) 
{
    struct sockaddr_in address; 
    int client_sock = 0, valread; 
    struct sockaddr_in serv_addr; 
    char buffer[1024] = {0}; 
    char * mycwd;


    ptr=fopen("Client_Output.log","a+");

    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s [dirName] [ip address] [portnumber] \n", argv[0]);
        fprintf(stderr, "\n\n-Sample");
        fprintf(stderr, "\n./client A 127.0.0.1 1071\n");
    }
    int port_number = atoi(argv[3]);

    act.sa_handler = signal_handler;
    act.sa_flags = 0;

    if ((sigemptyset(&act.sa_mask) == -1) || (sigaction(SIGINT, &act, NULL) == -1)  || (sigaction(SIGUSR1, &act, NULL) == -1) 
                                          || (sigaction(SIGPIPE, &act, NULL) == -1) || (sigaction(SIGQUIT, &act, NULL) == -1)
                                          || (sigaction(SIGHUP, &act, NULL) == -1)  || (sigaction(SIGTERM, &act, NULL) == -1)) {
        perror("Signal set error!");
        fprintf(ptr,"Signal set error!");
        exit(EXIT_FAILURE);
    }

    if ((client_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n");
        fprintf(ptr,"Signal set error!"); 
        return -1; 
    } 
   
    memset(&serv_addr, '0', sizeof(serv_addr)); 
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(port_number); 
       
    if(inet_pton(AF_INET, argv[2], &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n");
        fprintf(ptr,"\nInvalid address/ Address not supported \n"); 
        return -1; 
    }  
       
    if (connect(client_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n");
        fprintf(ptr,"\nConnection Failed \n");
        return -1; 
    } 
    while(1)
       {

        pthread_mutex_lock(&client_mutex);

        r_read(client_sock,&Client.pathName,sizeof(char)*1024);
        printf("Client pathName  %s\n ",Client.pathName);
        fprintf(ptr,"Client pathName  %s\n ",Client.pathName);
        r_read(client_sock,&Client.threadPoolsize,sizeof(int));
        printf("Threadpool size %d\n ",Client.threadPoolsize);
        fprintf(ptr,"Threadpool size  %d\n ",Client.threadPoolsize);

        pid_t client_pid = getpid();
        write(client_sock, &client_pid, sizeof (pid_t));
        printf("Client pid  %d\n ",client_pid);
        fprintf(ptr,"Client pid  %d\n ",client_pid);
        foo(argv[1]);
        pthread_mutex_unlock(&client_mutex);
    }
    close(client_sock);
    fclose(ptr);
    return 0;
}

static void signal_handler(int sig) 
{
    sigset_t block_mask;

    if (sig == SIGINT) 
    {
        sigemptyset(&block_mask);
        sigaddset(&block_mask, SIGINT);
        pthread_sigmask(SIG_BLOCK, &block_mask, NULL);
    }


    if (sig == SIGUSR1 || sig == SIGINT) 
    {
        //pthread_cancel(waiting_thread);
        //pthread_join(waiting_thread, NULL);
    }

    if (sig == SIGINT) 
    {
        fprintf(stderr, "\nClient : " "SIGINT signal detected!\n");
        exit(EXIT_FAILURE);

    }

    if (sig == SIGUSR1) 
    {
        fprintf(stderr, "\nClient : " "Server terminated!\n");
        //close(client_sock);
        exit(EXIT_FAILURE);
    }

    if (sig == SIGPIPE) 
    {
        fprintf(stderr, "\nClient : " "File transfer failed!\n");
        //close(client_sock);
        exit(EXIT_FAILURE);
    }

    if(sig == SIGQUIT || sig == SIGTERM || sig == SIGHUP){
        fprintf(stderr, "\nClient : " "Unexpected termination\n" );
        //close(client_sock);
        exit(EXIT_FAILURE);
    } 
}
