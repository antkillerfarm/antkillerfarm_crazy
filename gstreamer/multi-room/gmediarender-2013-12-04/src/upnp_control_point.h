#ifndef _UPNP_CONTROL_POINT_H
#define _UPNP_CONTROL_POINT_H

#include <glib.h>
#include <gio/gio.h>
#include <upnp/upnp.h>
#include <upnp/ithread.h>
#include <upnp/upnptools.h>

#define CP_SUCCESS		0
#define CP_ERROR		(-1)
#define CP_WARNING		1

#define EVENT_TYPE_DATA_MACRO(x) x, #x

#define DEVICE_PLAY_MODE_SINGLE 0
#define DEVICE_PLAY_MODE_MASTER 1
#define DEVICE_PLAY_MODE_SLAVE 2

#define DEVICE_GROUP_ID_UNKNOWN 0
#define DEVICE_GROUP_ID_SAME 1
#define DEVICE_GROUP_ID_NOT_SAME 2

#define DEVICE_GROUP_ID_FLAG_INIT 0
#define DEVICE_GROUP_ID_FLAG_REQUEST 1
#define DEVICE_GROUP_ID_FLAG_VALID 2

#define DEVICE_GROUP_ROLE_FLAG_INIT 0
#define DEVICE_GROUP_ROLE_FLAG_REQUEST 1
#define DEVICE_GROUP_ROLE_FLAG_VALID 2

///////////////////////////////////
// Macro needed be modified by usage

#define UP_SERVICE_SERVCOUNT  3
#define UP_SERVICE_AV_TRANSPORT	0
#define UP_SERVICE_CONNECTION_MANAGER	1
#define UP_SERVICE_RENDERING_CONTROL	2
#define UP_MAXVARS 4

#define UP_AV_TRANSPORT_VARCOUNT	0
#define UP_CONNECTION_MANAGER_VARCOUNT	0
#define UP_RENDERING_CONTROL_VARCOUNT 3

#define UP_MAX_VAL_LEN 5

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

typedef struct {
	GSocketConnection *connection;
	GSocketClient *client;
	char ip_addr[20];
        int port;
	char group_id[20];
	char group_role;
	char flag_ip_info;
	char flag_our_device;
	char flag_group_id;
	char flag_group_role;
	char flag_our_group;
	char flag_gst_pipeline;
}UpDeviceUserData;

struct UpDeviceNode{
        UpDevice device;
	UpDeviceUserData user_data;
        struct UpDeviceNode *next;
};

typedef enum {
	STATE_UPDATE = 0,
	DEVICE_ADDED = 1,
	DEVICE_REMOVED = 2,
	GET_VAR_COMPLETE = 3
}eventType;

typedef struct {
	int (*operation)(struct UpDeviceNode *devnode);
}DevNodeOperation;

///////////////////////////////////

extern ithread_mutex_t DeviceListMutex;
extern struct UpDeviceNode *GlobalDeviceList;
extern const char upnp_device_type[];
extern UpnpClient_Handle ctrlpt_handle;
extern int default_timeout;
extern const char *UpServiceName[];
extern const char *UpVarName[UP_SERVICE_SERVCOUNT][UP_MAXVARS];
extern char UpVarCount[UP_SERVICE_SERVCOUNT];

/////////////////////////////////////

int upnp_ctrl_point_start(void);
int upnp_ctrl_point_stop(void);
int ctrl_point_refresh(void);
int upnp_ctrl_point_callback_event_handler(Upnp_EventType EventType, void *Event, void *Cookie);


int upnp_discovery_search_result_handler(Upnp_EventType EventType, void *Event, void *Cookie);
int upnp_discovery_byebye_handler(Upnp_EventType EventType, void *Event, void *Cookie);
int upnp_control_action_complete_handler(Upnp_EventType EventType, void *Event, void *Cookie);
int upnp_control_get_var_complete_handler(Upnp_EventType EventType, void *Event, void *Cookie);
int upnp_event_received_handler(Upnp_EventType EventType, void *Event, void *Cookie);
int upnp_event_subscribe_handler(Upnp_EventType EventType, void *Event, void *Cookie);
int upnp_event_subscription_expired_handler(Upnp_EventType EventType, void *Event, void *Cookie);


int ctrl_point_dev_node_operation(DevNodeOperation *dev_node_op);
int dev_node_print(struct UpDeviceNode *devnode);
int dev_node_get_var(int service, struct UpDeviceNode *devnode, const char *varname, int is_lock);
int dev_node_get_ip_info(struct UpDeviceNode *devnode);
int dev_node_get_volume(struct UpDeviceNode *devnode);
int dev_node_get_group_info(struct UpDeviceNode *devnode);
void dev_node_get_var_handler(struct UpDeviceNode *devnode, const char *varName, const DOMString varValue);
int dev_node_add_gst_pipeline(struct UpDeviceNode *devnode);

int ctrl_point_remove_all(void);
void ctrl_point_add_device(IXML_Document *DescDoc, const char *location, int expires);
int ctrl_point_delete_node( struct UpDeviceNode *node );
int ctrl_point_remove_device(const char *UDN);
void ctrl_point_handle_get_var(const char *controlURL, const char *varName, const DOMString varValue);
void ctrl_point_handle_event(const char *sid, int evntkey, IXML_Document *changes);
void ctrl_point_handle_subscribe_update(const char *eventURL, const Upnp_SID sid, int timeout);
void ctrl_point_state_update(char *UDN, int Service, IXML_Document *ChangedVariables, char **State);
int ctrl_point_get_device(int devnum, struct UpDeviceNode **devnode);
char string_to_group_role(const char* string);

char *util_get_element_value(IXML_Element *element);
char *util_get_first_document_item(IXML_Document *doc, const char *item);
char *util_get_first_element_item(IXML_Element *element, const char *item);
int util_find_and_parse_service(IXML_Document *DescDoc, const char *location, const char *serviceType, char **serviceId, char **eventURL, char **controlURL);


#endif /* _UPNP_CONTROL_POINT_H */

