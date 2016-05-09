#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>
#include "ithread.h"
#include "upnp.h"

#define CP_SUCCESS		0
#define CP_ERROR		(-1)
#define CP_WARNING		1

#define UP_SERVICE_SERVCOUNT  2
#define UP_MAXVARS 4

struct Up_service {
    char ServiceId[NAME_SIZE];
    char ServiceType[NAME_SIZE];
    char *VariableStrVal[UP_MAXVARS];
    char EventURL[NAME_SIZE];
    char ControlURL[NAME_SIZE];
    char SID[NAME_SIZE];
};

struct UpDevice {
    char UDN[250];
    char DescDocURL[250];
    char FriendlyName[250];
    char PresURL[250];
    int  AdvrTimeOut;
    struct Up_service UpService[UP_SERVICE_SERVCOUNT];
};

struct UpDeviceNode {
    struct UpDevice device;
    struct UpDeviceNode *next;
};

ithread_mutex_t DeviceListMutex;

UpnpClient_Handle ctrlpt_handle = -1;

struct UpDeviceNode *GlobalDeviceList = NULL;

/*! Device type for tv device. */
const char UpDeviceType[] = "urn:schemas-upnp-org:device:MediaRenderer:1";

char *Util_GetFirstDocumentItem(IXML_Document *doc, const char *item)
{
	IXML_NodeList *nodeList = NULL;
	IXML_Node *textNode = NULL;
	IXML_Node *tmpNode = NULL;
	char *ret = NULL;

	nodeList = ixmlDocument_getElementsByTagName(doc, (char *)item);
	if (nodeList) {
		tmpNode = ixmlNodeList_item(nodeList, 0);
		if (tmpNode) {
			textNode = ixmlNode_getFirstChild(tmpNode);
			if (!textNode) {
				g_print("%s(%d): (BUG) ixmlNode_getFirstChild(tmpNode) returned NULL\n",
					__FILE__, __LINE__); 
				ret = strdup("");
				goto epilogue;
			}
			ret = strdup(ixmlNode_getNodeValue(textNode));
			if (!ret) {
				g_print("%s(%d): ixmlNode_getNodeValue returned NULL\n",
					__FILE__, __LINE__); 
				ret = strdup("");
			}
		} else
			g_print("%s(%d): ixmlNodeList_item(nodeList, 0) returned NULL\n",
				__FILE__, __LINE__);
	} else
		g_print("%s(%d): Error finding %s in XML Node\n",
			__FILE__, __LINE__, item);

epilogue:
	if (nodeList)
		ixmlNodeList_free(nodeList);

	return ret;
}

void Util_PrintEventType(Upnp_EventType S)
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

void CtrlPointAddDevice(
	IXML_Document *DescDoc,
	const char *location,
	int expires)
{
	char *deviceType = NULL;
	char *friendlyName = NULL;
	char *baseURL = NULL;
	//char *relURL = NULL;
	char *UDN = NULL;

	ithread_mutex_lock(&DeviceListMutex);
	g_print("%s\n", __FUNCTION__);
	UDN = Util_GetFirstDocumentItem(DescDoc, "UDN");
	g_print("UDN: %s\n", UDN);
	deviceType = Util_GetFirstDocumentItem(DescDoc, "deviceType");
	g_print("deviceType: %s\n", deviceType);
	friendlyName = Util_GetFirstDocumentItem(DescDoc, "friendlyName");
	g_print("friendlyName: %s\n", friendlyName);
	baseURL = Util_GetFirstDocumentItem(DescDoc, "URLBase");
	g_print("baseURL: %s\n", baseURL);
	//relURL = Util_GetFirstDocumentItem(DescDoc, "presentationURL");
	//g_print("relURL: %s\n", relURL);
	ithread_mutex_unlock(&DeviceListMutex);
}

