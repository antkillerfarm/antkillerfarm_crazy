#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include <upnp/upnp.h>
#include <upnp/ithread.h>
#include <upnp/upnptools.h>

#include "upnp_control_point.h"

char *util_get_element_value(IXML_Element *element)
{
	IXML_Node *child = ixmlNode_getFirstChild((IXML_Node *)element);
	char *temp = NULL;

	if (child != 0 && ixmlNode_getNodeType(child) == eTEXT_NODE)
		temp = strdup(ixmlNode_getNodeValue(child));

	return temp;
}

char *util_get_first_document_item(IXML_Document *doc, const char *item)
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

char *util_get_first_element_item(IXML_Element *element, const char *item)
{
	IXML_NodeList *nodeList = NULL;
	IXML_Node *textNode = NULL;
	IXML_Node *tmpNode = NULL;
	char *ret = NULL;

	nodeList = ixmlElement_getElementsByTagName(element, (char *)item);
	if (nodeList == NULL) {
	        g_print("%s(%d): Error finding %s in XML Node\n",
			__FILE__, __LINE__, item);
		return NULL;
	}
	tmpNode = ixmlNodeList_item(nodeList, 0);
	if (!tmpNode) {
		g_print("%s(%d): Error finding %s value in XML Node\n",
			__FILE__, __LINE__, item);
		ixmlNodeList_free(nodeList);
		return NULL;
	}
	textNode = ixmlNode_getFirstChild(tmpNode);
	ret = strdup(ixmlNode_getNodeValue(textNode));
	if (!ret) {
		g_print("%s(%d): Error allocating memory for %s in XML Node\n",
			__FILE__, __LINE__, item);
		ixmlNodeList_free(nodeList);
		return NULL;
	}
	ixmlNodeList_free(nodeList);

	return ret;
}

static IXML_NodeList *util_get_nth_service_list(
	/*! [in] . */
	IXML_Document *doc,
	/*! [in] . */
	unsigned int n)
{
	IXML_NodeList *ServiceList = NULL;
	IXML_NodeList *servlistnodelist = NULL;
	IXML_Node *servlistnode = NULL;

	/*  ixmlDocument_getElementsByTagName()
	 *  Returns a NodeList of all Elements that match the given
	 *  tag name in the order in which they were encountered in a preorder
	 *  traversal of the Document tree.  
	 *
	 *  return (NodeList*) A pointer to a NodeList containing the 
	 *                      matching items or NULL on an error. 	 */
	//g_print("util_get_nth_service_list called : n = %d\n", n);
	servlistnodelist =
		ixmlDocument_getElementsByTagName(doc, "serviceList");
	if (servlistnodelist &&
	    ixmlNodeList_length(servlistnodelist) &&
	    n < ixmlNodeList_length(servlistnodelist)) {
		/* For the first service list (from the root device),
		 * we pass 0 */
		/*servlistnode = ixmlNodeList_item( servlistnodelist, 0 );*/

		/* Retrieves a Node from a NodeList} specified by a 
		 *  numerical index.
		 *
		 *  return (Node*) A pointer to a Node or NULL if there was an 
		 *                  error. */
		servlistnode = ixmlNodeList_item(servlistnodelist, n);
		if (servlistnode) {
			/* create as list of DOM nodes */
			ServiceList = ixmlElement_getElementsByTagName(
				(IXML_Element *)servlistnode, "service");
		} else
			g_print("%s(%d): ixmlNodeList_item(nodeList, n) returned NULL\n",
				__FILE__, __LINE__);
	}
	if (servlistnodelist)
		ixmlNodeList_free(servlistnodelist);

	return ServiceList;
}

int util_find_and_parse_service(IXML_Document *DescDoc, const char *location,
	const char *serviceType, char **serviceId, char **eventURL, char **controlURL)
{
	unsigned int i;
	unsigned long length;
	int found = 0;
	int ret;
	unsigned int sindex = 0;
	char *tempServiceType = NULL;
	char *baseURL = NULL;
	const char *base = NULL;
	char *relcontrolURL = NULL;
	char *releventURL = NULL;
	IXML_NodeList *serviceList = NULL;
	IXML_Element *service = NULL;

	baseURL = util_get_first_document_item(DescDoc, "URLBase");
	if (baseURL)
		base = baseURL;
	else
		base = location;
	
	for (sindex = 0;
	     (serviceList = util_get_nth_service_list(DescDoc , sindex)) != NULL;
	     sindex++) {
		tempServiceType = NULL;
		relcontrolURL = NULL;
		releventURL = NULL;
		service = NULL;

		length = ixmlNodeList_length(serviceList);
		for (i = 0; i < length; i++) {
			service = (IXML_Element *)ixmlNodeList_item(serviceList, i);
			tempServiceType = util_get_first_element_item(
				(IXML_Element *)service, "serviceType");
			if (tempServiceType && strcmp(tempServiceType, serviceType) == 0) {
				g_print("Found service: %s\n", serviceType);
				*serviceId = util_get_first_element_item(service, "serviceId");
				g_print("serviceId: %s\n", *serviceId);
				relcontrolURL = util_get_first_element_item(service, "controlURL");
				releventURL = util_get_first_element_item(service, "eventSubURL");
				*controlURL = malloc(strlen(base) + strlen(relcontrolURL) + 1);
				if (*controlURL) {
					ret = UpnpResolveURL(base, relcontrolURL, *controlURL);
					if (ret != UPNP_E_SUCCESS)
						g_print("Error generating controlURL from %s + %s\n",
							base, relcontrolURL);
				}
				*eventURL = malloc(strlen(base) + strlen(releventURL) + 1);
				if (*eventURL) {
					ret = UpnpResolveURL(base, releventURL, *eventURL);
					if (ret != UPNP_E_SUCCESS)
						g_print("Error generating eventURL from %s + %s\n",
							base, releventURL);
				}
				free(relcontrolURL);
				free(releventURL);
				relcontrolURL = NULL;
				releventURL = NULL;
				found = 1;
				break;
			}
			free(tempServiceType);
			tempServiceType = NULL;
		}
		free(tempServiceType);
		tempServiceType = NULL;
		if (serviceList)
			ixmlNodeList_free(serviceList);
		serviceList = NULL;
	}

	free(baseURL);

	return found;
}

