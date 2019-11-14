
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>


int main(int argc, char const *argv[])
{
   char current[2048];

   if (argc == 1)
   {   
      if (getcwd(current, sizeof(current)) != NULL)
            fprintf(stdout, "%s\n", current);
        else
            fprintf(stderr, "getcwd -> error\n");
   }
   
   return 0;
}