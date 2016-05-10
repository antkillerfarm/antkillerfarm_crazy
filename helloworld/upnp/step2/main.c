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

typedef struct {
	char ServiceId[NAME_SIZE];
	char ServiceType[NAME_SIZE];
	char *VariableStrVal[UP_MAXVARS];
	char EventURL[NAME_SIZE];
	char ControlURL[NAME_SIZE];
	char SID[NAME_SIZE];
}Up_service;

typedef struct {
	char UDN[250];
	char DescDocURL[250];
	char FriendlyName[250];
	char PresURL[250];
	int  AdvrTimeOut;
        Up_service UpService[UP_SERVICE_SERVCOUNT];
}UpDevice;

struct UpDeviceNode{
        UpDevice device;
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

typedef struct {
	Upnp_EventType EventType;
        char EventTypeString[64];
	int (*event_handler)(Upnp_EventType EventType, void *Event, void *Cookie);
}EventTypeData;

#define EVENT_TYPE_DATA_MACRO(x) x, #x

int upnp_discovery_search_result_handler(Upnp_EventType EventType, void *Event, void *Cookie)
{
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
}

EventTypeData event_type_data[] =
{
	/* Discovery */
	{EVENT_TYPE_DATA_MACRO(UPNP_DISCOVERY_ADVERTISEMENT_ALIVE), upnp_discovery_search_result_handler},
	{EVENT_TYPE_DATA_MACRO(UPNP_DISCOVERY_ADVERTISEMENT_BYEBYE), NULL},
	{EVENT_TYPE_DATA_MACRO(UPNP_DISCOVERY_SEARCH_RESULT), upnp_discovery_search_result_handler},
	{EVENT_TYPE_DATA_MACRO(UPNP_DISCOVERY_SEARCH_TIMEOUT), NULL},

	/* SOAP */
	{EVENT_TYPE_DATA_MACRO(UPNP_CONTROL_ACTION_REQUEST), NULL},
	{EVENT_TYPE_DATA_MACRO(UPNP_CONTROL_ACTION_COMPLETE), NULL},
	{EVENT_TYPE_DATA_MACRO(UPNP_CONTROL_GET_VAR_REQUEST), NULL},
	{EVENT_TYPE_DATA_MACRO(UPNP_CONTROL_GET_VAR_COMPLETE), NULL},

	/* GENA */
	{EVENT_TYPE_DATA_MACRO(UPNP_EVENT_SUBSCRIPTION_REQUEST), NULL},
	{EVENT_TYPE_DATA_MACRO(UPNP_EVENT_RECEIVED), NULL},
	{EVENT_TYPE_DATA_MACRO(UPNP_EVENT_RENEWAL_COMPLETE), NULL},
	{EVENT_TYPE_DATA_MACRO(UPNP_EVENT_SUBSCRIBE_COMPLETE), NULL},
	{EVENT_TYPE_DATA_MACRO(UPNP_EVENT_UNSUBSCRIBE_COMPLETE), NULL},
	{EVENT_TYPE_DATA_MACRO(UPNP_EVENT_AUTORENEWAL_FAILED), NULL},
	{EVENT_TYPE_DATA_MACRO(UPNP_EVENT_SUBSCRIPTION_EXPIRED), NULL}
};

int event_type_data_num = sizeof(event_type_data)/sizeof(EventTypeData);

int CtrlPointCallbackEventHandler(Upnp_EventType EventType, void *Event, void *Cookie)
{
	/*int errCode = 0;*/

	int i;
	for (i = 0; i < event_type_data_num; i++)
	{
		if (event_type_data[i].EventType == EventType)
		{
			g_print("%s\n", event_type_data[i].EventTypeString);
			if (event_type_data[i].event_handler != NULL)
			{
				event_type_data[i].event_handler(EventType, Event, Cookie);
			}
			return 0;
		}
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

	//g_print("%d %s\n", event_type_data_num, event_type_data[0].EventTypeString);
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
