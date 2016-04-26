#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{ 
  FILE *fp = NULL;
  int i = 20;

  if (access("file_lock.test", F_OK) != 0) 
    {
      creat("file_lock.test", 0777);
    }

  if ((fp = fopen("file_lock.test", "r+b")) == NULL)
    printf("file open error!\n");

  if (flock(fp->_fileno, LOCK_EX) != 0) 
    printf("file lock by others\n");

  while(1) 
    { 
      printf("%d\n", i--);
      sleep(1);
      if (i == 0)
	break;
    } 

  fclose(fp);
  flock(fp->_fileno, LOCK_UN); 

  return 0;
}
