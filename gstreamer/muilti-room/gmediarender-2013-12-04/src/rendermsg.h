#ifndef _MSG_H_
#define _MSG_H_
#define RMSG
#define RMSGID 1234
#define MAX_TEXT 2048
struct msg_st
{
	long int msg_type;
	char text[MAX_TEXT];
};

int initMsgRcv(void);

#endif   //_MSG_H_
