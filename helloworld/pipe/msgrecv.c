#include <stdio.h>
#include <sys/msg.h>

#define MAX_TEXT 128
struct msg_st  
{  
  long int msg_type;  
  char text[MAX_TEXT];  
};

int main(int argc, char *argv[])
{
  int msgid = -1;
  struct msg_st data;
  long int msgtype = 0;

  msgid = msgget((key_t)1234, 0666 | IPC_CREAT);
  if(msgid == -1)
    {  
      printf("msgget failed with error\n");
      return 0;
    }

  //if(msgrcv(msgid, (void*)&data, MAX_TEXT, msgtype, 0) == -1) // blocking mode
  if(msgrcv(msgid, (void*)&data, MAX_TEXT, msgtype, IPC_NOWAIT) == -1)//non-blocking mode 
    {
      printf("msgrcv failed\n");
      return 0;
    }

  printf("received from msg queue: %s\n", data.text);
  return 0;
}