int CtrlPointRemoveAll(void)
{
	struct UpDeviceNode *curdevnode, *next;

	ithread_mutex_lock(&DeviceListMutex);

	curdevnode = GlobalDeviceList;
	GlobalDeviceList = NULL;

	while (curdevnode) {
		next = curdevnode->next;
		//TvCtrlPointDeleteNode(curdevnode);
		curdevnode = next;
	}

	ithread_mutex_unlock(&DeviceListMutex);

	return CP_SUCCESS;
}


int CtrlPointRefresh(void)
{
	int rc;

	CtrlPointRemoveAll();
	/* Search for all devices of type tvdevice version 1,
	 * waiting for up to 5 seconds for the response */
	rc = UpnpSearchAsync(ctrlpt_handle, 5, UpDeviceType, NULL);
	if (UPNP_E_SUCCESS != rc) {
	        g_print("Error sending search request%d\n", rc);

		return CP_ERROR;
	}

	return CP_SUCCESS;
}

int CtrlPointCallbackEventHandler(Upnp_EventType EventType, void *Event, void *Cookie)
{
	/*int errCode = 0;*/

        Util_PrintEventType(EventType);
	switch ( EventType ) {
	/* SSDP Stuff */
	case UPNP_DISCOVERY_ADVERTISEMENT_ALIVE:
	case UPNP_DISCOVERY_SEARCH_RESULT: {
		struct Upnp_Discovery *d_event = (struct Upnp_Discovery *)Event;
		IXML_Document *DescDoc = NULL;
		int ret;

		if (d_event->ErrCode != UPNP_E_SUCCESS) {
		        g_print("Error in Discovery Callback -- %d\n",
				d_event->ErrCode);
		}
		ret = UpnpDownloadXmlDoc(d_event->Location, &DescDoc);
		if (ret != UPNP_E_SUCCESS) {
		        g_print("Error obtaining device description from %s -- error = %d\n",
				d_event->Location, ret);
		} else {
			CtrlPointAddDevice(
				DescDoc, d_event->Location, d_event->Expires);
		}
		if (DescDoc) {
			ixmlDocument_free(DescDoc);
		}
		//TvCtrlPointPrintList();
		break;
	}
	case UPNP_DISCOVERY_SEARCH_TIMEOUT:
		/* Nothing to do here... */
		break;
	case UPNP_DISCOVERY_ADVERTISEMENT_BYEBYE: {

		break;
	}
	/* SOAP Stuff */
	case UPNP_CONTROL_ACTION_COMPLETE: {

		break;
	}
	case UPNP_CONTROL_GET_VAR_COMPLETE: {

		break;
	}
	/* GENA Stuff */
	case UPNP_EVENT_RECEIVED: {

		break;
	}
	case UPNP_EVENT_SUBSCRIBE_COMPLETE:
	case UPNP_EVENT_UNSUBSCRIBE_COMPLETE:
	case UPNP_EVENT_RENEWAL_COMPLETE: {

	}
	case UPNP_EVENT_AUTORENEWAL_FAILED:
	case UPNP_EVENT_SUBSCRIPTION_EXPIRED: {

		break;
	}
	/* ignore these cases, since this is not a device */
	case UPNP_EVENT_SUBSCRIPTION_REQUEST:
	case UPNP_CONTROL_GET_VAR_REQUEST:
	case UPNP_CONTROL_ACTION_REQUEST:
		break;
	}

	return 0;
}

int CtrlPointStart()
{
	//ithread_t timer_thread;
	int rc;
	unsigned short port = 0;
	char *ip_address = NULL;

	ithread_mutex_init(&DeviceListMutex, 0);

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

	CtrlPointRefresh();

	/* start a timer thread */
	//ithread_create(&timer_thread, NULL, TvCtrlPointTimerLoop, NULL);
	//ithread_detach(timer_thread);

	return CP_SUCCESS;
}

int CtrlPointStop(void)
{
	CtrlPointRemoveAll();
	UpnpUnRegisterClient( ctrlpt_handle );
	UpnpFinish();

	return CP_SUCCESS;
}

int main(int argc, char **argv)
{
	int rc;
	ithread_t cmdloop_thread;

	int sig;
	sigset_t sigs_to_catch;

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
