/** @file */

#ifndef IOT_DEVICE_H
#define IOT_DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>  
#include <stdio.h>
#include <time.h> 
#include <stddef.h>  
#include <assert.h>    
    
#include "iothub_client.h" 
#include "platform.h"
#include "threadapi.h"
#include "crt_abstractions.h"
#include "iothubtransportamqp.h"
#include "iothubtransporthttp.h"   
#include "list.h"  
    
#include "iot_device_platform.h"   /* enables\disables lo toolkit platform and logging */ 
    
#include "iot_json_parser.h"       /* open source JSON parsing library */  


/******************************************************************************/
/*                     Configuration Settings                                 */
/******************************************************************************/
    
#ifdef IOT_DEVICE_LO
/** 
 * Enumeration of customization settings.
 */    
enum IOT_DEVICE_LIMITS
{
    IOT_MAX_OUTBOUND_BUFFER_SIZE = 512,         /*!< maximum message payload size */
    IOT_MAX_DEVICE_UUID_SIZE = 37,              /*!< maximum device Id size */
    IOT_MAX_CHANNEL_COUNT = 10,                 /*!< maximum number of channels */
    IOT_MAX_CHANNEL_SIZE = 25,                  /*!< maximum size of channel Id */
    IOT_MAX_CONCURRENT_MESSAGES = 4,            /*!< maximum number of concurrent messages */
    IOT_MAX_MESSAGE_RETRY_COUNT = 3,            /*!< maximum number of retries for failed messages */  
    IOT_MAX_COMMAND_ID_SIZE = 20,               /*!< maximum size of "Device Command" ID (i.e. sequence number) */
    IOT_MAX_COMMAND_METHOD_SIZE = 20,           /*!< maximum size of "Device Command" method name */
    IOT_MAX_COMMAND_PARAMS_SIZE = 32,           /*!< maximum size of "Device Command" parameters */
    IOT_MAX_UNCONFIRMED_MESSAGE_COUNT = 0       /*!< maximum number of unconfirmed messages before additiional device-to-cloud send requests are rejected (Note: zero means no limit)  */                        
};
#else
/** 
 * Enumeration of customization settings.
 */  
enum IOT_DEVICE_LIMITS
{
    IOT_INITIAL_OUTBOUND_BUFFER_SIZE = 1024,    /*!< initial message payload size */        
    IOT_LOW_OUTBOUND_BUFFER_SIZE = 768,         /*!< remaining low buffer size threshold */
    IOT_INCREMENTAL_OUTBOUND_BUFFER_SIZE = 512, /*!< size to incrementally increase message payload buffer */
    IOT_MAX_DEVICE_UUID_SIZE = 37,              /*!< maximum device Id size */ 
    IOT_MAX_CHANNEL_COUNT = 256,                /*!< maximum number of channels */    
    IOT_MAX_MESSAGE_RETRY_COUNT = 3,            /*!< maximum number of retries for failed messages */
    IOT_MAX_UNCONFIRMED_MESSAGE_COUNT = 0       /*!< maximum number of unconfirmed messages before additiional device-to-cloud send requests are rejected (Note: zero means no limit)  */            
};
#endif

/******************************************************************************/
/*                              Data Types                                    */ 
/******************************************************************************/

/*
  List of data types passed in the "a" meta property of the IoT Hub Message.  
*/
static const char TYPE_UNKNOWN[] = "Unknown";
static const char TYPE_DEVICE_TREE[] = "DeviceTree";
static const char TYPE_DEVICES_REALTIME[] = "DevicesRealtimes";
static const char TYPE_REALTIMES[] = "Realtimes";
static const char TYPE_TRENDS[] = "Trends";

/******************************************************************************/
/*                              API TYPES                                     */ 
/******************************************************************************/

/*
  List of API data types in the serialized JSON message.
*/
static const char API_DEVICES[] = "d";
static const char API_DEVICE_REALTIMES[] = "devicerealtimes";
static const char API_REALTIMES[] = "realtimes";
static const char API_TRENDS[] = "trends";

/******************************************************************************/
/*                     CloudToDevice Request Types                            */ 
/******************************************************************************/

/*
   List of CloudToDevice requests. 
*/
static const char REQUEST_DEVICE_TREE[] = "DeviceTreeReq"; 
static const char REQUEST_DEVICE_REALTIMES[] = "DeviceRealtimesReq";
static const char REQUEST_REALTIMES[] = "RealtimesReq";
static const char REQUEST_DEVICE_COMMAND[] = "DeviceCommand";


/******************************************************************************/
/*                              Type Defs                                     */
/******************************************************************************/

/** 
 * Enumeration of device error codes.
 */
