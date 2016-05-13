#ifndef CTRL_POINT_H
#define CTRL_POINT_H

#include <stdio.h>
#include <string.h>
#include <glib.h>
#include "ithread.h"
#include "upnp.h"
#include "upnptools.h"

#define CP_SUCCESS		0
#define CP_ERROR		(-1)
#define CP_WARNING		1

#define UP_SERVICE_SERVCOUNT  2
#define UP_MAXVARS 4

#define FILTER_UNMATCH -1
#define FILTER_NULL -2

#define FILTER_NAME_SIZE 80

typedef struct {
	char ServiceId[NAME_SIZE];
	char ServiceType[NAME_SIZE];
	char *VariableStrVal[UP_MAXVARS];
	char EventURL[NAME_SIZE];
	char ControlURL[NAME_SIZE];
	char SID[NAME_SIZE];
}Up_service;

typedef struct {
	char UDN[250];
	char DescDocURL[250];
	char FriendlyName[250];
	char PresURL[250];
	int  AdvrTimeOut;
        Up_service UpService[UP_SERVICE_SERVCOUNT];
}UpDevice;

struct UpDeviceNode{
        UpDevice device;
        struct UpDeviceNode *next;
};

typedef struct {
	const char (*filter)[FILTER_NAME_SIZE];
	int filter_len;
}CPFilter;

extern ithread_mutex_t DeviceListMutex;
extern struct UpDeviceNode *GlobalDeviceList;

int Util_MatchFilters(CPFilter *filter, const char *str);
char *Util_GetFirstDocumentItem(IXML_Document *doc, const char *item);
char *Util_GetFirstElementItem(IXML_Element *element, const char *item);
void Util_ListService(IXML_NodeList *ServiceList, CPFilter *type_filter);
void Util_ListServiceList(IXML_Document *doc, CPFilter *type_filter);

void CtrlPointAddDevice(IXML_Document *DescDoc, const char *location, int expires);
int CtrlPointRemoveAll(void);

#endif
