#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#define PATH_MAX 255
#define MAXCHAR 1000
static int flag;
int isdirectory(char *path);
int sizepathfun (char *path);
int pathfun (char *path1);
int postOrderApply (char *path, int pathfun (char *path1));


int main(int argc, char *argv[])
{
    int a;
                      
    if (argc == 2)//without -z format
    {
        flag=0;
        //fprintf(stderr,"Usage: %s  FolderName\n", argv[0]);
        a = postOrderApply (argv[1], 0);
        
        printf(" %d\t%s\n", a, argv[1]); 
        exit(1);
    }
     if (argc == 3)//with -z format
    {
        flag=1;
        //fprintf(stderr,"Usage: %s -z FolderName\n", argv[0]);
        a = postOrderApply (argv[2], 0);
        printf(" %d\t%s\n",a, argv[2]);
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
                           
                        if(flag==0)//if the input without -z format
                        {
                            temp=postOrderApply (mycwd, pathfun);                      
                            printf(" %d\t\t%s\n",temp,mycwd);//WRITE                       
                        }

                        else if(flag==1)//if the input with -z format
                        {
                            temp=postOrderApply (mycwd, pathfun);
                            all+=temp;
                            printf(" %d\t\t%s\n",temp,mycwd);//WRITE
                        }
                        //closedir(directory);
                         //_exit(0);
                    
                }
                
                else if(isdirectory(mycwd)==0)//If it's not directory
                {
                    all+=x;
                    if (S_ISLNK(st.st_mode) || S_ISBLK(st.st_mode) || S_ISCHR(st.st_mode) || S_ISFIFO(st.st_mode)) //If it's a special file
                    {
                      printf(" Special File\t%s\n",mycwd);
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