typedef enum 
{
    IOT_OK,                             /*!< Operation was successful */ 
    IOT_INVALID_HANDLE,                 /*!< Invalid device handle argument passed to function */
    IOT_INVALID_ARGUMENT,               /*!< Invalid\missing argument passed to function */
    IOT_SEND_ERROR,                     /*!< Error occurred sending message to IoT Hub */
    IOT_NO_AVAILABLE_MESSAGE_BUFFER,    /*!< No available message buffer availavble for sending message to IoT Hub */
    IOT_SERIALIZE_ERROR,                /*!< Error occurred serializing message */
    IOT_PLATFORM_INIT_FAILED,           /*!< Error occurred initializing the platform */
    IOT_CONNECTION_FAILED,              /*!< Error occurred connecting to IoT Hub */
    IOT_THREAD_CREATION_FAILED,         /*!< Error creating message processing thread */
    IOT_SET_MESSAGE_CALLBACK_FAILED,    /*!< Error registering message callback function */
    IOT_DUPLICATE_CONNECTION,           /*!< Connection failed due to duplicate active connection */  
    IOT_SEND_REQUEST_REJECTED           /*!< Maximum number of unconfirmed messages has been reached */        
} IOT_DEVICE_ERROR_CODES;

/** 
 * Enumeration of device connection statuses.
 */
typedef enum 
{
    IOT_UNKNOWN,                        /*!< Unknown connection status */ 
    IOT_OPENED,                         /*!< Connection is active */ 
    IOT_CLOSING,                        /*!< Connection is in the process of closing */         
    IOT_CLOSED,                         /*!< Connection is closed */ 
    IOT_OTHER                           /*!< Other connection status (i.e. possible intermittent communication errors */ 
} IOT_CONNECTION_STATUS_CODES;

/** 
 * Enumeration of data types.  Identifies the type of data in an IOT_DATA structure.
 */
typedef enum 
{
    UNKNOWN_DATA_TYPE,                  /*!< Unknown data type */ 
    DEVICE_TREE,                        /*!< List of devices and child devices */ 
    DEVICES_REALTIME,                   /*!< List of device states */  
    CHANNEL_REALTIMES,                  /*!< List of channel values and status */ 
    TRENDS                              /*!< List of channel time series values */         
} IOT_DATA_TYPE;  

/** 
 * Enumeration of Cloud-to-Device request types.  Identifies the type of request in an IOT_REQUEST structure. 
 */
typedef enum 
{
    UNKNOWN_REQUEST_TYPE,               /*!< Unknown request type */ 
    GET_DEVICE_TREE,                    /*!< Request to publish device list */ 
    GET_DEVICES_REALTIME,               /*!< Request to publish device states */ 
    GET_CHANNEL_REALTIMES,              /*!< Request ot publish channel values and states */ 
    GET_TRENDS,                         /*!< Request to publish channel time series values */
    DEVICE_COMMAND                      /*!< Request to process device command */        
} IOT_REQUEST_TYPE; 

/** 
 * Enumeration of supported IoT Hub transport protocols.
 */
typedef enum 
{
    HTTP,                               /*!< HTTP protocol */ 
    AMQP,                               /*!< AMQP protocol */ 
    AMQP_WEBSOCKETS                     /*!< AMQP over Websockets protocol */ 
} IOT_PROTOCOL;

/*
  Handle to an IoT Hub connection. 
*/ 
typedef IOTHUB_CLIENT_LL_HANDLE IOT_DEVICE_HANDLE;

/*
  Structure containing session information for an IoT Hub connection.  
*/
typedef struct iot_device_connection IOT_DEVICE_CONNECTION;

/** 
 * Structure containing the status of a device connection with IoT Hub.
 * This structure is populated with a call to function "iot_getStatus".
 */
typedef struct iot_connection_status IOT_CONNECTION_STATUS;

/** 
 * Structure containing device connection options used to initialize a
 * connection with IoT Hub.
 * This structure is passed as a parameter to function "iot_open".
 */
typedef struct iot_connection_options IOT_CONNECTION_OPTIONS;

/** 
 * Structure containing one or more data entities (i.e. DeviceTree, DeviceRealtime,
 * ChannelRealtime, Trend etc).
 * This structure is passed as a parameter to the function "iot_send".
 */
typedef struct iot_data IOT_DATA;  

/** 
 * Data structure representing a channel trend value(s).
 */
typedef struct iot_data_trend_item IOT_DATA_TREND_ITEM; 

/** 
 * Data structure representing a device and its optional list of child devices.
 */
typedef struct iot_data_device_item IOT_DATA_DEVICE_ITEM; 

/** 
 * Data structure representing the status and latest value of a channel.
 */
typedef struct iot_data_channel_realtimes_item IOT_DATA_CHANNEL_REALTIMES_ITEM; 

/** 
 * Data structure representing the status of a device.
 */
typedef struct iot_data_devices_realtime_item IOT_DATA_DEVICES_REALTIME_ITEM; 

/** 
 * Structure representing a Device-to-Cloud message.
 * This is an internal data structure used to send messages to IoT Hub.
 */
typedef struct iot_message IOT_MESSAGE; 

/** 
 * Structure representing a Cloud-to-Device request.
 * This structure is passed as a parameter to the "CloudToDevice" callback.
 */
typedef struct iot_request IOT_REQUEST; 

/******************************************************************************/
/*                         Callback Signatures                                */
/******************************************************************************/

/** 
 * Callback function to receive Cloud-to-Device requests.
 * This "C" function pointer is passed as a parameter to the function "iot_registerCloudToDeviceCallback".
 * 
 * @param devicehandle              A handle to the device connection.
 * @param request                   A pointer to the request (refer to iot_request).
 *                                  Note: The memory for the IOT_REQUEST structure will be freed once the callback function exits.
 */
