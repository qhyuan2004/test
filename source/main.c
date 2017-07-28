#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "iot_device.h"
#include "globals.h"

int main(int argc, char** argv) {

    printf("\n\n >> --------- Starting Eaton IoT Device Client Sample App  --------- << \n\n");

    int exitCode = 0;


    /** Initialize global application variables with sample or used-defined data.
     * If the useSampleData flag is true (default),
     * the predefined, sample data will be used. If useSampleData flag is false,
     * the user-defined variables and their respective values will be used.
     */
    setGlobals();


    /** EXAMPLE: ESTABLISHING CONNECTIVITY TO THE IOT HUB
     Attempt to establish connectivity with an instance of an Azure IoT Hub
     per sample or user-defined connection string defined in globals.h

     IOT_DEVICE_HANDLE is an instance of the device handle from which
     subsequent communication between the configured, simulated client device(s)
     and IoT Hub will occur.
     */
    IOT_CONNECTION_OPTIONS connectionOptions;
    connectionOptions.protocol = HTTP;
    connectionOptions.deviceUUID = deviceUUID;
    connectionOptions.connectionString = connectionString;

    int status = 0;
    IOT_DEVICE_HANDLE deviceHandle = iot_open(&connectionOptions, &status);

    /** EXAMPLE: PUBLISH DEVICE \ DEVICE TREE
     * Now that we've established connectivity, let's prepare and publish the
     * sample or user-defined via IoT Hub device handle, as defined in globals.h,
     * which will allow\enable subsequent actions such as: publishing trend data,
     * real-time device communication, real-time device channel communication
     */
    printf("\n\n >> --------- PUBLISH DEVICE START ---------------------- \n");
    printf("\n\n >> --------- Associating Device With Connection Handle --------- << \n\n");

    IOT_DATA_DEVICE_ITEM device;

    /* Create a multi-purpose IOT_DATA to house DEVICE_TREE type IoT data */
    IOT_DATA deviceMetadata;
    deviceMetadata.dataType = DEVICE_TREE;
    deviceMetadata.devices = iot_list_create();

    /* Set the device's metadata per the respective global variable values */
    device.deviceUUID = deviceUUID;
    device.profile = deviceProfileUUID;
    device.name = deviceName;
    device.serial = deviceSerialNumber;
    device.assetTag = deviceAssetTag;
    device.mac = deviceMAC;
	
    device.subDevices =  iot_list_create();



    IOT_DATA_DEVICE_ITEM subdevice;

    /* Create a multi-purpose IOT_DATA to house DEVICE_TREE type IoT data */
    IOT_DATA subdeviceMetadata;
    subdeviceMetadata.dataType = DEVICE_TREE;
    subdeviceMetadata.devices = iot_list_create();

    /* Set the device's metadata per the respective global variable values */


//    subdevice.deviceUUID = "b9b02e15-8eab-5fbb-9fc1-458ca5cc9b83";
    subdevice.deviceUUID = "ce04ec44-b246-564e-ad4d-3cc4ee19cc6d";

    subdevice.profile = "c655b09e-bc8b-11e6-a4a6-cec0c932ce01";
    subdevice.name = "sub_simulated";
    subdevice.serial = "sub seri";
    subdevice.assetTag = "sub deviceAssetTag";
    subdevice.mac = "sub deviceMAC";
    subdevice.subDevices = NULL; 
	

    /** Add the populated device item to a list of devices
     * (to contain only one device for this example),
     * which will be associated w/the connection handle.
     */
    iot_list_add(device.subDevices, &subdevice);

    iot_list_add(deviceMetadata.devices, &device);

//	 iot_list_add(deviceMetadata.devices.subDevices, $subdevice);

    /** Functionality exists to facilitate building a network\device tree.
     * For demo purposes, we'll only create a placeholder list for sub-devices,
     * which again is not required.
     */
    //device.subDevices = NULL;

    /** Send the device's (or device tree's) meta from the device to the IoT Hub
     * NOTE: iot_send is part of the Eaton IoT Device SDK
     * and is used to send messages from the Device to the IoT Hub
     * via the pre-established connection to the IoT Hub.
     */
    iot_send(deviceHandle, &deviceMetadata);

    /** Perform cleanup by:
        1) removing all list items, to include any sublist items, previously created
        2) destroy the list(s) and sublist(s)
     */
    iot_list_destroy(deviceMetadata.devices);

    printf("\n\n >> --------- PUBLISH DEVICE END --------- << \n\n");


    /** EXAMPLE: PUBLISH DEVICE CHANNEL\POINT TREND DATA
     * Now that we've published the device via the connection handle,
     * let's prepare and publish sample trend data for one of its channels\points
     */
    printf("\n\n >> --------- PUBLISH DEVICE CHANNEL\\POINT TREND DATA START --------- << \n\n");

    printf("\n\n >> --------- Prepare and Send Trend Data Point from Device --------- << \n\n");

    IOT_DATA trendData;
    trendData.dataType = TRENDS;
    trendData.deviceUUID = (char*) subdevice.deviceUUID;
    trendData.trends = iot_list_create();

    time_t  tt = time(NULL);

    /* Set trend value(s) */
    IOT_DATA_TREND_ITEM trendItem;
    trendItem.channelTag = channelTag;
    trendItem.time = tt; //1465169399;
    trendItem.minValue = "1.0";
    trendItem.maxValue = "999.0";
    trendItem.actValue = "456.0";
    trendItem.avgValue = "500.0";

    LIST_ITEM_HANDLE* trendItems = (LIST_ITEM_HANDLE*) malloc(sizeof (LIST_ITEM_HANDLE) * maxDevicesSupported);
    trendItems[0] = iot_list_add(trendData.trends, &trendItem);

    /** Send the trend data point, for the specified channel, from the device to the IoT Hub
     * NOTE: iot_send is part of the Eaton IoT Device SDK
     * and is used to send messages from the Device to the IoT Hub
     * via the pre-established connection to the IoT Hub.
     */
    iot_send(deviceHandle, &trendData);

    /* Remove trend list items, destroy trend-related objects and free allocated memory */
    iot_list_destroy(trendData.trends);
    free(trendItems);

    printf("\n\n >> --------- PUBLISH DEVICE CHANNEL\\POINT TREND DATA END --------- << \n \n");

    /** EXAMPLE: PUBLISH DEVICE REAL-TIME DATA
    Now that we've published the device via the connection handle,
    let's publish a device specific real-time message */
    printf("\n\n >> --------- PUBLISH DEVICE REAL-TIME DATA START --------- << \n\n");

    printf("\n\n >> --------- Prepare and Send Device Real-time Data from Device --------- << \n\n");
    /* Define device's real-time items  */
    IOT_DATA_DEVICES_REALTIME_ITEM deviceRealtime = {deviceUUID, 0, 0, false, false, false};

    /* Build the device's real-time message */
    IOT_DATA deviceRealtimeData;
    deviceRealtimeData.dataType = DEVICES_REALTIME;
    deviceRealtimeData.devicesRealtime = iot_list_create();

    iot_list_add(deviceRealtimeData.devicesRealtime, &deviceRealtime);


    /** Send the real-time message from the device to the IoT Hub
     * NOTE: iot_send is part of the Eaton IoT Device SDK
     * and is used to send messages from the Device to the IoT Hub
     * via the pre-established connection to the IoT Hub.
     */
    iot_send(deviceHandle, &deviceRealtimeData);

    /* Remove list items, destroy objects */
    iot_list_destroy(deviceRealtimeData.devicesRealtime);

    printf("\n\n >> --------- PUBLISH DEVICE REAL-TIME DATA END --------- << \n\n");

    /** EXAMPLE: PUBLISH DEVICE CHANNEL \\ POINT REAL-TIME DATA
     * Now that we've published the device via the connection handle,
     * let's publish a real-time message for one of the device's channels\points
     */
    printf("\n\n >> --------- PUBLISH DEVICE CHANNEL\\POINT REAL-TIME DATA START --------- << \n\n");

    printf("\n\n >> --------- Prepare and Send Device Channel Real-time Data from Device --------- << \n\n");

	
    int i;
   for  (i=1;  i<100;  i=i+1)
{
    tt = time(NULL);
     float v;
     char  str_v[15]; 
     v  =  rand() % 20;
     sprintf(str_v, "%f", v);
     IOT_DATA_CHANNEL_REALTIMES_ITEM channelRealtime = {channelTag, tt, 313, str_v, false, false, false};
    

    /* Build a device channel real-time message */
    IOT_DATA channelRealtimeData;
    channelRealtimeData.dataType = CHANNEL_REALTIMES;
//    channelRealtimeData.deviceUUID = (char*) deviceUUID;
    channelRealtimeData.deviceUUID = (char*) subdevice.deviceUUID;

    channelRealtimeData.channelRealtimes = iot_list_create();

    iot_list_add(channelRealtimeData.channelRealtimes, &channelRealtime);

    /**
     * Send the real-time message associated with the specified channel from the device to the IoT Hub
     * NOTE: iot_send is part of the Eaton IoT Device SDK
     * and is used to send messages from the Device to the IoT Hub
     * via the pre-established connection to the IoT Hub.
     */
    iot_send(deviceHandle, &channelRealtimeData);

    /* Cleanup and destroy objects */
    iot_list_destroy(channelRealtimeData.channelRealtimes);

    printf("\n\n >> --------- PUBLISH DEVICE CHANNEL\\POINT REAL-TIME DATA END --------- << \n\n");
    
    sleep(5);
}



    /* Wait for user input to terminate the session */
	ThreadAPI_Sleep(1000 * 10);
    printf("\n\n >> --------- PRESS ANY KEY TO QUIT APPLICATION --------- << \n\n ");
    char string[11];
    fgets(string, 10, stdin);


    /* Close the connection, and exit the application  */
    iot_close(deviceHandle);
    printf("\n\n >> --------- Device-to-Hub Connection Closed. Terminating Application: %d --------- << \n\n", exitCode);
    return (EXIT_SUCCESS);
}


