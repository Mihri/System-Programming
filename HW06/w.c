//I put some codes from lecture book

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "restart.h"
#define MAXNAME 100
#define R_FLAGS O_RDONLY
#define W_FLAGS (O_WRONLY | O_CREAT)
#define W_PERMS (S_IRUSR | S_IWUSR)

int numcopiers;
char destination[100];

typedef struct
{
   int args[3];
   pthread_t tid;
}copy_t;

void *copyfilepass(void *arg);

int foo(char *find);
int isdirectory(char *path);


int main(int argc, char *argv[])
{
    if (argc != 5)
    {
           fprintf(stderr, "Usage: %s number_of_consumers buffer_size source_dir_path dest_dir_path\n", argv[0]);
           return 1;
    }

    numcopiers = atoi(argv[1]);
    strcpy(destination,argv[4]);

    foo(argv[3]);
   
    return 0;
}

int foo(char *find)
{
    int *bytesp;
    copy_t *copies;
    int error;
    char filename[MAXNAME];
    int i;
   
    int totalbytes = 0;
    char mycwd[PATH_MAX];
    char mydest[PATH_MAX];
    struct dirent *direntp; 
    struct stat status;
    struct stat st;
    DIR *directory;
    int x=0;

     
    if ((directory = opendir(find))== NULL)
    {
       fprintf(stderr,"Source directory acilamadi veya bulunamadi.\n");
       exit(-1);
    }

    /*if ((directory = opendir(destination))== NULL)
    {
       fprintf(stderr,"Target directory acilamadi veya bulunamadi.\n");
       exit(-1);
    }*/

    while ((direntp = readdir(directory)) != NULL)
    {
        if (strcmp(direntp->d_name, ".")!= 0&&strcmp(direntp->d_name, "..")!= 0&&direntp->d_name[strlen(direntp->d_name) - 1] != '~')
        {
            sprintf(mycwd, "%s/%s", find, direntp->d_name);//finds the path
            if(lstat(mycwd,&st)== 0)
              x= st.st_size;

            if(isdirectory(mycwd)!=0)//if path is a directory
            {
               
                foo(mycwd);   
            }
            else
            {
                if ((copies = (copy_t *)calloc(numcopiers, sizeof(copy_t))) == NULL)
                {
                       perror("Failed to allocate copier space");
                       return 1;
                }
                       /* open the source and destination files and create the threads */
                for (i = 0; i < numcopiers; i++) 
                {
                        copies[i].tid = pthread_self();  
                        if (snprintf(filename, MAXNAME, "%s", mycwd) == MAXNAME) 
                        {
                            fprintf(stderr, "Input filename %s too long", mycwd);
                            continue; 
                        }
                       if ((copies[i].args[0] = open(filename, R_FLAGS)) == -1) 
                       {
                        fprintf(stderr, "Failed to open source file %s: %s\n",filename, strerror(errno));
                          continue; 
                        }
                       
                      sprintf(mydest, "%s/%s", destination, direntp->d_name);//finds the path
                      /*if (snprintf(filename, MAXNAME, "%s", mydest) == MAXNAME)
                      {
                          fprintf(stderr, "Output filename %s too long", destination);
                           continue;
                      }*/
                      if (snprintf(filename, MAXNAME, "%s", mydest) == MAXNAME) 
                      {
                          fprintf(stderr, "Output filename %s too long",destination);
                          continue;
                      }
                      if ((copies[i].args[1] = open(filename, W_FLAGS, W_PERMS)) == -1) 
                      {
                            fprintf(stderr, "Failed to open destination file %s: %s\n",filename, strerror(errno));
                                continue; 
                      }
                         
                       
                       if (error == pthread_create((&copies[i].tid), NULL,copyfilepass, copies[i].args)) 
                       {
                          fprintf(stderr, "Failed to create thread %d: %s\n", i + 1,strerror(error));
                          copies[i].tid = pthread_self();    /* cannot be value for new thread */
                        }
                }
                              /* wait for the threads to finish and report total bytes */
                for (i = 0; i < numcopiers; i++) 
                {
                    if (pthread_equal(copies[i].tid, pthread_self()))        /* not created */
                        continue;
                    if (error == pthread_join(copies[i].tid, (void**)&bytesp)) 
                    {
                        fprintf(stderr, "Failed to join thread %d\n", i);
                        continue;
                    }

                    if (bytesp == NULL) 
                    {
                        fprintf(stderr, "Thread %d failed to return status\n", i);
                        continue; 
                    }
                    
                    printf("Thread %d copied %d bytes from %s to %s\n",i, *bytesp, mycwd, destination);
                    totalbytes += *bytesp;
                }
                printf("Total bytes copied = %d\n", totalbytes);

            }
        }
    }
    return 0;
}

void *copyfilepass(void *arg) 
{
   int *argint;
   argint = (int *)arg;
   argint[2] = copyfile(argint[0], argint[1]);
   close(argint[0]);
   close(argint[1]);
   return argint + 2;
}

int isdirectory(char *path)
{
    struct stat statbuf;
    if(stat(path,&statbuf)==-1)
        return 0;
    else
        return S_ISDIR(statbuf.st_mode);
}