typedef void(*CloudToDeviceCallback)(IOT_DEVICE_HANDLE deviceHandle, const IOT_REQUEST *request);   

/** 
 * Callback function invoked at specified intervals.
 * This "C" function pointer is passed as a parameter to the function "iot_registerTimerCallback".
 * 
 * @param devicehandle              A handle to the device connection.
 */
typedef void (*TimerCallback)(IOT_DEVICE_HANDLE deviceHandle);  

/******************************************************************************/
/*                       Cloud-to-Device Request                              */
/******************************************************************************/

#ifdef IOT_DEVICE_LO

/** 
 * Structure representing a Cloud-to-Device request.
 * This structure is passed as a parameter to the "CloudToDevice" callback.
 */
struct iot_request 
{
    IOT_REQUEST_TYPE requestType;                                /*!< Request Type (i.e. GetDeviceTree, GetChannelRealtimes etc) */ 
    char deviceUUID[IOT_MAX_DEVICE_UUID_SIZE];                   /*!< Device Id in GUID format: 00000000-0000-0000-0000-000000000000 */
    int channelCount;                                            /*!< Number of channels in channel list.  Only applicable if the request type is: GET_CHANNEL_REALTIMES. */
    char channels[IOT_MAX_CHANNEL_COUNT][IOT_MAX_CHANNEL_SIZE];  /*!< List of channels */
    /* DeviceCommand properties */
    char commandID[IOT_MAX_COMMAND_ID_SIZE];                     /*!< String to identify the command instance (i.e. sequence number). This generated ID is passed in from the cloud. */  
    char commandDeviceUUID[IOT_MAX_DEVICE_UUID_SIZE];            /*!< Device UUID command is targeting.  Only applicable if the request type is: DEVICE_COMMAND. */
    char commandMethod[IOT_MAX_COMMAND_METHOD_SIZE];             /*!< Command method name (i.e. SetChannelValue).  Only applicable if the request type is: DEVICE_COMMAND. */
    char commandParams[IOT_MAX_COMMAND_PARAMS_SIZE];             /*!< Command parameters in JSON format (i.e. {"tag":"<channel-tag>","v":"<channel-value>"} ).  Only applicable if the request type is: DEVICE_COMMAND. */    
};

#else

/** 
 * Structure representing a Cloud-to-Device request.
 * This structure is passed as a parameter to the "CloudToDevice" callback.
 */
struct iot_request 
{
    IOT_REQUEST_TYPE requestType;                                /*!< Request Type (i.e. GetDeviceTree, GetChannelRealtimes etc) */ 
    char *deviceUUID;                                            /*!< Device Id in GUID format: 00000000-0000-0000-0000-000000000000 */    
    int channelCount;                                            /*!< Number of channels in channel list.  Only applicable if the request type is: GET_CHANNEL_REALTIMES. */
    char **channels;                                             /*!< List of channels */
    /* DeviceCommand properties */
    char *commandID;                                             /*!< String to identify the command instance (i.e. sequence number).This generated ID is passed in from the cloud. */     
    char *commandDeviceUUID;                                     /*!< Device UUID command is targeting.  Only applicable if the request type is: DEVICE_COMMAND. */
    char *commandMethod;                                         /*!< Command method name (i.e. SetChannelValue).  Only applicable if the request type is: DEVICE_COMMAND. */
    char *commandParams;                                         /*!< Command parameters in JSON format (i.e. {"tag":"<channel-tag>","v":"<channel-value>"} ).Only applicable if the request type is: DEVICE_COMMAND. */
};

#endif

/******************************************************************************/
/*                Device-to-Cloud Message (Internal)                          */ 
/******************************************************************************/

/** 
 * Structure representing a Device-to-Cloud message.
 * This is an internal data structure used to send messages to IoT Hub.
 */
struct iot_message {
    IOTHUB_MESSAGE_HANDLE messageHandle;    /*!< Handle to IoT Hub message */ 
    void *connection;                       /*!< Pointer to message context */ 
    unsigned long messageId;                /*!< Unique Id assigned to the message */
    int retryCount;                         /*!< Number of failed message resend attempts */     
#ifdef IOT_DEVICE_LO
    char serializedMessage[IOT_MAX_OUTBOUND_BUFFER_SIZE]; /*!< Serialized message payload */
    bool inUse;                             /*!< Flag indicating if message buffer is in use (i.e. message sent but not confirmed). */ 
#else
    int serializedBufferSize;               /*!< Length of message */
    char *serializedMessage;                /*!< Serialized message payload */
#endif
};
    
/******************************************************************************/
/*                     IOT Device Connection (Internal)                       */
/******************************************************************************/

