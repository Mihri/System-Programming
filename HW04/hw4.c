#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
//#include "restart.h"
#define PATH_MAX 255
#define MAXCHAR 1000
#define BUFSIZE 256
#define FIFO_PERM  (S_IRUSR | S_IWUSR)
#define FIFO_MODES O_RDONLY
//pid_t r_wait(int *stat_loc);
//#include <asm-generic/emergency-restart.h>

int dofifochild(const char *fifoname, const char *idstring);
int dofifoparent(const char *fifoname);

static int flag;
int isdirectory(char *path);
int sizepathfun (char *path);
int pathfun (char *path1);
int postOrderApply (char *path, int pathfun (char *path1));
pid_t p_parent;
pid_t id;

int main(int argc, char *argv[])
{
    int a;
                      
    if (argc == 2)//without -z format
    {
        flag=0;
        fprintf(stderr,"Usage: %s  FolderName\n", argv[0]);
        a = postOrderApply (argv[1], 0);
        
        printf(" %d    %d    %s\n", id,a, argv[1]); 
        //printf("%d child processes created. Main process is %d.\n",counter/2,p_parent);        
        exit(1);
    }
     if (argc == 3)//with -z format
    {
        flag=1;
        fprintf(stderr,"Usage: %s -z FolderName\n", argv[0]);
        a = postOrderApply (argv[2], 0);
        printf(" %d     %d    %s\n",id, a, argv[2]);
        //printf("%d child processes created. Main process is %d.\n",counter/2,p_parent);
        exit(1);
    }
    return 0;
}
int postOrderApply (char *find, int pathfun (char *path1))
{
    char mycwd[PATH_MAX];
    struct dirent *direntp;  
    struct stat status;
    struct stat st;
    DIR *directory=NULL;
    int all=0;
    int x=0;
    int temp=0;
    int temp2=0;
    id=getpid();
    pid_t childpid;
    p_parent=getppid();

    char bufin[BUFSIZE] = "empty";
    char bufout[BUFSIZE] = " ";
    int bytesin;

    int fd[2];
    if (pipe(fd) == -1) 
    {
        perror("Failed to create the pipe");
        return 1;
    }
    bytesin = strlen(bufin);

    /*if (mkfifo(find, FIFO_PERM) == -1) 
    {           

        if (errno != EEXIST) 
        {
            fprintf(stderr, "[%ld]:failed to create named pipe %s: %s\n",(long)getpid(), "151044096.txt", strerror(errno));
            return 1;
        }
    }*/


    if ((directory = opendir(find))== NULL)
    {
       fprintf(stderr,"Directory acilamadi veya bulunamadi.\n");
       exit(-1);
    }

    while ((direntp = readdir(directory)) != NULL) 
    {
        if (strcmp(direntp->d_name, ".")!= 0&&strcmp(direntp->d_name, "..")!= 0&&direntp->d_name[strlen(direntp->d_name) - 1] != '~')
        {
            sprintf(mycwd, "%s/%s", find, direntp->d_name);//finds the path
            
            if(lstat(mycwd,&st)== 0)
              x= st.st_size;
            childpid=fork();

            if(childpid==0)//if child process
            {
                if(isdirectory(mycwd)!=0)//if path is a directory 
                {
                           
                        if(flag==0)//if the input without -z format
                        {
                            temp=postOrderApply (mycwd, pathfun);                      
                            printf(" %d    %d    %s\n",id,temp,mycwd);//WRITE  
                            temp2=temp;
                            sprintf(bufout,"% d   %s",temp2,mycwd);
                            //return dofifochild("151044096.txt", bufout);                     
                        }

                        else if(flag==1)//if the input with -z format
                        {
                            temp=postOrderApply (mycwd, pathfun);
                            all+=temp;
                            temp2=temp;
                            //printf(" %d    %d    %s\n",id,temp,mycwd);//WRITE
                            //close(fd[0]);
                            sprintf(bufout," %d     %s",temp2,mycwd);
                            //char bufout[] = temp;
                            write(fd[1], bufout, strlen(bufout)+1);
                            //return dofifochild("151044096.txt", bufout);
                        }
                        //closedir(directory);
                         //_exit(0);
                    
              
                }
                else if(isdirectory(mycwd)==0)//If it's not directory
                {
                    all+=x;
                    if (S_ISLNK(st.st_mode) || S_ISBLK(st.st_mode) || S_ISCHR(st.st_mode) || S_ISFIFO(st.st_mode)) //If it's a special file
                    {
                      printf(" %d    Special File    %s\n",id,mycwd);
                    }
                    //return dofifochild("151044096.txt", "this was written by the child");
                }
            }
            else if(childpid<0)
            {
                printf("Fork failed\n");
                break;
            }
            else if(childpid>0)//if it's parent processs
            {  
                //childpid = waitpid(childpid,NULL,0);
                //close(fd[1]);
                bytesin = read(fd[0], bufin, BUFSIZE);
                fprintf(stderr, " %ld    %s    \n",(long)getpid(), bufin);
                //return dofifoparent("151044096.txt");

                exit(all);

            }

      }
  }    
    while ((closedir(directory) == -1 )&& (errno ==EINTR));
    //while (r_wait(NULL) > 0);
    return all;
}
/*pid_t r_wait(int *stat_loc) 
{
    int retval;
    while (((retval = wait(stat_loc)) == -1) && (errno == EINTR));
    return retval;
}*/
int isdirectory(char *path)
{
    struct stat statbuf;
    if(stat(path,&statbuf)==-1)
        return 0;
    else
        return S_ISDIR(statbuf.st_mode);
}
int pathfun (char *path1)
{
    struct stat st;
    if(stat(path1,&st)==0)
        return (st.st_size);
    else
        return -1;
}
int sizepathfun (char *path)
{
    struct stat st;
    if(stat(path,&st)==0)
        return (st.st_size);
    else
        return -1;
}


