#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc, char const *argv[])
{
    struct dirent *direntp;  
    struct stat status;
    DIR *directory=NULL;

    memset(&status, 0, sizeof(struct stat));
    directory = opendir(".");
    
    if (directory == NULL)
    {
        fprintf(stderr,"Directory acilamadi veya bulunamadi.\n");
        exit(-1);
    } 

    else
    {  
        printf("Type\tPermissions\tSize\t\tName\n");
        while ((direntp = readdir(directory)) != NULL) 
        {
            if (strcmp(direntp->d_name, ".")!= 0&&strcmp(direntp->d_name, "..")!= 0&&direntp->d_name[strlen(direntp->d_name) - 1] != '~')
            {
                stat(direntp->d_name, &status);
                switch (status.st_mode & S_IFMT) 
                {
                    case S_IFBLK:  printf(" b "); break;
                    case S_IFCHR:  printf(" c "); break; 
                    case S_IFDIR:  printf(" d "); break; 
                    case S_IFIFO:  printf(" p "); break; 
                    case S_IFLNK:  printf(" l "); break; 
                    case S_IFSOCK: printf(" s "); break;
                    default:       printf(" - "); break;
                }
                printf((status.st_mode & S_IRUSR) ? "\t r" : "\t -");
                printf((status.st_mode & S_IWUSR) ? "w" : "-");
                printf((status.st_mode & S_IXUSR) ? "x" : "-");
                printf((status.st_mode & S_IRGRP) ? "r" : "-");
                printf((status.st_mode & S_IWGRP) ? "w" : "-");
                printf((status.st_mode & S_IXGRP) ? "x" : "-");
                printf((status.st_mode & S_IROTH) ? "r" : "-");
                printf((status.st_mode & S_IWOTH) ? "w" : "-");
                printf((status.st_mode & S_IXOTH) ? "x" : "-");
                printf("\t%lld", status.st_size);
                printf("\t\t%s\n", direntp->d_name);
            }
        }
        printf("%c", '\0');
    }
    
    closedir(directory);

    return 0;
}
  