#ifdef IOT_DEVICE_LO
struct iot_device_connection {
    char deviceUUID[IOT_MAX_DEVICE_UUID_SIZE];      /* Device Id in GUID format: 00000000-0000-0000-0000-000000000000 */    
    IOTHUB_CLIENT_LL_HANDLE handle;                 /* A handle to the IoT Hub device connection. */
    CloudToDeviceCallback c2dCallback;              /* Address of "C" CloudToDevice function callback */ 
    TimerCallback timerCallback;                    /* Address of "C" TIMER function callback */ 
    int timerCallbackInterval;                      /* Timer callback interval (in seconds) */ 
    THREAD_HANDLE messageThreadId;                  /* Thread Id of message thread */
    bool close;                                     /* Function "iot_close" has been invoked */ 
    IOT_CONNECTION_STATUS_CODES connectionStatus;   /* Status of IoT Hub connection */
    long msgOutbound;                               /* Number of Device-to-Cloud messages sent to IoT Hub */
    long msgConfirmed;                              /* Number of confirmed Device-to-Cloud messages sent to IoT Hub */
    long msgInbound;                                /* Number of Cloud-to-Device messages received */
    time_t lastTimerCallbackInvoked;                /* Last time the "TIMER" callback was invoked */
    IOT_REQUEST iotRequest;                         /* Cloud-to-Device request */
    IOT_MESSAGE iotMessage[IOT_MAX_CONCURRENT_MESSAGES];  /* List of message buffers */
    IOT_DEVICE_ERROR_CODES lastErrorCode;           /* last SDK error code */
    int unconfirmedMessageCount;                    /* number of unconfirmed sent messages (consecutive) */     
};  
#else
struct iot_device_connection {
    char *deviceUUID;                               /* Device Id in GUID format: 00000000-0000-0000-0000-000000000000 */   
    IOTHUB_CLIENT_LL_HANDLE handle;                 /* A handle to the IoT Hub device connection. */
    CloudToDeviceCallback c2dCallback;              /* Address of "C" CloudToDevice function callback */
    TimerCallback timerCallback;                    /* Address of "C" TIMER function callback */ 
    int timerCallbackInterval;                      /* Timer callback interval (in seconds) */ 
    THREAD_HANDLE messageThreadId;                  /* Thread Id of message thread */
    bool close;                                     /* Function "iot_close" has been invoked */ 
    IOT_CONNECTION_STATUS_CODES connectionStatus;   /* Status of IoT Hub connection */
    long msgOutbound;                               /* Number of Device-to-Cloud messages sent to IoT Hub */
    long msgConfirmed;                              /* Number of confirmed Device-to-Cloud messages sent to IoT Hub */
    long msgInbound;                                /* Number of Cloud-to-Device messages received */
    time_t lastTimerCallbackInvoked;                /* Last time the "TIMER" callback was invoked */
    IOT_REQUEST iotRequest;                         /* Cloud-to-Device request */
    IOT_DEVICE_ERROR_CODES lastErrorCode;           /* last SDK error code */
    int unconfirmedMessageCount;                    /* number of unconfirmed sent messages (consecutive) */   
    
};
#endif

/******************************************************************************/
/*              Device SDK Serialization Methods (Internal)                   */
/******************************************************************************/

/* 
   Serialize data entities to Json to be transmitted from Device-to-Cloud.

   @param connection                A pointer to the device connection.
   @param iotMessage                A pointer to the message to be transmitted to IoT Hub.
   @param data                      The data entities to be serialized (refer to iot_data).
 
   @return bool                     True if serialization was successful else False.
*/
extern bool serialize(IOT_DEVICE_CONNECTION *connection, IOT_MESSAGE *iotMessage, const IOT_DATA *data); 

/* 
   Serialize list of devices to Json.

   @param iotMessage                A pointer to the message to be transmitted to IoT Hub.
   @param data                      The data entities to be serialized (refer to iot_data).
 
   @return bool                     True if serialization was successful else False.
*/
extern bool serializeDeviceTreeJson(IOT_MESSAGE *iotMessage, const IOT_DATA *data); 

/* 
   Serialize a single device to Json.

   @param iotMessage                A pointer to the message to be transmitted to IoT Hub.
   @param dataDevice                The device data entity to serialize.
   @param firstDevice               Flag indicating whether this is the first device in the list.
 
   @return bool                     True if serialization was successful else False.
*/
extern bool serializeDeviceJson(IOT_MESSAGE *iotMessage, const IOT_DATA_DEVICE_ITEM *dataDevice, bool firstDevice); 

/* 
   Serialize a list of device states to Json.

   @param iotMessage                A pointer to the message to be transmitted to IoT Hub.
   @param data                      The data entities to be serialized (refer to iot_data).
 
   @return bool                     True if serialization was successful else False.
*/
extern bool serializeDevicesRealtimeJson(IOT_MESSAGE *iotMessage, const IOT_DATA *data); 

/* 
   Serialize device state for a single device to Json.

   @param iotMessage                A pointer to the message to be transmitted to IoT Hub.
   @param data                      The device realtime data entity to serialize.
   @param firstItem                 Flag indicating whether this is the first data item in the list.
 
   @return bool                     True if serialization was successful else False.
*/
extern bool serializeDeviceRealtimeJson(IOT_MESSAGE *iotMessage, const IOT_DATA_DEVICES_REALTIME_ITEM *data, bool firstItem);

/* 
   Serialize a list of channel states and values to Json.

   @param iotMessage                A pointer to the message to be transmitted to IoT Hub.
   @param data                      The data entities to be serialized (refer to iot_data).
 
   @return bool                     True if serialization was successful else False.
*/
extern bool serializeChannelRealtimesJson(IOT_MESSAGE *iotMessage, const IOT_DATA *data);

