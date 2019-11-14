#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#define PATH_MAX 255
#define MAXCHAR 1000
static int flag;
static int counter=0;
pid_t p_parent;
pid_t id;
FILE *f, *f2;
//pid_t r_wait(int *stat_loc);
int isdirectory(char *path);
int sizepathfun (char *path);
int pathfun (char *path1);
int postOrderApply (char *path, int pathfun (char *path1));

void sig_handler(int signo)/*signals*/
{
  if(signo == SIGINT || signo == SIGUSR1 || signo == SIGKILL || signo == SIGSTOP)
      printf("Exit condition: due to signal no#%d\n",signo);
  exit(0);
}
int main(int argc, char *argv[])
{
    int a;
    f = fopen("151044096sizes.txt", "a+");                      
    if (argc == 2)//without -z format
    {
        flag=0;
        fprintf(stderr,"Usage: %s  FolderName\n", argv[0]);
        a = postOrderApply (argv[1], 0);
        fprintf(f," %d    %d    %s\n", id, a, argv[1]);
        printf(" %d    %d    %s\n", id, a, argv[1]); 
        fprintf(f,"%d child processes created. Main process is %d.\n",counter/2,p_parent);//when you do fork it makes 2 child, because of that i divide
        printf("%d child processes created. Main process is %d.\n",counter/2,p_parent);        
        exit(1);
    }
     if (argc == 3)//with -z format
    {
        flag=1;
        fprintf(stderr,"Usage: %s -z FolderName\n", argv[0]);
        a = postOrderApply (argv[2], 0);
        fprintf(f," %d    %d    %s\n", id, a, argv[2]);
        printf(" %d    %d    %s\n", id, a, argv[2]);
        fprintf(f,"%d child processes created. Main process is %d.\n",counter/2,p_parent);
        printf("%d child processes created. Main process is %d.\n",counter/2,p_parent);
        exit(1);
    }
    fclose(f);
    return 0;
}
int postOrderApply (char *find, int pathfun (char *path1))
{
    char mycwd[PATH_MAX];
    struct dirent *direntp;  
    struct stat status;
    struct stat st;
    DIR *directory;
    int all=0;
    int x=0;
    int temp=0;
    id=getpid();
    pid_t childpid;
    p_parent=getppid();
    char buffer[MAXCHAR];
    int fd,fr;
    struct flock lock;
    char ch;
    FILE *f1;
    signal(SIGINT,  sig_handler);
    signal(SIGUSR1, sig_handler);
    signal(SIGKILL, sig_handler);
    signal(SIGSTOP, sig_handler);

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

            if(isdirectory(mycwd)!=0)//if path is a directory 
            {
                childpid=fork();//makes fork
                //++counter;
                if(childpid==0)//if child process
                {
                    f = fopen("151044096sizes.txt", "a+");                    
                    fd = open ("151044096sizes.txt", O_WRONLY);//Opens the file for lock, write only format
                    
                    if(flag==0)//if the input without -z format
                    {
                        temp=postOrderApply (mycwd, pathfun);
                        memset (&lock, 0, sizeof(lock));//LOCK
                        lock.l_type = F_WRLCK;//LOCK
                        fcntl (fd, F_SETLKW, &lock);//LOCK
                        fprintf(f," %d    %d    %s\n",id,temp,mycwd);//WRITE
                        lock.l_type = F_UNLCK;//UNNLOCK
                        fcntl (fd, F_SETLKW, &lock);//UNLOCK
                        fclose(f);
                        close(fd);
                    }

                    else if(flag==1)//if the input with -z format
                    {
                        temp=postOrderApply (mycwd, pathfun);
                        all+=temp;
                        memset (&lock, 0, sizeof(lock));//LOCK
                        lock.l_type = F_WRLCK;//LOCK
                        fcntl (fd, F_SETLKW, &lock);//LOCK
                        fprintf(f," %d    %d    %s\n",id,temp,mycwd);//WRITE
                        lock.l_type = F_UNLCK;//UNNLOCK
                        fcntl (fd, F_SETLKW, &lock);//UNLOCK
                        fclose(f);
                        close(fd);
                    }
                    closedir(directory);
                     _exit(0);
                }

                else if(childpid<0)
                {
                    printf("Fork failed\n");
                    break;
                }

                else if(childpid>0)//if it's parent processs
                {  
                  childpid = waitpid(childpid,NULL,0);
                  //rewind(f);
                  fr = open ("151044096sizes.txt", O_RDONLY);//Opens the file for lock, read only format
                  f2 = fopen("151044096sizes.txt", "r+");
                  
                  memset (&lock, 0, sizeof(lock));//LOCK
                  lock.l_type=F_RDLCK;//LOCK
                  fcntl(fr,F_SETLKW,&lock);//LOCK
                  /*while((ch = fgetc(f2)) != EOF)//READ
                      printf("%c", ch);//READ*/
                   while (fgets(buffer, MAXCHAR, f2) != NULL){ ++counter;}
                        printf("%s", buffer);
                  lock.l_type=F_UNLCK;//UNLOCK
                  fcntl(fr,F_SETLKW,&lock);//UNLOCK
                  fclose(f2);
                  close(fr);

                }                
            }
            else if(isdirectory(mycwd)==0)//If it's not directory
            {
                all+=x;
                if (S_ISLNK(st.st_mode) || S_ISBLK(st.st_mode) || S_ISCHR(st.st_mode) || S_ISFIFO(st.st_mode)) //If it's a special file
                {
                  f1 = fopen("151044096sizes.txt", "a+");
                  fprintf(f1," %d           Special File    %s\n",id, mycwd);
                  printf(" %d           Special File    %s\n",id, mycwd);
                  fclose(f1);
                }
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