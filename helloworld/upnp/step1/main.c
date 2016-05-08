#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>
#include "ithread.h"
#include "upnp.h"

#define CP_SUCCESS		0
#define CP_ERROR		(-1)
#define CP_WARNING		1

UpnpClient_Handle ctrlpt_handle = -1;

void PrintEventType(Upnp_EventType S)
{
	switch (S) {
	/* Discovery */
	case UPNP_DISCOVERY_ADVERTISEMENT_ALIVE:
		g_print("UPNP_DISCOVERY_ADVERTISEMENT_ALIVE\n");
		break;
	case UPNP_DISCOVERY_ADVERTISEMENT_BYEBYE:
		g_print("UPNP_DISCOVERY_ADVERTISEMENT_BYEBYE\n");
		break;
	case UPNP_DISCOVERY_SEARCH_RESULT:
		g_print( "UPNP_DISCOVERY_SEARCH_RESULT\n");
		break;
	case UPNP_DISCOVERY_SEARCH_TIMEOUT:
		g_print( "UPNP_DISCOVERY_SEARCH_TIMEOUT\n");
		break;
	/* SOAP */
	case UPNP_CONTROL_ACTION_REQUEST:
		g_print("UPNP_CONTROL_ACTION_REQUEST\n");
		break;
	case UPNP_CONTROL_ACTION_COMPLETE:
		g_print("UPNP_CONTROL_ACTION_COMPLETE\n");
		break;
	case UPNP_CONTROL_GET_VAR_REQUEST:
		g_print("UPNP_CONTROL_GET_VAR_REQUEST\n");
		break;
	case UPNP_CONTROL_GET_VAR_COMPLETE:
		g_print("UPNP_CONTROL_GET_VAR_COMPLETE\n");
		break;
	/* GENA */
	case UPNP_EVENT_SUBSCRIPTION_REQUEST:
		g_print("UPNP_EVENT_SUBSCRIPTION_REQUEST\n");
		break;
	case UPNP_EVENT_RECEIVED:
		g_print("UPNP_EVENT_RECEIVED\n");
		break;
	case UPNP_EVENT_RENEWAL_COMPLETE:
		g_print("UPNP_EVENT_RENEWAL_COMPLETE\n");
		break;
	case UPNP_EVENT_SUBSCRIBE_COMPLETE:
		g_print("UPNP_EVENT_SUBSCRIBE_COMPLETE\n");
		break;
	case UPNP_EVENT_UNSUBSCRIBE_COMPLETE:
		g_print("UPNP_EVENT_UNSUBSCRIBE_COMPLETE\n");
		break;
	case UPNP_EVENT_AUTORENEWAL_FAILED:
		g_print("UPNP_EVENT_AUTORENEWAL_FAILED\n");
		break;
	case UPNP_EVENT_SUBSCRIPTION_EXPIRED:
		g_print("UPNP_EVENT_SUBSCRIPTION_EXPIRED\n");
		break;
	}
}


int CtrlPointCallbackEventHandler(Upnp_EventType EventType, void *Event, void *Cookie)
{
	/*int errCode = 0;*/

        PrintEventType(EventType);
	return 0;
}

int CtrlPointStart()
{
	//ithread_t timer_thread;
	int rc;
	unsigned short port = 0;
	char *ip_address = NULL;

	//ithread_mutex_init(&DeviceListMutex, 0);

        g_print("Initializing UPnP Sdk with\n"
			 "\tipaddress = %s port = %u\n",
			 ip_address ? ip_address : "{NULL}", port);

	rc = UpnpInit(ip_address, port);
	if (rc != UPNP_E_SUCCESS) {
		g_print("WinCEStart: UpnpInit() Error: %d\n", rc);
		UpnpFinish();

		return CP_ERROR;
	}
	if (!ip_address) {
		ip_address = UpnpGetServerIpAddress();
	}
	if (!port) {
		port = UpnpGetServerPort();
	}

        g_print("UPnP Initialized\n"
			 "\tipaddress = %s port = %u\n",
			 ip_address ? ip_address : "{NULL}", port);
        g_print("Registering Control Point\n");
	rc = UpnpRegisterClient(CtrlPointCallbackEventHandler,
				&ctrlpt_handle, &ctrlpt_handle);
	if (rc != UPNP_E_SUCCESS) {
	        g_print("Error registering CP: %d\n", rc);
		UpnpFinish();

		return CP_ERROR;
	}

        g_print("Control Point Registered\n");

	//TvCtrlPointRefresh();

	/* start a timer thread */
	//ithread_create(&timer_thread, NULL, TvCtrlPointTimerLoop, NULL);
	//ithread_detach(timer_thread);

	return CP_SUCCESS;
}

int CtrlPointStop(void)
{
	//CtrlPointRemoveAll();
	UpnpUnRegisterClient( ctrlpt_handle );
	UpnpFinish();

	return CP_SUCCESS;
}

int main(int argc, char **argv)
{
	int rc;
	ithread_t cmdloop_thread;
#ifdef WIN32
#else
	int sig;
	sigset_t sigs_to_catch;
#endif
	int code;

	rc = CtrlPointStart();
	if (rc != CP_SUCCESS) {
	        g_print("Error starting UPnP Control Point\n");
		return rc;
	}

	/* Catch Ctrl-C and properly shutdown */
	sigemptyset(&sigs_to_catch);
	sigaddset(&sigs_to_catch, SIGINT);
	sigwait(&sigs_to_catch, &sig);
        g_print("Shutting down on signal %d...\n", sig);
	rc = CtrlPointStop();

	return rc;
}
