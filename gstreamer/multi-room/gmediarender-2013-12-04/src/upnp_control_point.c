#include <stdio.h>
#include <stdlib.h>
#include <gst/gst.h>

#include "upnp_control_point.h"
#include "output_module.h"
#include "output_gstreamer.h"

/*! Device type for tv device. */
const char upnp_device_type[] = "urn:schemas-upnp-org:device:MediaRenderer:1";

EventTypeData event_type_data[] =
{
	/* Discovery */
	{EVENT_TYPE_DATA_MACRO(UPNP_DISCOVERY_ADVERTISEMENT_ALIVE), upnp_discovery_search_result_handler},
	{EVENT_TYPE_DATA_MACRO(UPNP_DISCOVERY_ADVERTISEMENT_BYEBYE), upnp_discovery_byebye_handler},
	{EVENT_TYPE_DATA_MACRO(UPNP_DISCOVERY_SEARCH_RESULT), upnp_discovery_search_result_handler},
	{EVENT_TYPE_DATA_MACRO(UPNP_DISCOVERY_SEARCH_TIMEOUT), NULL},

	/* SOAP */
	{EVENT_TYPE_DATA_MACRO(UPNP_CONTROL_ACTION_COMPLETE), upnp_control_action_complete_handler},
	{EVENT_TYPE_DATA_MACRO(UPNP_CONTROL_GET_VAR_COMPLETE), upnp_control_get_var_complete_handler},

	/* GENA */
	{EVENT_TYPE_DATA_MACRO(UPNP_EVENT_RECEIVED), upnp_event_received_handler},
	{EVENT_TYPE_DATA_MACRO(UPNP_EVENT_RENEWAL_COMPLETE), upnp_event_subscribe_handler},
	{EVENT_TYPE_DATA_MACRO(UPNP_EVENT_SUBSCRIBE_COMPLETE), upnp_event_subscribe_handler},
	{EVENT_TYPE_DATA_MACRO(UPNP_EVENT_UNSUBSCRIBE_COMPLETE), upnp_event_subscribe_handler},
	{EVENT_TYPE_DATA_MACRO(UPNP_EVENT_AUTORENEWAL_FAILED), upnp_event_subscription_expired_handler},
	{EVENT_TYPE_DATA_MACRO(UPNP_EVENT_SUBSCRIPTION_EXPIRED), upnp_event_subscription_expired_handler}
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
	DevNodeOperation dev_node_op;
	dev_node_op.operation = dev_node_print;
        ctrl_point_dev_node_operation(&dev_node_op);

	dev_node_op.operation = dev_node_get_ip_info;
        ctrl_point_dev_node_operation(&dev_node_op);

	dev_node_op.operation = dev_node_get_group_info;
        ctrl_point_dev_node_operation(&dev_node_op);

	dev_node_op.operation = dev_node_add_gst_pipeline;
        ctrl_point_dev_node_operation(&dev_node_op);

	gchar cmd[64];
	sprintf(cmd, "Clock#%s#%d\n", UpnpGetServerIpAddress(), gst_data.clock_port);
	send_cmd_to_server(cmd);
	return CP_SUCCESS;
}

int upnp_discovery_byebye_handler(Upnp_EventType EventType, void *Event, void *Cookie)
{
	struct Upnp_Discovery *d_event = (struct Upnp_Discovery *)Event;

	if (d_event->ErrCode != UPNP_E_SUCCESS) {
		g_print("Error in Discovery ByeBye Callback -- %d\n",
				 d_event->ErrCode);
	}
	g_print("Received ByeBye for Device: %s\n", d_event->DeviceId);
        ctrl_point_remove_device(d_event->DeviceId);
	g_print("After byebye:\n");
	DevNodeOperation dev_node_op;
	dev_node_op.operation = dev_node_print;
        ctrl_point_dev_node_operation(&dev_node_op);
	return CP_SUCCESS;
}

int upnp_control_action_complete_handler(Upnp_EventType EventType, void *Event, void *Cookie)
{
	struct Upnp_Action_Complete *a_event = (struct Upnp_Action_Complete *)Event;

	if (a_event->ErrCode != UPNP_E_SUCCESS) {
		g_print("Error in  Action Complete Callback -- %d\n",
				 a_event->ErrCode);
	}
	return CP_SUCCESS;
}

int upnp_control_get_var_complete_handler(Upnp_EventType EventType, void *Event, void *Cookie)
{

	struct Upnp_State_Var_Complete *sv_event = (struct Upnp_State_Var_Complete *)Event;

	if (sv_event->ErrCode != UPNP_E_SUCCESS) {
		g_print("Error in Get Var Complete Callback -- %d\n",
				 sv_event->ErrCode);
	} else {
		g_print("%s %s %s %s\n", __FUNCTION__, sv_event->StateVarName, sv_event->CurrentVal, UpnpString_get_String(sv_event->CtrlUrl));
		ctrl_point_handle_get_var(
					  UpnpString_get_String(sv_event->CtrlUrl),
					  sv_event->StateVarName,
					  sv_event->CurrentVal);
	}
	return CP_SUCCESS;
}

int upnp_event_received_handler(Upnp_EventType EventType, void *Event, void *Cookie)
{
	struct Upnp_Event *e_event = (struct Upnp_Event *)Event;

        ctrl_point_handle_event(
		e_event->Sid,
		e_event->EventKey,
		e_event->ChangedVariables);
	return CP_SUCCESS;
}

int upnp_event_subscribe_handler(Upnp_EventType EventType, void *Event, void *Cookie)
{
	struct Upnp_Event_Subscribe *es_event = (struct Upnp_Event_Subscribe *)Event;

	if (es_event->ErrCode != UPNP_E_SUCCESS) {
		g_print("Error in Event Subscribe Callback -- %d\n",
				 es_event->ErrCode);
	} else {
		ctrl_point_handle_subscribe_update(
			(const char *)es_event->PublisherUrl,
			es_event->Sid,
			es_event->TimeOut);
	}
	return CP_SUCCESS;
}

int upnp_event_subscription_expired_handler(Upnp_EventType EventType, void *Event, void *Cookie)
{
	struct Upnp_Event_Subscribe *es_event = (struct Upnp_Event_Subscribe *)Event;
	int TimeOut = default_timeout;
	Upnp_SID newSID;
	int ret;

	ret = UpnpSubscribe(
		ctrlpt_handle,
		(const char *)es_event->PublisherUrl,
		&TimeOut,
		newSID);
	if (ret == UPNP_E_SUCCESS) {
		g_print("Subscribed to EventURL with SID=%s\n", newSID);
	        ctrl_point_handle_subscribe_update(
			(const char *)es_event->PublisherUrl,
			newSID,
			TimeOut);
	} else {
		g_print("Error Subscribing to EventURL -- %d\n", ret);
	}
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

