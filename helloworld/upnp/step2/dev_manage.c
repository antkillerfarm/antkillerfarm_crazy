#include "ctrl_point.h"

/*! Device type for add devices. */
const char CPDeviceType[] = "urn:schemas-upnp-org:device:MediaRenderer:1";

void CtrlPointAddDevice(
	IXML_Document *DescDoc,
	const char *location,
	int expires)
{
	char *deviceType = NULL;
	char *friendlyName = NULL;
	char presURL[200];
	char *baseURL = NULL;
	char *relURL = NULL;
	char *UDN = NULL;

	struct UpDeviceNode *tmpdevnode;
	int ret = 1;
	int found = 0;
	int service;

	ithread_mutex_lock(&DeviceListMutex);

	UDN = Util_GetFirstDocumentItem(DescDoc, "UDN");
	g_print("UDN: %s\n", UDN);
	deviceType = Util_GetFirstDocumentItem(DescDoc, "deviceType");
	g_print("deviceType: %s\n", deviceType);
	friendlyName = Util_GetFirstDocumentItem(DescDoc, "friendlyName");
	g_print("friendlyName: %s\n", friendlyName);
	baseURL = Util_GetFirstDocumentItem(DescDoc, "URLBase");
	g_print("baseURL: %s\n", baseURL);
	relURL = Util_GetFirstDocumentItem(DescDoc, "presentationURL");
	g_print("relURL: %s\n", relURL);
	ret = UpnpResolveURL((baseURL ? baseURL : location), relURL, presURL);
	
	if (UPNP_E_SUCCESS != ret)
	{
	        g_print("Error generating presURL from %s + %s\n",
			baseURL, relURL);
	}

	if (strcmp(deviceType, CPDeviceType) == 0)
	{
		/* Check if this device is already in the list */
		tmpdevnode = GlobalDeviceList;
		while (tmpdevnode) {
			if (strcmp(tmpdevnode->device.UDN, UDN) == 0) {
				found = 1;
				break;
			}
			tmpdevnode = tmpdevnode->next;
		}

		if (found)
		{
			/* The device is already there, so just update  */
			/* the advertisement timeout field */
			tmpdevnode->device.AdvrTimeOut = expires;
		}
		else
		{
		        Util_ListService(DescDoc);
		}
	}
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