/* 
   Serialize a single channel state and value to Json.

   @param iotMessage                A pointer to the message to be transmitted to IoT Hub.
   @param deviceUUID                The Device Id in GUID format: 00000000-0000-0000-0000-000000000000.
   @param data                      The Channel Realtime data entity to serialize. 
   @param firstItem                 Flag indicating whether this is the first data item in the list.
 
  @return bool                     True if serialization was successful else False.
*/
extern bool serializeChannelRealtimeJson(IOT_MESSAGE *iotMessage, const char *deviceUUID, const IOT_DATA_CHANNEL_REALTIMES_ITEM *data, bool firstItem);

/* 
   Serialize timeseries data for a list of channels to Json.

   @param iotMessage                A pointer to the message to be transmitted to IoT Hub.
   @param data                      The data entities to be serialized (refer to iot_data).
 
   @return bool                     True if serialization was successful else False.
*/
extern bool serializeTrendsJson(IOT_MESSAGE *iotMessage, const IOT_DATA *data); 

/* 
   Serialize timeseries data for a single channel to Json.

   @param iotMessage                A pointer to the message to be transmitted to IoT Hub.
   @param device UUID               The Device Id in GUID format: 00000000-0000-0000-0000-000000000000. 
   @param data                      The trend data entity to serialize.
   @param firstItem                 Flag indicating whether this is the first data item in the list.
 
   @return bool                     True if serialization was successful else False.
*/
extern bool serializeTrendJson(IOT_MESSAGE *iotMessage, const char *deviceUUID, const IOT_DATA_TREND_ITEM *data, bool firstItem);

/*
   Append a string value to a Json buffer.

   @param buffer                A pointer to a character string to append the value to.
   @param name                  The name of the field.
   @param value                 The value of the field.
   @param includeTrailingComma  Flag indicating whether a trailing comma should be added to the buffer.
*/
extern void addStringField(char *buffer, const char* name, const char *value, bool includeTrailingComma);

/*
   Append a numeric (int) value to a Json buffer.

   @param buffer                A pointer to a character string to append the value to.
   @param name                  The name of the field.
   @param value                 The value of the field.
   @param includeTrailingComma  Flag indicating whether a trailing comma should be added to the buffer.
*/
extern void addIntNumericField(char *buffer, const char* name, const int* value, bool includeTrailingComma);

/*
   Append a numeric (long) value to a Json buffer.

   @param buffer                A pointer to a character string to append the value to.
   @param name                  The name of the field.
   @param value                 The value of the field.
   @param includeTrailingComma  Flag indicating whether a trailing comma should be added to the buffer.
*/
extern void addLongNumericField(char *buffer, const char* name, const long* value, bool includeTrailingComma);

/*
   Append a boolean value to a Json buffer.

   @param buffer                A pointer to a character string to append the value to.
   @param name                  The name of the field.
   @param value                 The value of the field.
   @param includeTrailingComma  Flag indicating whether a trailing comma should be added to the buffer.
*/
extern void addBooleanField(char *buffer, const char* name, const bool *value, bool includeTrailingComma);

/* 
   Deserialize a Cloud-to-Device Json request.

   @param connection                A pointer to the device connection.
   @param msg                       Pointer to character string containing the request.
   @param len                       Length of request message. 
   @param request                   Pointer to IOT_REQUEST structure to be populated representing the Cloud-to-Device request.
*/
extern void deserialize(IOT_DEVICE_CONNECTION *connection, const char *msg, int len, IOT_REQUEST *request);

/* 
   Deserialize a Cloud-to-Device Json Get Device Tree request.

   @param msg                       Pointer to character string containing the request.
   @param len                       Length of message request. 
   @param request                   Pointer to IOT_REQUEST structure to be populated representing the request.
*/
extern void deserializeGetDeviceTreeJson(const char *msg, int len, IOT_REQUEST *request);

/* 
   Deserialize a Cloud-to-Device Json Get Device Realtime request.

   @param msg                       Pointer to character string containing the request.
   @param len                       Length of message request.  
   @param request                   Pointer to IOT_REQUEST structure to be populated representing the Cloud-to-Device request.
*/
extern void deserializeGetDevicesRealtimeJson(const char *msg, int len, IOT_REQUEST *request);

/* 
   Deserialize a Cloud-to-Device Json Get Channel Realtime request.

   @param msg                       Pointer to character string containing the request.
   @param len                       Length of message request.  
   @param request                   Pointer to IOT_REQUEST structure to be populated representing the Cloud-to-Device request.
*/
extern void deserializeGetChannelRealtimesJson(const char *msg, int len, IOT_REQUEST *request);

/* 
   Deserialize a Cloud-to-Device Json Device Command request.

   @param msg                       Pointer to character string containing the request.
   @param len                       Length of message request.  
   @param request                   Pointer to IOT_REQUEST structure representing the Cloud-to-Device request.
*/
extern void deserializeDeviceCommandJson(const char *msg, int len, IOT_REQUEST *request);