int dofifochild(const char *fifoname, const char *idstring) 
{
    char buf[BUFSIZE];
    int fd;
    int rval;
    ssize_t strsize;
    fprintf(stderr, "[%ld]:(child) about to open FIFO %s...\n",(long)getpid(), fifoname);
    while (((fd = open(fifoname, O_WRONLY)) == -1) && (errno == EINTR)) ;
    if (fd == -1) 
    {
        fprintf(stderr, "[%ld]:failed to open named pipe %s for write: %s\n",(long)getpid(), fifoname, strerror(errno));
        return 1; 
    }
    rval = snprintf(buf, BUFSIZE, "-----[%ld]:%s\n", (long)getpid(), idstring);
    if (rval < 0) 
    {
        fprintf(stderr, "[%ld]:failed to make the string:\n", (long)getpid());
        return 1; 
    }
    strsize = strlen(buf) + 1;
    fprintf(stderr, "[%ld]:about to write...\n", (long)getpid());
    printf(" buffff %s\n",buf );
    rval = write(fd, buf, strsize);
    if (rval != strsize) 
    {
        fprintf(stderr, "[%ld]:failed to write to pipe: %s\n",(long)getpid(), strerror(errno));
        return 1; 
    }
    fprintf(stderr, "[%ld]:finishing...\n", (long)getpid());
    return 0; 
}


int dofifoparent(const char *fifoname) 
{
    char buf[BUFSIZE];
    int fd;
    int rval;
    fprintf(stderr, "[%ld]:(parent) about to open FIFO %s...\n",(long)getpid(), fifoname);
    while (((fd = open(fifoname, FIFO_MODES)) == -1) && (errno == EINTR))  ;
    if (fd == -1) 
    {
        fprintf(stderr, "[%ld]:failed to open named pipe %s for read: %s\n",(long)getpid(), fifoname, strerror(errno));
        return 1; 
    }
    fprintf(stderr, "[%ld]:about to read...\n", (long)getpid());
    rval = read(fd, buf, BUFSIZE);
    if (rval == -1) 
    {
        fprintf(stderr, "[%ld]:failed to read from pipe: %s\n",(long)getpid(), strerror(errno));
        return 1; 
    }
    fprintf(stderr, "[%ld]:read %.*s\n", (long)getpid(), rval, buf);
    return 0; 
}