void setGlobals() {

    printf("\n\n >> --------- INITIALIZING GLOBAL VARIABLES --------- << \n\n ");

    /* use pre-defined, sample data */
    if (useSampleData) {

        printf("\n\n >> --------- Initializing Global App Variables (Sample)  --------- << \n\n");

        connectionString = sampleConnectionString;
        deviceUUID = sampleDeviceUUID;
        deviceProfileUUID = sampleDeviceProfileUUID;
        deviceName = sampleDeviceName;
        deviceSerialNumber = sampleDeviceSerialNumber;
        deviceAssetTag = sampleDeviceAssetTag;
        deviceMAC = sampleDeviceMAC;
        channelTag = (const char*) sampleChannelTag;

        trendInterval = defaultTrendInterval;
        realtimeInterval = defaultRealtimeInterval;

    } else { /*  use custom, user-defined data  */

        printf("\n\n >> --------- Initializing Global App Variables (User-Defined)  --------- << \n\n");

        connectionString = customConnectionString;
        deviceUUID = customDeviceUUID;
        deviceProfileUUID = customDeviceProfileUUID;
        deviceName = customDeviceName;
        deviceSerialNumber = customDeviceSerialNumber;
        deviceAssetTag = customDeviceAssetTag;
        deviceMAC = customDeviceMAC;
        channelTag = (const char*) customChannelTag;

        if (!customTrendInterval) {
            printf("\n\n >> Using Default Trend Interval");
            trendInterval = defaultTrendInterval;
        } else {
            printf("\n\n >> Setting Custom Trend Interval");
            trendInterval = customTrendInterval;
        }

        if (!customRealtimeInterval) {
            printf("\n\n >> Using Default Real-time Interval");
            realtimeInterval = defaultRealtimeInterval;
        } else {

            printf("\n\n >> Setting Custom Real-time Interval");
            realtimeInterval = customRealtimeInterval;
        }
    }

    globalsInitialized = true;

    printf("\n\n >> Using IoT Hub Connection String: %s ", connectionString);
    printf("\n\n >> Adding Device Profile UUID: %s ", deviceProfileUUID);
    printf("\n\n >> Adding Device Name: %s ", deviceName);
    printf("\n\n >> Adding Device Serial Number: %s ", deviceSerialNumber);
    printf("\n\n >> Adding Device Asset Tag: %s ", deviceAssetTag);
    printf("\n\n >> Adding Device MAC: %s ", deviceMAC);
    printf("\n\n >> Adding Device Channel Tag: %s ", (const char*) channelTag);

    printf("\n\n >> --------- INITIALIZED GLOBAL VARIABLES --------- <<\n\n");
}

