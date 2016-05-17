#ifndef _UPNP_CONTROL_POINT_H
#define _UPNP_CONTROL_POINT_H

#define CP_SUCCESS		0
#define CP_ERROR		(-1)
#define CP_WARNING		1

#define FILTER_NAME_SIZE 80

#define FILTER_UNMATCH -1
#define FILTER_NULL -2

#define EVENT_TYPE_DATA_MACRO(x) x, #x

///////////////////////////////////
// Macro needed be modified by usage

#define UP_SERVICE_SERVCOUNT  3
#define UP_MAXVARS 4

///////////////////////////////////

typedef struct {
	Upnp_EventType EventType;
        char EventTypeString[64];
	int (*event_handler)(Upnp_EventType EventType, void *Event, void *Cookie);
}EventTypeData;

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
	const char **filter;
	int filter_len;
}ControlPointFilter;

///////////////////////////////////

extern ithread_mutex_t DeviceListMutex;
extern struct UpDeviceNode *GlobalDeviceList;
extern const char upnp_device_type[];

/////////////////////////////////////

int upnp_ctrl_point_start(void);
int upnp_ctrl_point_stop(void);
int ctrl_point_refresh(void);
int upnp_ctrl_point_callback_event_handler(Upnp_EventType EventType, void *Event, void *Cookie);

int upnp_discovery_search_result_handler(Upnp_EventType EventType, void *Event, void *Cookie);

int ctrl_point_remove_all(void);
void ctrl_point_add_device(IXML_Document *DescDoc, const char *location, int expires);

int util_match_filters(ControlPointFilter *filter, const char *str);
char *util_get_first_document_item(IXML_Document *doc, const char *item);
char *util_get_first_element_item(IXML_Element *element, const char *item);
void util_list_service(IXML_NodeList *ServiceList, ControlPointFilter *type_filter);
void util_list_service_list(IXML_Document *doc, ControlPointFilter *type_filter);
#endif /* _UPNP_CONTROL_POINT_H */
