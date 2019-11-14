#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h> 
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#define SIZE 10000 

int main(int argc, char const *argv[])
{
    int i=0;
    int counter=0;
    char *x=NULL;
    char line[SIZE]="";
    char *token=NULL;
    char str[10][SIZE];
    char path[SIZE]="";
    char temp[SIZE]="";
    char CurDir[SIZE]="";    
    char *ch=NULL;
    int a=0;
    pid_t childPid=-1;
    pid_t childPid2=-1;
    int flag=1;
    int pipe_fds[2];
    int fd=-1;

    x=getwd(CurDir);
    while(1)
    {
        counter=0;
        fgets(line, SIZE , stdin);
        token=strtok(line," ");
        
        while(token!=NULL)
        {
            strcpy(str[counter],token);
            ++counter;
            token=strtok(NULL," ");
        }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if(counter==1)
        {
            a=strlen(str[0]);
            str[0][a-1]='\0';
            if(!strcmp(str[0],"exit"))
            {
                printf("Exiting...\n");
                return 0;
            }
            else if(!strcmp(str[0],"help"))
            {
                printf("GTUShell Commands\n");
                printf("cd : Change directory \n");
                printf("pwd : which will print the path of present working directory. \n");
                printf("lsf : which will list file type (R for regular file, S for non-regular(special) file), access rights (int the form of rwxr-xr-x, just like actual lsf), file size(bytes) and file name of all files (not directories) in the present working directory. \n");
                printf("wc: which will print on standard output the number of lines in the file provided to it as argument or the string coming from standard input until EOF character \n");
                printf("cat : which will print on standard output the contents of the file provided to it as argument or from standard input.\n");
                printf("buNeDu : show all directories with path and sizes\n");
                printf("help : will help all of these \n");
                printf("exit : exits from GTUShell\n");
                
                printf("> : directional right = into file \n");
                printf("< : directional left = will get from file \n");
                printf("| : Pipe operation\n");

                flag =0;
            }
            
            str[0][a-1]='\n';
            str[0][a]='\0';
        }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        else if(counter==2)
        {
            if (!strcmp(str[0],"cd"))
            {
                a=strlen(str[1]);
                str[1][a-1]='\0';
                chdir(str[1]);
                flag=0;

            }        
        }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        else if (counter==3)
        {
            if (!strcmp(str[1],">")&&!strcmp(str[0],"pwd"))
            {
                a=strlen(str[2]);
                str[2][a-1]='\0';
                fd=open(str[2],O_RDWR|O_CREAT|O_TRUNC,0666);
                ch = getwd(temp);
                write(fd,temp,strlen(temp));
                flag=0;         
            }
        }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


        if(flag)
        {
            childPid=fork();
            if(!childPid)
            {
                if(counter==1)
                {
                    a=strlen(str[0]);
                    str[0][a-1]='\0';
                    if(!strcmp(str[0],"lsf"))
                    {
                        sprintf(path,"%s/%s",CurDir,str[0]);

                        if (execl(path,str[0],(char *) NULL) == -1) 
                        {
                            fprintf(stderr, "Couldn't exec!\n");
                            exit(1);
                        }
                    }
                    if(!strcmp(str[0],"pwd"))
                    {
                        sprintf(path,"%s/%s",CurDir,str[0]);

                        if (execl(path,str[0],(char *) NULL) == -1) 
                        {
                            fprintf(stderr, "Couldn't exec!\n");
                            exit(1);
                        }
                    }

                    else
                    {
                        fprintf(stderr, "Wrong arguments!\nPlease enter proper arguments!\n");
                        exit(1);
                    }                   
                }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                else if (counter==2)
                {
                    a=strlen(str[1]);
                    str[1][a-1]='\0';
                    if(!strcmp(str[0],"cat"))
                    {
                        sprintf(path,"%s/%s",CurDir,str[0]);
                        if (execl(path,str[0],str[1],(char *) NULL) == -1) 
                        {
                            fprintf(stderr, "Couldn't exec!\n");
                            exit(1);
                        }
                    
                    }
                    if(!strcmp(str[0],"wc"))
                    {
                        sprintf(path,"%s/%s",CurDir,str[0]);
                        if (execl(path,str[0],str[1],(char *) NULL) == -1) 
                        {
                            fprintf(stderr, "Couldn't exec!\n");
                            exit(1);
                        }
                    
                    }
                    if(!strcmp(str[0],"buNeDu"))
                    {
                        sprintf(path,"%s/%s",CurDir,str[0]);
                        if (execl(path,str[0],str[1],(char *) NULL) == -1) 
                        {
                            fprintf(stderr, "Couldn't exec!\n");
                            exit(1);
                        }
                    
                    }
                    else
                    {
                        fprintf(stderr, "Wrong arguments!\nPlease enter proper arguments!\n");
                        exit(1);
                    }

                }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                
                else if(counter==3)
                {
                    
                    a=strlen(str[2]);
                    str[2][a-1]='\0';


                    if(!strcmp(str[0],"lsf") && !strcmp(str[1],"|") && !strcmp(str[2],"wc"))
                    {

                        pipe(pipe_fds);
                        childPid2=fork();

                        if(!childPid2)
                        {
                            close(pipe_fds[0]);
                            dup2(pipe_fds[1],STDOUT_FILENO);
                            sprintf(path,"%s/%s",CurDir,str[0]);
                            if (execl(path,str[0],(char *) NULL) == -1) 
                            {
                                fprintf(stderr, "Couldn't exec!\n");
                                exit(1);
                            }
                            close(pipe_fds[1]);
                            exit(1);
                        }
                        else
                        {
                            wait(NULL);
                            close(pipe_fds[1]);
                            dup2(pipe_fds[0],STDIN_FILENO);
                            sprintf(path,"%s/%s",CurDir,str[2]);
                            if (execl(path,str[2],(char *) NULL) == -1) 
                            {
                                fprintf(stderr, "Couldn't exec!\n");
                                exit(1);
                            }
                            close(pipe_fds[0]);
                            exit(1);

                        }
                    }

                    else if(!strcmp(str[0],"lsf") && !strcmp(str[1],"|") && !strcmp(str[2],"cat"))
                    {

                        pipe(pipe_fds);
                        childPid2=fork();

                        if(!childPid2)
                        {
                            close(pipe_fds[0]);
                            dup2(pipe_fds[1],STDOUT_FILENO);
                            sprintf(path,"%s/%s",CurDir,str[0]);
                            if (execl(path,str[0],(char *) NULL) == -1) 
                            {
                                fprintf(stderr, "Couldn't exec!\n");
                                exit(1);
                            }
                            close(pipe_fds[1]);
                            exit(1);
                        }
                        else
                        {
                            wait(NULL);
                            close(pipe_fds[1]);
                            dup2(pipe_fds[0],STDIN_FILENO);
                            sprintf(path,"%s/%s",CurDir,str[2]);
                            if (execl(path,str[2],(char *) NULL) == -1) 
                            {
                                fprintf(stderr, "Couldn't exec!\n");
                                exit(1);
                            }
                            close(pipe_fds[0]);
                            exit(1);

                        }
                    }
                    else if(!strcmp(str[0],"lsf") && !strcmp(str[1],">"))
                    {
                        
                        fd=open(str[2],O_RDWR|O_CREAT|O_TRUNC,0666);                    
                        dup2(fd,STDOUT_FILENO);                        
                        sprintf(path,"%s/%s",CurDir,str[0]);                       
                        if (execl(path,str[0],(char *) NULL) == -1) 
                        {
                            fprintf(stderr, "Couldn't exec!\n");
                            exit(1);
                        }
                        exit(1);    
                    }

                    else if(!strcmp(str[0],"buNeDu") && !strcmp(str[1],"-z"))
                    {

                        pipe(pipe_fds);
                        childPid2=fork();

                        if(!childPid2)
                        {
                            close(pipe_fds[0]);
                            dup2(pipe_fds[1],STDOUT_FILENO);
                            sprintf(path,"%s/%s",CurDir,str[0]);
                            if (execl(path,str[0],(char *) NULL) == -1) 
                            {
                                fprintf(stderr, "Couldn't exec!\n");
                                exit(1);
                            }
                            close(pipe_fds[1]);
                            exit(1);
                        }
                        else
                        {
                            wait(NULL);
                            close(pipe_fds[1]);
                            dup2(pipe_fds[0],STDIN_FILENO);
                            sprintf(path,"%s/%s",CurDir,str[1]);
                            if (execl(path,str[1],(char *) NULL) == -1) 
                            {
                                fprintf(stderr, "Couldn't exec!\n");
                                exit(1);
                            }
                            close(pipe_fds[0]);
                            exit(1);

                        }
                    }


                }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                else if(counter==4)
                {
                    
                    a=strlen(str[3]);
                    str[3][a-1]='\0';                    

                    if(!strcmp(str[0],"cat") && !strcmp(str[2],"|") && !strcmp(str[3],"wc"))
                    {

                        pipe(pipe_fds);
                        childPid2=fork();

                        if(!childPid2)
                        {
                            close(pipe_fds[0]);
                            dup2(pipe_fds[1],STDOUT_FILENO);
                            sprintf(path,"%s/%s",CurDir,str[0]);
                            if (execl(path,str[0],str[1],(char *) NULL) == -1) 
                            {
                                fprintf(stderr, "Couldn't exec!\n");
                                exit(1);
                            }
                            close(pipe_fds[1]);
                            exit(1);
                        }
                        else
                        {
                            wait(NULL);
                            close(pipe_fds[1]);
                            dup2(pipe_fds[0],STDIN_FILENO);
                            sprintf(path,"%s/%s",CurDir,str[3]);
                            if (execl(path,str[3],(char *) NULL) == -1)
                            {
                                fprintf(stderr, "Couldn't exec!\n");
                                exit(1);
                            }
                            close(pipe_fds[0]);
                            exit(1);

                        }
                    }

                    else if(!strcmp(str[0],"cat") && !strcmp(str[2],">"))
                    {
                        fd=open(str[3],O_RDWR|O_CREAT|O_TRUNC,0666);                    
                        dup2(fd,STDOUT_FILENO);                        
                        sprintf(path,"%s/%s",CurDir,str[0]);
                        
                        if (execl(path,str[0],str[1],(char *) NULL) == -1) 
                        {
                            fprintf(stderr, "Couldn't exec!\n");
                            exit(1);
                        }                       
                        exit(1);                            
                    }
                }
                exit(1);
                
            }
        }

        wait(NULL);
        flag=1;
    }

    return 0;
}