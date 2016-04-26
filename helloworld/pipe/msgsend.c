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

  msgid = msgget((key_t)1234, 0666 | IPC_CREAT);
  if(msgid == -1)
    {  
      printf("msgget failed with error\n");
      return 0;
    }

  data.msg_type = 1;
  sprintf(data.text, "this is test data for the msg queue example\n");

  if(msgsnd(msgid, (void*)&data, MAX_TEXT, 0) == -1)  
    {
      printf("msgsnd failed\n");
    }

  return 0;
}

