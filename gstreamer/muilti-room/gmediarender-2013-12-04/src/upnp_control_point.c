#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include <upnp/upnp.h>
#include <upnp/ithread.h>
#include <upnp/upnptools.h>

#include "upnp_control_point.h"

/*! Device type for tv device. */
const char upnp_device_type[] = "urn:schemas-upnp-org:device:MediaRenderer:1";

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

////////////////////////////////////////

ithread_mutex_t DeviceListMutex;

UpnpClient_Handle ctrlpt_handle = -1;

struct UpDeviceNode *GlobalDeviceList = NULL;

int upnp_ctrl_point_callback_event_handler(Upnp_EventType EventType, void *Event, void *Cookie)
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
		ctrl_point_add_device(DescDoc, d_event->Location, d_event->Expires);
	}
	if (DescDoc) {
		ixmlDocument_free(DescDoc);
	}
	ctrl_point_print_list();
	return CP_SUCCESS;
}

int ctrl_point_refresh(void)
{
	int rc;

	ctrl_point_remove_all();
	/* Search for all devices of type tvdevice version 1,
	 * waiting for up to 5 seconds for the response */
	rc = UpnpSearchAsync(ctrlpt_handle, 5, upnp_device_type, NULL);
	if (UPNP_E_SUCCESS != rc) {
	        g_print("Error sending search request%d\n", rc);

		return CP_ERROR;
	}

	return CP_SUCCESS;
}

int upnp_ctrl_point_start(void)
{
	int rc;
	unsigned short port = 0;
	char *ip_address = NULL;

	ithread_mutex_init(&DeviceListMutex, 0);

	g_print("Initializing UPnP Sdk with\n"
			 "\tipaddress = %s port = %u\n",
			 ip_address ? ip_address : "{NULL}", port);

	//g_print("XXX %d\n", get_argument_length(arguments_set_vol));

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
	rc = UpnpRegisterClient(upnp_ctrl_point_callback_event_handler,
				&ctrlpt_handle, &ctrlpt_handle);
	if (rc != UPNP_E_SUCCESS) {
		g_print("Error registering CP: %d\n", rc);
		//UpnpFinish();

		return CP_ERROR;
	}

	g_print("Control Point Registered\n");

        ctrl_point_refresh();

	return CP_SUCCESS;
}

int upnp_ctrl_point_stop(void)
{
        ctrl_point_remove_all();
	UpnpUnRegisterClient(ctrlpt_handle);

	return CP_SUCCESS;
}