int ctrl_point_dev_node_operation(DevNodeOperation *dev_node_op)
{
	struct UpDeviceNode *tmpdevnode;

	ithread_mutex_lock(&DeviceListMutex);

	g_print("ctrl_point_dev_node_operation:\n");
	tmpdevnode = GlobalDeviceList;
	while (tmpdevnode) {
		if (dev_node_op->operation)
		{
			dev_node_op->operation(tmpdevnode);
		}
		tmpdevnode = tmpdevnode->next;
	}
	g_print("\n");
	ithread_mutex_unlock(&DeviceListMutex);

	return CP_SUCCESS;
}

void ctrl_point_handle_get_var(
	const char *controlURL,
	const char *varName,
	const DOMString varValue)
{

	struct UpDeviceNode *tmpdevnode;
	int service;

	ithread_mutex_lock(&DeviceListMutex);

	tmpdevnode = GlobalDeviceList;
	while (tmpdevnode) {
		for (service = 0; service < UP_SERVICE_SERVCOUNT; service++) {
			if (strcmp
			    (tmpdevnode->device.UpService[service].ControlURL,
			     controlURL) == 0) {
				break;
			}
		}
		tmpdevnode = tmpdevnode->next;
	}

	ithread_mutex_unlock(&DeviceListMutex);
}

void ctrl_point_handle_event(
	const char *sid,
	int evntkey,
	IXML_Document *changes)
{
	struct UpDeviceNode *tmpdevnode;
	int service;

	ithread_mutex_lock(&DeviceListMutex);

	tmpdevnode = GlobalDeviceList;
	while (tmpdevnode) {
		for (service = 0; service < UP_SERVICE_SERVCOUNT; ++service) {
			if (strcmp(tmpdevnode->device.UpService[service].SID, sid) ==  0) {
				g_print("Received Upnp %s Event: %d for SID %s\n",
					UpServiceName[service],
					evntkey,
					sid);
				ctrl_point_state_update(
					tmpdevnode->device.UDN,
					service,
					changes,
					(char **)&tmpdevnode->device.UpService[service].VariableStrVal);
				break;
			}
		}
		tmpdevnode = tmpdevnode->next;
	}

	ithread_mutex_unlock(&DeviceListMutex);
}

void ctrl_point_handle_subscribe_update(
	const char *eventURL,
	const Upnp_SID sid,
	int timeout)
{
	struct UpDeviceNode *tmpdevnode;
	int service;

	ithread_mutex_lock(&DeviceListMutex);

	tmpdevnode = GlobalDeviceList;
	while (tmpdevnode) {
		for (service = 0; service < UP_SERVICE_SERVCOUNT; service++) {
			if (strcmp
			    (tmpdevnode->device.UpService[service].EventURL,
			     eventURL) == 0) {
				g_print
				    ("Received Upnp %s Event Renewal for eventURL %s\n",
				     UpServiceName[service], eventURL);
				strcpy(tmpdevnode->device.UpService[service].
				       SID, sid);
				break;
			}
		}

		tmpdevnode = tmpdevnode->next;
	}

	ithread_mutex_unlock(&DeviceListMutex);

	return;
	timeout = timeout;
}

int ctrl_point_get_device(int devnum, struct UpDeviceNode **devnode)
{
	int count = devnum;
	struct UpDeviceNode *tmpdevnode = NULL;

	if (count)
		tmpdevnode = GlobalDeviceList;
	while (--count && tmpdevnode) {
		tmpdevnode = tmpdevnode->next;
	}
	if (!tmpdevnode) {
	        g_print("Error finding UpDevice number -- %d\n",
				 devnum);
		return CP_ERROR;
	}
	*devnode = tmpdevnode;

	return CP_SUCCESS;
}

