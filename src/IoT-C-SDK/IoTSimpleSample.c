#include "IoTSimpleSample.h"


// Message Declaration
BEGIN_NAMESPACE(IoTSimpleDemo);
DECLARE_MODEL(SimpleMessage,
WITH_DATA(ascii_char_ptr, DeviceId),
WITH_DATA(int, Data)
);
END_NAMESPACE(IoTSimpleDemo);

// Global variables
bool g_Running = true;
IOTHUB_CLIENT_HANDLE g_IoTHubClient = NULL;
const char* g_deviceId = "Device-1";
const char* g_IoTHubConnectionString = “[设备连接字符串]”;

// 初始化传感器
void InitSensors(void)
{
    // TODO: 初始化传感器
    srand((int)time(NULL));  // This is a sample.
}

// 初始化IoT Hub Client
IOTHUB_CLIENT_HANDLE InitIoTHubClient(const char* connectionString)
{
    printf("Starting the IoTHub client simple sample...\r\n");
    if(NULL == connectionString)
    {
        printf("Invalid connection string...\r\n"); 
        return NULL;       
    }

    IOTHUB_CLIENT_HANDLE iotHub = NULL;
    if (0 != platform_init())
    {
        printf("Failed to initialize the platform.\r\n");
    }  
    else if(NULL == (iotHub = IoTHubClient_CreateFromConnectionString(
        connectionString, AMQP_Protocol)))
    {
        printf("Failed to creating the IoT Hub on Azure, please check connection string.\r\n");
    } 

    serializer_init(NULL);

    return iotHub;
}

// 从传感器提取数据
SimpleMessage* RetrieveDataFromSensors(void)
{
    //创建消息对象
    SimpleMessage* message = CREATE_MODEL_INSTANCE(IoTSimpleDemo, SimpleMessage); 
    // TODO:提取数据，并把数据赋值给消息对象
    message->Data = rand() % 100;
    message->DeviceId = (char*)g_deviceId;

    return message;
}

void SendMessageToIoTHub(IOTHUB_CLIENT_HANDLE iothubClient, unsigned char* buffer, size_t size)
{
    if(NULL == iothubClient)
    {
        printf("Invalid IoT Hub handle...\r\n"); 
        return ;       
    }
    if(NULL == buffer)
    {
        printf("Invalid message buffer...\r\n"); 
        return ;       
    }
    
    IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromByteArray(buffer, size);
    if (messageHandle == NULL)
    {
        printf("unable to create a new IoTHubMessage\r\n");
    }
    // 利用SDK API 异步发送消息
    IoTHubClient_SendEventAsync(iothubClient, messageHandle, MessageSentCompleted, messageHandle);
    printf("The message:\r\n %s \r\n has been sent.\r\n", buffer);
    // 释放缓冲区内存
    free(buffer);
    buffer = NULL;    
}

void MessageSentCompleted(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
	printf("Confirmation received for message from device with result = %s\r\n", 
    ENUM_TO_STRING(IOTHUB_CLIENT_CONFIRMATION_RESULT, result));
    // 销毁 message handle
    IOTHUB_MESSAGE_HANDLE sentMessageHandle = (IOTHUB_MESSAGE_HANDLE) userContextCallback;
    IoTHubMessage_Destroy(sentMessageHandle);
}

void IoT_Device_Run(void)
{
    // 初始化传感器
    InitSensors();
    // 初始化IoT Hub Client
    IOTHUB_CLIENT_HANDLE handle = InitIoTHubClient(g_IoTHubConnectionString);
    if(NULL == handle)
        return ;
    while(g_Running)
    {
        // 从传感器周期性地提取数据
        SimpleMessage* msg = RetrieveDataFromSensors();
        if(NULL != msg)
        {
            // 序列化消息数据
            unsigned char* destination = NULL; // 序列化结果缓冲区指针
            size_t size; // 序列化结果的大小

            if (SERIALIZE(&destination, &size, msg->DeviceId, msg->Data) != CODEFIRST_OK)
            {
                printf("Serialize message failed.\r\n");
                continue;
            }

            // 发送数据到IoT Hub 服务端
            SendMessageToIoTHub(handle, destination, size);
        }
        // 每间隔一秒提取数据并发送一次
        ThreadAPI_Sleep(1000);
    }
}