/*
    Validates a Cloud-to-Device Device Command for proper content.  
  
    @param request                  Pointer to IOT_REQUEST structure representing the Cloud-To-Device Device Command.  
  
    @return bool                    True if Device Command is valid else False.
*/
extern bool validateDeviceCommand(IOT_REQUEST *request);

#ifndef IOT_DEVICE_LO
/*
   Allocates memory for a serialized message buffer.
  
   @param iotMessage                A pointer to the message to be transmitted to IoT Hub. 
  
   @return bool                     True if memory was successfully allocated else False. 
*/
extern bool allocateSerializedBuffer(IOT_MESSAGE *iotMessage);

/*
   Checks if memory for a serialized message buffer needs to be reallocated.

   @param iotMessage                A pointer to the message to be transmitted to IoT Hub.
   @param dataEntitySize            Size of data entity to be serialized and added to the message buffer.  
  
   @return bool                     True if buffer size was sufficient or memory was successfully reallocated else False.  
*/
extern bool checkSerializedBufferSize(IOT_MESSAGE *iotMessage, int dataEntitySize);
#endif

/******************************************************************************/
/*                  Device SDK Misc Methods (Internal)                        */
/******************************************************************************/

/*
  Create a list of connections. 
*/
extern void createConnectionList();

/*
   Returns an IOT_DEVICE_CONNECTION associated with a IOT_DEVICE_HANDLE.
*/
extern IOT_DEVICE_CONNECTION* getConnection(IOT_DEVICE_HANDLE deviceHandle);

/*
   Get a handle to the list of connections; 
*/
extern LIST_HANDLE getConnectionList();

/*
   Returns the number of active IoT Hub connections.
*/
extern int getConnectionCount();

/*
   Determines if a connection associated with a Device UUID exists.
*/
extern bool connectionExists(const char *deviceUUID);

/*
   Deletes an IOT_DEVICE_CONNECTION and associated memory.
*/
extern void deleteConnection(IOT_DEVICE_CONNECTION *connection);

/*
   Looks up the data type associated with a data entity structure. 
*/
extern const char* getDataType(const IOT_DATA *data);

/*
   Validates the contents of IOT_CONNECTION_OPTIONS. 
*/
extern bool validateConnectionOptions(const IOT_CONNECTION_OPTIONS *connectionOptions, int *status);

/*
   Initializes global variables. 
*/
extern void initializeGlobalVariables();

/*
   Performs platform initialization. 
*/
extern bool initializePlatform(int *status);

/*
  Initializes an IoT Hub connection.  
*/
extern IOT_DEVICE_CONNECTION* initializeConnection(IOTHUB_CLIENT_LL_HANDLE handle, const IOT_CONNECTION_OPTIONS *connectionOptions);

/*
   Creates an IoT Hub message 
*/
extern IOT_MESSAGE* createMessage(IOT_DEVICE_CONNECTION *connection, const IOT_DATA *data, int *messageIndex);

/*
   Sets Iot Hub message properties. 
*/
extern void setMessageProperties(IOT_DEVICE_CONNECTION *connection, IOT_MESSAGE *iotMessage, int messageIndex, const IOT_DATA *data);


/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/*                       Device SDK Interface                                 */
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/


/******************************************************************************/
/*                            Structures                                      */
/******************************************************************************/

/******************************************************************************/
/*                  IOT Device Connection Options                             */
/******************************************************************************/

/** 
 * Structure containing device connection options used to initialize a
 * connection with IoT Hub.
 * This structure is passed as a parameter to function "iot_open".
 */
struct iot_connection_options {
    IOT_PROTOCOL protocol;        /*!< Communication protocol (HTTP,AMQP,AMQP_WEBSOCKETS) */ 
    const char *deviceUUID;       /*!< Device Id in GUID format: 00000000-0000-0000-0000-000000000000 */
    const char *connectionString; /*!< Connection string to IoT Hub instance (format: HostName=<name>;DeviceId=<00000000-0000-0000-0000-000000000000>;SharedAccessKey=<key> ) */  
};    

/******************************************************************************/
/*                  IOT Device Connection Status                              */
/******************************************************************************/

/** 
 * Structure containing the status of a device connection with IoT Hub.
 * This structure is populated with a call to function "iot_getStatus".
 */
struct iot_connection_status {
    IOT_CONNECTION_STATUS_CODES connectionStatus; /*!< IoT Hub connection status (refer to iot_connection_status_codes) */    
    long msgOutbound;                             /*!< Number of messages sent to IoT Hub */
    long msgConfirmed;                            /*!< Number of message confirmations received from IoT Hub */
    long msgInbound;                              /*!< Number of messages received from IoT Hub */
    IOT_DEVICE_ERROR_CODES lastErrorCode;         /*!< Last SDK error code (refer to iot_device_error_codes) */ 
};   
 
/******************************************************************************/
/*                        IOT Device Data Entities                            */ 
/******************************************************************************/

/** 
 * Structure containing one or more data entities (i.e. DeviceTree, DeviceRealtime,
 * ChannelRealtime, Trend etc).
 * This structure is passed as a parameter to the function "iot_send".
 */
