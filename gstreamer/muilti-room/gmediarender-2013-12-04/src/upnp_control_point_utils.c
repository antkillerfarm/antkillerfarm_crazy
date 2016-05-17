#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include <upnp/upnp.h>
#include <upnp/ithread.h>
#include <upnp/upnptools.h>

#include "upnp_control_point.h"

int util_match_filters(ControlPointFilter *filter, const char *str)
{
	int i;
	if (filter->filter == NULL)
	{
		return FILTER_NULL;
	}
	for (i = 0; i < filter->filter_len; i++)
	{
		if (strcmp(filter->filter[i], str) == 0)
		{
			return i;
		}
	}
	return FILTER_UNMATCH;
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

void util_list_service(IXML_NodeList *ServiceList, ControlPointFilter *type_filter)
{
        int i;
	int list_len = ixmlNodeList_length(ServiceList);
	char *tempServiceType = NULL;
	IXML_Element *service = NULL;
	//char *baseURL = NULL;
	//const char *base = NULL;
	char *relcontrolURL = NULL;
	char *releventURL = NULL;
	//g_print("%s\n", __FUNCTION__);
	for (i = 0; i < list_len; i++)
	{
		service = (IXML_Element *)ixmlNodeList_item(ServiceList, i);
		tempServiceType = util_get_first_element_item((IXML_Element *)service, "serviceType");
		if (util_match_filters(type_filter, tempServiceType) != -1)
		{
		        g_print("Found service: %s\n", tempServiceType);
			relcontrolURL = util_get_first_element_item(service, "controlURL");
			releventURL = util_get_first_element_item(service, "eventSubURL");
			free(relcontrolURL);
			free(releventURL);
			relcontrolURL = NULL;
			releventURL = NULL;
		}
		if (tempServiceType)
		{
			free(tempServiceType);
			tempServiceType = NULL;
		}
	}
}

void util_list_service_list(IXML_Document *doc, ControlPointFilter *type_filter)
{
	IXML_NodeList *ServiceList = NULL;
	IXML_NodeList *servlistnodelist = NULL;
	IXML_Node *servlistnode = NULL;
	int i;

	servlistnodelist = ixmlDocument_getElementsByTagName(doc, "serviceList");
	if (servlistnodelist)
	{
		int list_len = ixmlNodeList_length(servlistnodelist);
		for (i = 0; i < list_len; i++)
		{
			servlistnode = ixmlNodeList_item(servlistnodelist, i);
		        if (servlistnode)
			{
				/* create as list of DOM nodes */
				ServiceList = ixmlElement_getElementsByTagName((IXML_Element *)servlistnode, "service");
				if (ServiceList)
				{
					util_list_service(ServiceList, type_filter);
					ixmlNodeList_free(ServiceList);
				}
			}
		}
	}
	if (servlistnodelist)
	{
		ixmlNodeList_free(servlistnodelist);
	}
}
