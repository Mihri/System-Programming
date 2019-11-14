#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#define PATH_MAX 255

int isdirectory(char *path);
int sizepathfun (char *path);
int pathfun (char *path1);
int postOrderApply (char *path, int pathfun (char *path1));
int flag;

int main(int argc, char *argv[])
{ 
    if (argc == 2) 
	{
		fprintf(stderr,"Kullanım: %s  FolderName\n", argv[0]);
		postOrderApply (argv[1], 0);
		flag=0;
		exit(1);
	}
	else if (argc == 3) 
	{
		fprintf(stderr,"Kullanım: %s -z FolderName\n", argv[0]);
		postOrderApply (argv[2], 0);
		flag=1;
		exit(1);
	}
    return 0; 
}

/*gonderilen directorydeki file ve directory lere bakip iclerindeki file ve directory lerin buyuklugunu bulan fonksiyon.*/
int postOrderApply (char *find, int pathfun (char *path1))
{
	
    char mycwd[PATH_MAX];
    struct dirent *direntp; /*direntp adını degistir, dirent kütüphanesinden *direntp olusturuyorum*/
    struct stat status; /*status adını değiştir direntten geliyor durumunu gösteriyo,kritik section*/   
    DIR *directory;/*directory değiştir,klasör tutuyor*/
    int all=0;
    int x=0;
    int temp=0;

    	if ((directory = opendir(find))== NULL) /*directory nin NULL olma durumuna bakiyorum*/
		{
       		fprintf(stderr,"Directory acilamadi veya bulunamadi.\n");
       		exit(-1);
		}    
    	while ((direntp = readdir(directory)) != NULL) /*Null degilse directory icini okumaya basliyorum*/
    	{
		/*recursion i ilerletmek icin mycwd yi daha sonra find parametresi yerine gonderecegim. */
             
            if (strcmp(direntp->d_name, ".")!= 0 && strcmp(direntp->d_name, "..")!= 0 && direntp->d_name[strlen(direntp->d_name) - 1] != '~')/*directory olarak . ve .. dosyalari degilse islemleri yapiyorum.*/
            {
            	sprintf(mycwd, "%s/%s", find, direntp->d_name);
		                            

		        //printf("path----->: %s\n",mycwd);

        	    if(isdirectory(mycwd)!=0) /* If directory founds */
                {
                		if(flag==0)
                       	{
                       		x= sizepathfun (mycwd);
                        	temp=postOrderApply (mycwd, pathfun)+x;
                        
                       		//printf("directory found: %s size %d\n",mycwd,temp);
                       		printf("%d  %s\n",temp,mycwd);
                       	}
                        
                       	else if(flag==1)
                       	{
                       		printf("ASSDFFGLHLJLŞSFŞLKSŞLKLŞRGDMGDK"); 
                       		x= sizepathfun (mycwd);
                        	temp=postOrderApply (mycwd, pathfun)+x;
                        
                       		//printf("directory found: %s size %d\n",mycwd,temp);
                        	printf("%d %s\n",temp,mycwd);
                       		all+=temp;
                       		
                       	}

		    	}
		    	else if(isdirectory(mycwd)==0)/*eger directory degilde file sa*/ 
           	    {	
					x= sizepathfun (mycwd);
					all+=x;
					//printf("file found: %s file-size %d\n",mycwd,x);
					printf("%d  %s\n",x,mycwd);
				}  	
		
            }   
    }
    while ((closedir(directory) == -1 )&& (errno == EINTR));/*directory i kapatma islemi.*/

    return all;
}

/*directory mi degil mi kontrol eder.*/
int isdirectory(char *path)
{
    struct stat statbuf;
    if(stat(path,&statbuf)==-1)
        return 0;
    else
        return S_ISDIR(statbuf.st_mode);
}

/*file in boyutu bulur.*/
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