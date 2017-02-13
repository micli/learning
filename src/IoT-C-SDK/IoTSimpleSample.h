#ifndef _IOT_SIMPLE_SAMPLE_H_
#define _IOT_SIMPLE_SAMPLE_H_

#include <stdio.h>
#include <stdlib.h>

// Azure IoT C Client SDK
#include "serializer.h"
#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "iothub_client.h"
#include "iothub_message.h"
#include "iothubtransportamqp.h"

#ifdef __cplusplus
extern "C" {
#endif

    // functions
    // 初始化传感器状态
    void InitSensors(void);
    // 初始化IoT Hub Client
    IOTHUB_CLIENT_HANDLE InitIoTHubClient(const char* connectionString);
    // 从传感器提取数据
    // SimpleMessage* RetrieveDataFromSensors(void);
    // 发送数据到IoT Hub
    void SendMessageToIoTHub(IOTHUB_CLIENT_HANDLE iothubClient, unsigned char* buffer, size_t size);
    void MessageSentCompleted(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback);
    void IoT_Device_Run(void);

#ifdef __cplusplus
}
#endif

#endif /* _IOT_SIMPLE_SAMPLE_H_ */