struct iot_data 
{
    IOT_DATA_TYPE dataType;       /*!< Type of data entity (refer to iot_data_type) */ 
    const char *deviceUUID;       /*!< Device Id in GUID format: 00000000-0000-0000-0000-000000000000 or NULL */
    LIST_HANDLE devices;          /*!< Handle to list of device entities (refer to iot_data_device_item) or NULL if dataType doesn't equal DEVICE_TREE */
    LIST_HANDLE devicesRealtime;  /*!< Handle to list of device realtime entities (refer to iot_data_devices_realtime_item) or NULL if dataType doesn't equal DEVICES_REALTIME */ 
    LIST_HANDLE channelRealtimes; /*!< Handle to list of channel realtime entities (refer to iot_data_channel_realtimes_item) or NULL if dataType doesn't equal CHANNEL_REALTIMES */ 
    LIST_HANDLE trends;           /*!< Handle to list of trend entities (refer to iot_data_trend_item) or NULL if dataType doesn't equal TRENDS */
};    

/** 
 * Data structure representing a device and its optional list of child devices.
 */
struct iot_data_device_item
{
    const char *deviceUUID;           /*!< Device Id in GUID format: 00000000-0000-0000-0000-000000000000 */
    const char *profile;              /*!< Device Profile id in GUID format: 00000000-0000-0000-0000-000000000000 */
    const char *name;                 /*!< (optional) User friendly name for the device or NULL */
    const char *serial;               /*!< (optional) Device serial number or NULL */
    const char *assetTag;             /*!< (optional) Asset name associated with the device or NULL */
    const char *mac;                  /*!< (optional) Device mac address or NULL */
    LIST_HANDLE subDevices;           /*!< (optional) List of child devices or NULL */
};

/** 
 * Data structure representing the status of a device.
 */
struct iot_data_devices_realtime_item
{
    const char *deviceUUID;           /*!< Device Id in GUID format: 00000000-0000-0000-0000-000000000000 */
    long time;                        /*!< Number of seconds since the Epoch (January 1st, 1970) when this update is captured or zero if not available */
    int elapsedms;                    /*!< Number of milliseconds elapsed since the last second boundary or zero if not available */
    bool disconnected;                /*!< False if the device is not disconnected */
    bool disabled;                    /*!< False if the device is not disabled */
    bool disarmed;                    /*!< False if the device is not disarmed */
};

/** 
 * Data structure representing the status and latest value of a channel.
 */
struct iot_data_channel_realtimes_item
{
    const char *channelTag;           /*!< Assigned channel Tag */
    long time;                        /*!< Number of seconds since the Epoch (January 1st, 1970) when this update is captured or zero if not available */
    int milliseconds;                 /*!< Number of milliseconds elapsed since the last second boundary or zero if not available */
    const char *value;                /*!< Represents the channel's measurement value in string format */
    bool  disconnected;               /*!< False if the channel is not disconnected */
    bool disabled;                    /*!< False if the channel is not disabled */
    bool disarmed;                    /*!< False if the channel is not disarmed */
};

/** 
 * Data structure representing a channel trend value(s).
 */
struct iot_data_trend_item
{
    const char *channelTag;           /*!< Assigned channel Tag */
    long time;                        /*!< Number of seconds since the Epoch (January 1st, 1970) when this update is captured or zero if not available */
    const char *actValue;             /*!< (optional) The channel's actual value (on that time interval) in string format or NULL if it dosen't exist */
    const char *avgValue;             /*!< (optional) The channel's avg value during that time interval in string format or NULL if it doesn't exist */  
    const char *minValue;             /*!< (optional) The channel's min value during that time interval in string format or NULL if it dosen't exist */
    const char *maxValue;             /*!< (optional) The channel's max value during that time interval in string format or NULL it it doesn't exist */
};

/******************************************************************************/
/*                               Functions                                    */
/******************************************************************************/

/** 
 * Opens a connection with IoT Hub.
 * Each device connecting to IoT Hub must first be registered and assigned a unique
 * device UUID in the form: 00000000-0000-0000-0000-000000000000.
 * Once registered, a device will be assigned a unique connection string.  
 *
 * @param connectionOptions             Device connection options\settings (refer to iot_connection_options).
 * @param status                        The status of the open request (refer to iot_device_error_codes). 
 * 
 * @return IOT_DEVICE_HANDLE            A handle to the device connection or NULL if the
 *                                      connection failed.
 */
extern IOT_DEVICE_HANDLE iot_open(const IOT_CONNECTION_OPTIONS *connectionOptions, int *status);  

/** 
 * Closes a connection with IoT Hub.
 * After a connection has been closed, no further messages will be sent or received
 * from IoT Hub.
 *
 * @param deviceHandle              A handle to the device connection.
 * 
 * @return bool                     True if the connection successfully closed else False.
 */
extern bool iot_close(IOT_DEVICE_HANDLE deviceHandle);  

/** 
 * Registers a CloudToDevice callback function for processing Cloud-To-Device requests.
 * Only one "CloudToDevice" callback can be registered per device connection. 
 *
 * @param deviceHandle              A handle to the device connection.
 * @param callback                  A pointer to the "C" callback function.
 * 
 * @return bool                     True if callback was successfully registered else False.
 */ 
