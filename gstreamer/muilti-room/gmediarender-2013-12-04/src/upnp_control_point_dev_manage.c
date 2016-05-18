#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include <upnp/upnp.h>
#include <upnp/ithread.h>
#include <upnp/upnptools.h>

#include "upnp_control_point.h"


/*! Device type for add devices. */
//const int ctrl_point_service_type_len = 3;
const char *ctrl_point_service_type[] =
{
	"urn:schemas-upnp-org:service:AVTransport:1",
	"urn:schemas-upnp-org:service:ConnectionManager:1",
	"urn:schemas-upnp-org:service:RenderingControl:1"
};

const char *UpServiceName[] = { "AVTransport", "ConnectionManager", "RenderingControl"};

const char *UpVarName[UP_SERVICE_SERVCOUNT][UP_MAXVARS] = {
    {"", "", "", ""},
    {"", "", "", ""},
    {"Volume", "", "", ""}
};
char UpVarCount[UP_SERVICE_SERVCOUNT] =
{ UP_AV_TRANSPORT_VARCOUNT, UP_CONNECTION_MANAGER_VARCOUNT, UP_RENDERING_CONTROL_VARCOUNT};

int default_timeout = 1801;

////////////////////////////////////

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
	char *serviceId[UP_SERVICE_SERVCOUNT] = {0};
	char *eventURL[UP_SERVICE_SERVCOUNT] = {0};
	char *controlURL[UP_SERVICE_SERVCOUNT] = {0};
	Upnp_SID eventSID[UP_SERVICE_SERVCOUNT];
	int TimeOut[UP_SERVICE_SERVCOUNT];
	struct UpDeviceNode *deviceNode;
	struct UpDeviceNode *tmpdevnode;
	int ret = 1;
	int found = 0;
	int service;
	int var;
	int i;
	for (i = 0; i < UP_SERVICE_SERVCOUNT; i++)
	{
		TimeOut[i] = default_timeout;
	}

	ithread_mutex_lock(&DeviceListMutex);

	/* Read key elements from description document */
	UDN = util_get_first_document_item(DescDoc, "UDN");
	deviceType = util_get_first_document_item(DescDoc, "deviceType");
	friendlyName = util_get_first_document_item(DescDoc, "friendlyName");
	baseURL = util_get_first_document_item(DescDoc, "URLBase");
	relURL = util_get_first_document_item(DescDoc, "presentationURL");

	ret = UpnpResolveURL((baseURL ? baseURL : location), relURL, presURL);

	if (UPNP_E_SUCCESS != ret)
		g_print("Error generating presURL from %s + %s\n",
				 baseURL, relURL);

	if (strcmp(deviceType, upnp_device_type) == 0) {
		g_print("Found Upnp device\n");

		/* Check if this device is already in the list */
		tmpdevnode = GlobalDeviceList;
		while (tmpdevnode) {
			if (strcmp(tmpdevnode->device.UDN, UDN) == 0) {
				found = 1;
				break;
			}
			tmpdevnode = tmpdevnode->next;
		}

		if (found) {
			/* The device is already there, so just update  */
			/* the advertisement timeout field */
			tmpdevnode->device.AdvrTimeOut = expires;
		} else {
			for (service = 0; service < UP_SERVICE_SERVCOUNT;
			     service++) {
				if (util_find_and_parse_service
				    (DescDoc, location, ctrl_point_service_type[service],
				     &serviceId[service], &eventURL[service],
				     &controlURL[service])) {
					g_print
					    ("Subscribing to EventURL %s...\n",
					     eventURL[service]);
					ret =
					    UpnpSubscribe(ctrlpt_handle,
							  eventURL[service],
							  &TimeOut[service],
							  eventSID[service]);
					if (ret == UPNP_E_SUCCESS) {
						g_print
						    ("Subscribed to EventURL with SID=%s\n",
						     eventSID[service]);
					} else {
						g_print
						    ("Error Subscribing to EventURL -- %d\n",
						     ret);
						strcpy(eventSID[service], "");
					}
				} else {
					g_print
					    ("Error: Could not find Service: %s\n",
					     ctrl_point_service_type[service]);
				}
			}
			/* Create a new device node */
			deviceNode =
			    (struct UpDeviceNode *)
			    malloc(sizeof(struct UpDeviceNode));
			strcpy(deviceNode->device.UDN, UDN);
			strcpy(deviceNode->device.DescDocURL, location);
			strcpy(deviceNode->device.FriendlyName, friendlyName);
			strcpy(deviceNode->device.PresURL, presURL);
			deviceNode->device.AdvrTimeOut = expires;
			for (service = 0; service < UP_SERVICE_SERVCOUNT;
			     service++) {
				if (serviceId[service] == NULL) {
					/* not found */
					continue;
				}
				strcpy(deviceNode->device.UpService[service].
				       ServiceId, serviceId[service]);
				strcpy(deviceNode->device.UpService[service].
				       ServiceType, ctrl_point_service_type[service]);
				strcpy(deviceNode->device.UpService[service].
				       ControlURL, controlURL[service]);
				strcpy(deviceNode->device.UpService[service].
				       EventURL, eventURL[service]);
				strcpy(deviceNode->device.UpService[service].
				       SID, eventSID[service]);
				for (var = 0; var < UpVarCount[service]; var++) {
					deviceNode->device.
					    UpService[service].VariableStrVal
					    [var] =
					    (char *)malloc(UP_MAX_VAL_LEN);
					strcpy(deviceNode->device.
					       UpService[service].VariableStrVal
					       [var], "");
				}
			}
			deviceNode->next = NULL;
			/* Insert the new device node in the list */
			if ((tmpdevnode = GlobalDeviceList)) {
				while (tmpdevnode) {
					if (tmpdevnode->next) {
						tmpdevnode = tmpdevnode->next;
					} else {
						tmpdevnode->next = deviceNode;
						break;
					}
				}
			} else {
				GlobalDeviceList = deviceNode;
			}
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
	for (service = 0; service < UP_SERVICE_SERVCOUNT; service++) {
		if (serviceId[service])
			free(serviceId[service]);
		if (controlURL[service])
			free(controlURL[service]);
		if (eventURL[service])
			free(eventURL[service]);
	}
}

int ctrl_point_remove_device(const char *UDN)
{
	struct UpDeviceNode *curdevnode;
	struct UpDeviceNode *prevdevnode;

	ithread_mutex_lock(&DeviceListMutex);

	curdevnode = GlobalDeviceList;
	if (!curdevnode) {
		g_print(
			"WARNING: TvCtrlPointRemoveDevice: Device list empty\n");
	} else {
		if (0 == strcmp(curdevnode->device.UDN, UDN)) {
			GlobalDeviceList = curdevnode->next;
		        ctrl_point_delete_node(curdevnode);
		} else {
			prevdevnode = curdevnode;
			curdevnode = curdevnode->next;
			while (curdevnode) {
				if (strcmp(curdevnode->device.UDN, UDN) == 0) {
					prevdevnode->next = curdevnode->next;
					ctrl_point_delete_node(curdevnode);
					break;
				}
				prevdevnode = curdevnode;
				curdevnode = curdevnode->next;
			}
		}
	}

	ithread_mutex_unlock(&DeviceListMutex);

	return CP_SUCCESS;
}

int ctrl_point_remove_all(void)
{
        struct UpDeviceNode *curdevnode, *next;

	ithread_mutex_lock(&DeviceListMutex);

	curdevnode = GlobalDeviceList;
	GlobalDeviceList = NULL;

	while (curdevnode) {
		next = curdevnode->next;
		ctrl_point_delete_node(curdevnode);
		curdevnode = next;
	}

	ithread_mutex_unlock(&DeviceListMutex);

	return CP_SUCCESS;
}

int ctrl_point_delete_node( struct UpDeviceNode *node )
{
	int rc, service, var;

	if (NULL == node) {
		g_print("ERROR: ctrl_point_delete_node: Node is empty\n");
		return CP_ERROR;
	}

	for (service = 0; service < UP_SERVICE_SERVCOUNT; service++) {
		/*
		   If we have a valid control SID, then unsubscribe 
		 */
		if (strcmp(node->device.UpService[service].SID, "") != 0) {
			rc = UpnpUnSubscribe(ctrlpt_handle,
					     node->device.UpService[service].
					     SID);
			if (UPNP_E_SUCCESS == rc) {
				g_print("Unsubscribed from Upnp %s EventURL with SID=%s\n",
				     UpServiceName[service],
				     node->device.UpService[service].SID);
			} else {
				g_print("Error unsubscribing to Upnp %s EventURL -- %d\n",
				     UpServiceName[service], rc);
			}
		}

		for (var = 0; var < UpVarCount[service]; var++) {
			if (node->device.UpService[service].VariableStrVal[var]) {
				free(node->device.
				     UpService[service].VariableStrVal[var]);
			}
		}
	}

	free(node);
	node = NULL;

	return CP_SUCCESS;
}

void ctrl_point_state_update(char *UDN, int Service, IXML_Document *ChangedVariables, char **State)
{
	IXML_NodeList *properties;
	IXML_NodeList *variables;
	IXML_Element *property;
	IXML_Element *variable;
	long unsigned int length;
	long unsigned int length1;
	long unsigned int i;
	int j;
	char *tmpstate = NULL;

	g_print("Upnp State Update (service %d):\n", Service);
	/* Find all of the e:property tags in the document */
	properties = ixmlDocument_getElementsByTagName(ChangedVariables,
		"e:property");
	if (properties) {
		length = ixmlNodeList_length(properties);
		for (i = 0; i < length; i++) {
			/* Loop through each property change found */
			property = (IXML_Element *)ixmlNodeList_item(
				properties, i);
			/* For each variable name in the state table,
			 * check if this is a corresponding property change */
			for (j = 0; j < UpVarCount[Service]; j++) {
				variables = ixmlElement_getElementsByTagName(
					property, UpVarName[Service][j]);
				/* If a match is found, extract 
				 * the value, and update the state table */
				if (variables) {
					length1 = ixmlNodeList_length(variables);
					if (length1) {
						variable = (IXML_Element *)
							ixmlNodeList_item(variables, 0);
						tmpstate = util_get_element_value(variable);
						if (tmpstate) {
							strcpy(State[j], tmpstate);
							g_print(
								" Variable Name: %s New Value:'%s'\n",
								UpVarName[Service][j], State[j]);
						}
						if (tmpstate)
							free(tmpstate);
						tmpstate = NULL;
					}
					ixmlNodeList_free(variables);
					variables = NULL;
				}
			}
		}
		ixmlNodeList_free(properties);
	}
	return;
	UDN = UDN;
}

