#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>


int main(int argc, char const *argv[])
{
   int fileDescriptor=-1;
   char character;

   if (argc == 2)
   {   
      fileDescriptor=open(argv[1],O_RDONLY);
      if(fileDescriptor==-1)
      {
          fprintf(stderr, "File couldn't opened: %s\n", argv[1]);
          exit(1);
      }
   }
   else
   {
      fileDescriptor=STDIN_FILENO;
   }

   while( read(fileDescriptor,&character,sizeof(char))>0)
   {
      printf("%c",character);
   }

   printf("\n");
   close (fileDescriptor);

   return 0;
}