extern bool iot_registerCloudToDeviceCallback(IOT_DEVICE_HANDLE deviceHandle, CloudToDeviceCallback callback);

/** 
 * Registers a TIMER callback function to be invoked at set intervals.
 * Only one "Timer" callback can be registered per device connection. 
 *
 * @param deviceHandle              A handle to the device connection.
 * @param interval                  The interval at which the callback will be invoked (seconds).
 * @param callback                  A pointer to the "C" callback function.
 * 
 * @return bool                     True if callback was successfully registered else False.
 */ 
extern bool iot_registerTimerCallback(IOT_DEVICE_HANDLE deviceHandle, int interval, TimerCallback callback);

/** 
 * UnRegisters the CloudToDevice callback.
 *
 * @param deviceHandle              A handle to the device connection.
 * 
 * @return bool                     True if callback was successfully unregistered else False.
 */ /*
   Sends a Device-To-Cloud message. 
*/
extern bool iot_unRegisterCloudToDeviceCallback(IOT_DEVICE_HANDLE deviceHandle);

/** 
 * UnRegisters the TIMER callback.
 *
 * @param deviceHandle              A handle to the device connection.
 * 
 * @return bool                     True if callback was successfully unregistered else False.
 */ 
extern bool iot_unRegisterTimerCallback(IOT_DEVICE_HANDLE deviceHandle);

/** 
 * Sends data to IoT Hub (i.e. DeviceTree, DeviceRealtime, 
 * ChannelRealtime, Trends etc).
 * This is a non-blocking call.
 *
 * @param deviceHandle              A handle to the device connection.
 * @param data                      A list of one or more data entity values (refer to iot_data). 
 *                                  Note: the caller is responsible for freeing the IOT_DATA structure.                                                     
 * 
 * @return bool                     True if request was successfully processed else False.
 */ 
extern bool iot_send(IOT_DEVICE_HANDLE deviceHandle, const IOT_DATA *data);

/** 
 * Gets the status of the connection. 
 *
 * @param deviceHandle              A handle to the device connection.
 * @param connectionStatus          A structure containing the connection status (refer to iot_connection_status).
 *                                  Note: The passed in address of the connection status is
 *                                  populated with the latest connection information.                    
 * 
 * @return bool                     True if connection status was successfully retrieved else False.
 */ 
extern bool iot_getStatus(IOT_DEVICE_HANDLE deviceHandle, IOT_CONNECTION_STATUS *connectionStatus);

/**
 * Creates a list of IOT_DATA items (refer to iot_data).
 * 
 * @return LIST_HANDLE           A handle to the list.               
 */
extern LIST_HANDLE iot_list_create();

/**
 * Destroys a list of structures representing a linked list of IOT_DATA items (refer to iot_data).
 * Memory for the list of LIST_ITEM_INSTANCE structures is freed.
 * The caller is responsible for freeing memory for the contents of the LIST_ITEM_INSTANCE(s).  
 * 
 * LIST_ITEM_INSTANCE
 *                      struct LIST_ITEM_INSTANCE
 *                      {
 *                        const void* item;   
 *                        void* next;
 *                      }  
 *  
 * @param listHandle             A handle to the list.
 *  
 * @return void 
 */
extern void iot_list_destroy(LIST_HANDLE listHandle);

/**
 * Adds a new item to a list of IOT_DATA items (refer to iot_data).
 * 
 * @param listHandle             A handle to the list.
 * @param item                   A pointer to an iot_data item.
 *  
 * @return LIST_ITEM_HANDLE      A handle to the list item. 
 */
extern LIST_ITEM_HANDLE iot_list_add(LIST_HANDLE listHandle, const void *item);

/**
 * Removes an IOT_DATA item from a list (refer to iot_data).
 * 
 * @param listHandle             A handle to the list.
 * @param itemHandle             A handle to the list item.
 *  
 * @return void                   
 */
extern void iot_list_remove(LIST_HANDLE listHandle, LIST_ITEM_HANDLE itemHandle);

/**
 * Retrieves a handle to the first item in a list of IOT_DATA items (refer to iot_data).
 * 
 * @param listHandle             A handle to the list.
 *  
 * @return LIST_ITEM_HANDLE      A handle to the first item in the list or NULL if list is empty.                   
 */
extern LIST_ITEM_HANDLE iot_list_get_head_item(LIST_HANDLE listHandle);

/**
 * Retrieves a handle to the next item in a list of IOT_DATA items (refer to iot_data).
 * 
 * @param itemHandle             A handle to the list item.
 *  
 * @return LIST_ITEM_HANDLE      A handle to the next item in the list or NULL if the end of the list has been reached.                   
 */
extern LIST_ITEM_HANDLE iot_list_get_next_item(LIST_ITEM_HANDLE itemHandle);

/**
 * Retrieves a IOT_DATA item from a list of IOT_DATA items (refer to iot_data).
 * 
 * @param itemHandle             A handle to the list item.
 *  
 * @return IOT_DATA              A pointer to an IOT_DATA item or NULL if the item does not exist.                   
 */
extern const void* iot_list_item_get_value(LIST_ITEM_HANDLE itemHandle);

#ifdef __cplusplus
}
#endif

#endif /* IOT_DEVICE_H */

