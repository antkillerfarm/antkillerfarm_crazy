#ifndef CTRL_POINT_H
#define CTRL_POINT_H

#include <stdio.h>
#include <string.h>
#include <glib.h>
#include "ithread.h"
#include "upnp.h"

#define CP_SUCCESS		0
#define CP_ERROR		(-1)
#define CP_WARNING		1

#define UP_SERVICE_SERVCOUNT  2
#define UP_MAXVARS 4

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

extern ithread_mutex_t DeviceListMutex;
extern struct UpDeviceNode *GlobalDeviceList;

char *Util_GetFirstDocumentItem(IXML_Document *doc, const char *item);
void Util_ListService(IXML_NodeList *ServiceList);
void Util_ListServiceList(IXML_Document *doc);

void CtrlPointAddDevice(IXML_Document *DescDoc, const char *location, int expires);
int CtrlPointRemoveAll(void);

#endif
