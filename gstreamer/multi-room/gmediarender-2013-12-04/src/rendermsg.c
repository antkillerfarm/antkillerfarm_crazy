#include <stdio.h>
#include <sys/msg.h>
#include <upnp/ithread.h>
#include <errno.h>

#include "output.h"
#include "upnp_transport.h"

#include "rendermsg.h"

static void *rdmsgrcv(void *userdata)
{

	int msgid = -1;
	struct msg_st data;
	long int msgtype = 0;
	msgid = msgget((key_t)RMSGID, 0666 | IPC_CREAT);
	if(msgid == -1){  
		printf("msgget failed with error\n");
		return 0;
 	}
 	
 	fprintf(stderr, "msgrcv begin msgid = %d!\n", msgid);
	while(1){
		if(msgrcv(msgid, (void*)&data, MAX_TEXT, msgtype, 0) != -1) {
		//	fprintf(stderr, "received from msg queue: %s\n", data.text);
			timerPlay(data.text);
		}
	}
	fprintf(stderr, "errno = %d\n", errno);
}

int initMsgRcv(void)
{
	fprintf(stderr, "Msg Rcv processing start!\n");
	int rc;
	pthread_t pid;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, ITHREAD_CREATE_DETACHED);
	rc = pthread_create(&pid, &attr, rdmsgrcv, NULL);
	ithread_attr_destroy(&attr);
	return rc;
}

