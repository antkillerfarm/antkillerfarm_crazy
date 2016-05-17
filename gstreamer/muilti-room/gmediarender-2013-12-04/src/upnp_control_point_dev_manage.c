#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include <upnp/upnp.h>
#include <upnp/ithread.h>
#include <upnp/upnptools.h>

#include "upnp_control_point.h"


/*! Device type for add devices. */
const int ctrl_point_service_type_len = 3;
const char *ctrl_point_service_type[] =
{
	"urn:schemas-upnp-org:service:AVTransport:1",
	"urn:schemas-upnp-org:service:ConnectionManager:1",
	"urn:schemas-upnp-org:service:RenderingControl:1"
};

void ctrl_point_add_device(
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
	//int service;

	ithread_mutex_lock(&DeviceListMutex);

	UDN = util_get_first_document_item(DescDoc, "UDN");
	g_print("UDN: %s\n", UDN);
	
	deviceType = util_get_first_document_item(DescDoc, "deviceType");
	g_print("deviceType: %s\n", deviceType);
	
	friendlyName = util_get_first_document_item(DescDoc, "friendlyName");
	g_print("friendlyName: %s\n", friendlyName);
	baseURL = util_get_first_document_item(DescDoc, "URLBase");
	g_print("baseURL: %s\n", baseURL);
	relURL = util_get_first_document_item(DescDoc, "presentationURL");
	g_print("relURL: %s\n", relURL);
	ret = UpnpResolveURL((baseURL ? baseURL : location), relURL, presURL);
	
	if (UPNP_E_SUCCESS != ret)
	{
	        g_print("Error generating presURL from %s + %s\n",
			baseURL, relURL);
	}

	if (strcmp(deviceType, upnp_device_type) == 0)
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
		        ControlPointFilter service_type_filter;
			service_type_filter.filter = ctrl_point_service_type;
			service_type_filter.filter_len = ctrl_point_service_type_len;
		        util_list_service_list(DescDoc, &service_type_filter);
		}
	}
	ithread_mutex_unlock(&DeviceListMutex);

	if (deviceType)
		free(deviceType);
	if (friendlyName)
		free(friendlyName);
	if (UDN)
		free(UDN);
	if (baseURL)
		free(baseURL);
	if (relURL)
		free(relURL);
}

int ctrl_point_remove_all(void)
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
