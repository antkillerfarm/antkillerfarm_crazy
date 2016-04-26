#include <stdio.h>
#include <unistd.h>

void main()
{ 
  FILE * in_file;
  int count = 1;
  char buf[80];
  mkfifo("mypipe", 0777);
  in_file = fopen("mypipe", "r"); 
  if (in_file == NULL) { 
    printf("Error in fdopen.\n");
    return;
  } 
  while ((count = fread(buf, 1, 80, in_file)) > 0) 
    printf("received from pipe: %s\n", buf); 
  fclose(in_file); 
  remove("mypipe");
}
