#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gst/gst.h>

#include "upnp_control.h"
#include "upnp_control_point.h"
#include "output_gstreamer.h"

int dev_node_get_var(int service, struct UpDeviceNode *devnode, const char *varname, int is_lock)
{
	int rc;
	//g_print("%s 0\n", __FUNCTION__);
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

int dev_node_get_ip_info(struct UpDeviceNode *devnode, gpointer data)
{
	if (devnode->user_data.flag_ip_info == FALSE)
	{
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
				if (strcmp(p, UpnpGetServerIpAddress()) == 0)
				{
					devnode->user_data.flag_our_device = TRUE;
				}
			}
			else if (i == 2)
			{
				devnode->user_data.port = atoi(p);
				//strcpy(devnode->user_data.port, p);
			}
			p = strtok(NULL, d);
			i++;
		}
		g_print("%s %d\n", devnode->user_data.ip_addr, devnode->user_data.port);
		devnode->user_data.flag_ip_info = TRUE;
	}
	return CP_SUCCESS;
}

int dev_node_get_volume(struct UpDeviceNode *devnode, gpointer data)
{
	return dev_node_get_var(UP_SERVICE_RENDERING_CONTROL, devnode, "Volume", FALSE);
}

int dev_node_get_group_info(struct UpDeviceNode *devnode, gpointer data)
{
	if (devnode->user_data.flag_our_device == TRUE)
	{
		return CP_SUCCESS;
	}
	if (devnode->user_data.flag_group_id == DEVICE_GROUP_ID_FLAG_INIT)
	{
		dev_node_get_var(UP_SERVICE_RENDERING_CONTROL, devnode, "GroupID", FALSE);
		devnode->user_data.flag_group_id = DEVICE_GROUP_ID_FLAG_REQUEST;
	}
	if (devnode->user_data.flag_group_role == DEVICE_GROUP_ROLE_FLAG_INIT)
	{
	        dev_node_get_var(UP_SERVICE_RENDERING_CONTROL, devnode, "GroupRole", FALSE);
		devnode->user_data.flag_group_role = DEVICE_GROUP_ROLE_FLAG_REQUEST;
	}
	return CP_SUCCESS;
}

int dev_node_print(struct UpDeviceNode *devnode, gpointer data)
{
	g_print(" -- %s\n", devnode->device.UDN);
	return CP_SUCCESS;
}

void dev_node_get_var_handler(struct UpDeviceNode *devnode, const char *varName, const DOMString varValue)
{
	if (strcmp(varName, "GroupID") == 0)
	{
		g_print("%s: %s %s %s\n", __FUNCTION__, varName, varValue, get_control_var(CONTROL_VAR_GROUPID));
		strncpy(devnode->user_data.group_id, varValue, 20);
		if (strcmp(varValue, get_control_var(CONTROL_VAR_GROUPID)) == 0)
		{
			devnode->user_data.flag_our_group = DEVICE_GROUP_ID_SAME;
		}
		else
		{
			devnode->user_data.flag_our_group = DEVICE_GROUP_ID_NOT_SAME;
		}
		devnode->user_data.flag_group_id = DEVICE_GROUP_ID_FLAG_VALID;
		g_print("%s: %d\n", __FUNCTION__, devnode->user_data.flag_our_group);
	}
	else if (strcmp(varName, "GroupRole") == 0)
	{
		g_print("%s: %s %s %s\n", __FUNCTION__, varName, varValue, get_control_var(CONTROL_VAR_GROUPROLE));
		devnode->user_data.group_role = string_to_group_role(varValue);
		devnode->user_data.flag_group_role = DEVICE_GROUP_ROLE_FLAG_VALID;
	}
}

int dev_node_add_gst_pipeline(struct UpDeviceNode *devnode, gpointer data)
{
	if (devnode->user_data.flag_gst_pipeline == TRUE)
	{
		return CP_SUCCESS;
	}
	if (devnode->user_data.flag_our_group != DEVICE_GROUP_ID_SAME)
	{
		return CP_SUCCESS;
	}
	if (devnode->user_data.group_role != DEVICE_PLAY_MODE_SLAVE)
	{
		return CP_SUCCESS;
	}
	if (devnode->user_data.flag_ip_info != TRUE)
	{
		return CP_SUCCESS;
	}
	
	g_print("%s: %s\n", __FUNCTION__, devnode->user_data.ip_addr);
	add_slave_to_pipeline(devnode->user_data.ip_addr);
	add_slave_to_control(devnode);
	devnode->user_data.flag_gst_pipeline = TRUE;
	
	return CP_SUCCESS;
}

int dev_node_send_cmd(struct UpDeviceNode *devnode, gpointer data)
{
	GError * error = NULL;
	char* cmd = (char*)data;
        if (devnode->user_data.flag_gst_pipeline != TRUE)
	{
		return CP_SUCCESS;
	}
	GOutputStream * ostream = g_io_stream_get_output_stream (G_IO_STREAM (devnode->user_data.connection));
	g_output_stream_write(ostream, cmd, strlen(cmd), NULL, &error);
	/* don't forget to check for errors */
	if (error != NULL)
	{
		g_print ("%s", error->message);
	}
	return CP_SUCCESS;
}

void send_cmd_to_server(gchar *cmd)
{
	DevNodeOperation dev_node_op;
	dev_node_op.operation = dev_node_print;
	dev_node_op.data = cmd;
        ctrl_point_dev_node_operation(&dev_node_op);
}
