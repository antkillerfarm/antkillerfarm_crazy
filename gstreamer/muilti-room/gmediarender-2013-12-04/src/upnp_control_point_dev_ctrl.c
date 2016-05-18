#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include <upnp/upnp.h>
#include <upnp/ithread.h>
#include <upnp/upnptools.h>

#include "upnp_control_point.h"

int dev_node_get_var(int service, struct UpDeviceNode *devnode, const char *varname, int is_lock)
{
	
	int rc;
	g_print("%s 0\n", __FUNCTION__);
	if (is_lock)
	{
		ithread_mutex_lock(&DeviceListMutex);
	}
	
	rc = UpnpGetServiceVarStatusAsync(
		ctrlpt_handle,
		devnode->device.UpService[service].ControlURL,
		varname,
		upnp_ctrl_point_callback_event_handler,
		NULL);
	if (rc != UPNP_E_SUCCESS) {
		g_print(
			"Error in UpnpGetServiceVarStatusAsync -- %d\n",
			rc);
		rc = CP_ERROR;
	}

	if (is_lock)
	{
		ithread_mutex_unlock(&DeviceListMutex);
	}
	return rc;
}

int dev_node_get_volume(struct UpDeviceNode *devnode)
{
	g_print("%s %d\n", __FUNCTION__, UP_SERVICE_RENDERING_CONTROL);
	return dev_node_get_var(UP_SERVICE_RENDERING_CONTROL, devnode, "Volume", FALSE);
	//g_print(" -- %s\n", devnode->device.UDN);
	//return CP_SUCCESS;
}

int dev_node_print(struct UpDeviceNode *devnode)
{
	g_print(" -- %s\n", devnode->device.UDN);
	return CP_SUCCESS;
}
