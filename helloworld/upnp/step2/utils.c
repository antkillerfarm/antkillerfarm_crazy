#include "ctrl_point.h"

char *Util_GetFirstDocumentItem(IXML_Document *doc, const char *item)
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

void Util_ListService(IXML_NodeList *ServiceList)
{
        int i;
	int list_len = ixmlNodeList_length(ServiceList);
	for (i = 0; i < list_len; i++)
	{
		g_print("%s\n", __FUNCTION__);
	}
}

void Util_ListServiceList(IXML_Document *doc)
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
		        if (!servlistnode)
			{
				/* create as list of DOM nodes */
				ServiceList = ixmlElement_getElementsByTagName((IXML_Element *)servlistnode, "service");
				if (!ServiceList)
				{
					Util_ListService(ServiceList);
				}
			} 
		}
	}
}
