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

int dev_node_get_ip_info(struct UpDeviceNode *devnode)
{
	//g_print("%s %s\n", __FUNCTION__, devnode->device.DescDocURL);
	//return dev_node_get_var(UP_SERVICE_RENDERING_CONTROL, devnode, "Volume", FALSE);
	char DescDocURL[250];
	strcpy(DescDocURL, devnode->device.DescDocURL);
	const char *d = "/:";
	char *p;
	int i = 0;
	p = strtok(DescDocURL, d);
	while(p)
	{
		if (i == 1)
		{
			strcpy(devnode->user_data.ip_addr, p);
		}
		else if (i == 2)
		{
			strcpy(devnode->user_data.port, p);
		}
		p = strtok(NULL, d);
		i++;
	}
	g_print("%s %s\n", devnode->user_data.ip_addr, devnode->user_data.port);
	return CP_SUCCESS;
}

int dev_node_get_volume(struct UpDeviceNode *devnode)
{
	return dev_node_get_var(UP_SERVICE_RENDERING_CONTROL, devnode, "Volume", FALSE);
}

int dev_node_print(struct UpDeviceNode *devnode)
{
	g_print(" -- %s\n", devnode->device.UDN);
	return CP_SUCCESS;
}
