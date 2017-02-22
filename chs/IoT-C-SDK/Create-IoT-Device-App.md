# 使用IoT Client C SDK 创建自己的设备应用

## 文档目标

> 编制本文档主要是为了阐述如何利用IoT C SDK 开发一个简单的设备端应用以便实现周期性的数据的上送和接收。
开发人员可以利用本文档中涉及到的代码片段快速地创建一个简单的应用程序实与Azure IoT Hub 的数据交互。

## 文档内容和适用范围

> 文档适用于刚刚接触Azure IoT 解决方案，并使用Linux 操作系统作为设备操作系统，需要将设备采集的数据上报到Azure IoT Hub 服务的场景。

## 目录

+ 设备发送数据到IoT Hub 的操作流程
+ 声明消息结构体
+ 通过数据连接串创建iothubClient
+ 发送数据到IoT Hub
+ 监控数据接收
+ 如何快速开始？
+ 关于交叉编译

## 设备发送数据到IoT Hub 的操作流程

一般来说，IoT 设备都是以固定时间周期向服务端上报传感器数据，在服务器端实现数据的归集和分析。总结在IoT 设备端的执行流程如下：
![IoT设备应用流程图](https://github.com/micli/learning/blob/master/images/IoT-C-SDK/device-app-process.png 'IoT设备应用流程图')

1. 应用程序启动后，首先需要初始化传感器设备，创建与传感器的连接。比如485串口，GPS设备等等，做好从传感器周期性地提取数据的准备；
2. 应用程序需要通过IoT Hub 设备连接字符串创建iothubClient 对象，实现与Azure IoT Hub 的连接；
3. 应用程序通过指定的方法从传感器中周期性地提取传感器数据信息，组成要上送IoT Hub 服务的消息；
4. 应用程序调用IoT SDK API 把消息上送到Azure IoT Hub 服务中；
5. 消息发送到Azure IoT Hub 之后，IoT SDK 会通过回调函数通知发送程序，此时应进行后处理；
6. 步骤3-步骤5将周期性地往复执行；
7. 程序退出之前对占用资源进行释放；

## 声明消息结构体

IoT C SDK 中预定义了一组宏用来帮助开发人员声明要传输的消息。

```c
BEGIN_NAMESPACE(WeatherStation);
DECLARE_MODEL(ContosoAnemometer,
WITH_DATA(ascii_char_ptr, DeviceId),
WITH_DATA(int, WindSpeed),
WITH_ACTION(TurnFanOn),
WITH_ACTION(TurnFanOff),
WITH_ACTION(SetAirResistance, int, Position)
);
END_NAMESPACE(WeatherStation);
```

消息的声明必须要使用BEGIN\_NAMESPACE 和END\_NAMESPACE 宏包裹起来，并且需要在这对宏中声明命名空间的名称。
DECLARE_MODEL 宏用来声明要发送的消息结构。第一个参数是消息对象的名称，此后都是消息体的成员声明。
WITH\_DATA 宏用来声明单项消息体的成员，其中第一个参数是消息体成员的数据类型，第二个参数是消息体成员的名字。
IoT C SDK 内建支持的序列化数据类型请参考下表：


数据类型 | 描述
---------|----------
double | 双精度浮点
int | 32位整型
float | 单精度浮点
long | 长整型
int8_t | 8位整型
int16_t | 16位整型
int32_t | 32位整型
int64_t | 64位整型
bool | 布尔型
ascii_char_ptr | ASCII码字符串
EDM_DATE_TIME_OFFSET | 日期时间偏移量
EDM_GUID | GUID
EDM_BINARY | 二进制
DECLARE_STRUCT | 复杂类型

对于Azure IoT Hub 服务下发的消息，在IoT 设备端需要有对应的函数来处理。下发消息的处理函数的映射关系是通过WITH_ACTION 宏来声明的。
开发人员需要通过定义这些声明来实现IoT 设备与IoT Hub 之间的数据交互。在消息传递过程中，IoT C SDK 会将消息结构序列化为JSON 格式的文本，
在设备和IoT Hub 之间进行传递。对于二进制内容，IoT C SDK 会将数据线转换为Base64 编码，以确保数据变成可打印字符再进行传递。

## 通过数据连接串创建iothubClient

IoT C SDK 提供了名为IoTHubClient_CreateFromConnectionString 的API函数，该函数通过传入的设备连接串可以生成一个
IOTHUB\_CLIENT\_HANDLE 类型的句柄对象。这个对象代表了客户端设备与服务端Azure IoT Hub 的链接。在为该函数传入设备连接串
的同时，还需要传入协议提供者的指针，用来指名用什么连接协议与Azure IoT Hub 进行通信。通常协议的提供者指针为：
AMQP\_Protocol，MQTT\_Protocol，HTTP\_Protocol 三种选择。
设备的注册和连接串的创建，请参考
[在Linux 操作系统的设备上使用Azure IoT C SDK](https://github.com/micli/learning/blob/master/chs/IoT-C-SDK/Compile-IoT-C-SDK.md)
 一文中获取IoT Hub 设备联接串一节的内容。

在正式调用IoTHubClient_CreateFromConnectionString 之前，需要调用platform\_init()来要求IoT C SDK 进行初始化。
代码如下：

```c
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
```

## 发送数据到IoT Hub

IoT 设备的一个最主要的功能就是用来完成把数据从传感器提取，再发往Azure IoT Hub 服务的过程。当数据从传感器中提取出来后，
就需要调用IoT C SDK的API 把数据上送到Azure IoT Hub。IoT C SDK 提供了IoTHubClient\_SendEventAsync() 函数来实现
异步地发送消息功能。IoTHubClient\_SendEventAsync() 函数要求传入IOTHUB\_CLIENT\_HANDLE 对象来表用数据要发送到哪一个
已经联接的Azure IoT Hub 服务。第二个参数要求传入消息句柄对象，表明要发送的消息是哪个一。第三个参数是回调函数指针，由于是
异步发送，IoTHubClient\_SendEventAsync()函数是立即返回的，此时该函数会在后台线程中完成消息的发送。当消息发送结束，
会调用开发人员指定的回调函数，以便开发人员进行消息发送后的后续的处理。第四个参数是一个上下文对象指针。对象的类型可由
开发人员自定义，这个对象主要用来记录发送消息的状态信息。当回调函数被触发的时候，该对象将作为参数传入回调函数，
帮助开发人员恢复状态。

在消息发送之前，需要对消息进行序列化操作。IoT C SDK 的SERIALIZE 宏可以帮助开发人员把内存中的数据和对象转换成JSON字节流。
这一过程我们称之为序列化。SERIALIZE 宏要求开发人员传入一个指向字符串的指针和一个指向size_t 类型的指针。这个宏会自动地
为序列化字节流动态分配内存，并将内存首地址保存在传入宏的指向字符串的指针中。因此开发人员在使用结束后，需要自行释放缓冲区，
避免内存泄露。 序列化消息和发送消息的代码如下：

```c
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
// 发送数据到IoT Hub
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
```

当消息传递到Azure IoT Hub 之后，IoT C SDK 会回调已经预先定义好的事后处理函数，也就是上面代码中的MessageSentCompleted，
回调函数的声明如下:

```c
void MessageSentCompleted(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
    printf("Confirmation received for message from device with result = %s\r\n",
    ENUM_TO_STRING(IOTHUB_CLIENT_CONFIRMATION_RESULT, result));
}
```

在回调函数中，第一个参数代表发送的结果，可能是成功或者是失败；第二个参数是IoTHubClient_SendEventAsync传入的上下文状态对象。

## 监控Azure IoT Hub 数据接收

在代码编写完成后，需要验证数据是否已经可以正常地上传到Azure IoT Hub 中。这需要使用iothub-explorer 工具来帮助监控
Azure IoT Hub 收到的消息。iothub-explorer 工具需要Node.js的支持，并通过npm 进行安装，命令如下：

> npm install -g iothub-explorer

安装结束后，可以通过iothub-explorer 的monitor-events 参数来监控指定设备ID 上送的消息，命令如下：

> iothub-explorer monitor-events [设备Id] --login [Azure IoT Hub 管理连接字符串]

连接成功后，会在命令行中显示指定设备Id上送的消息。如下图所示：

![iothub-explorer监控设备发送消息](https://github.com/micli/learning/blob/master/images/IoT-C-SDK/iothub-explorer-monitor.png 'iothub-explorer监控设备消息')

以上代码位于[https://github.com/micli/learning/tree/master/src/IoT-C-SDK](https://github.com/micli/learning/tree/master/src/IoT-C-SDK) 之中。
在这一切准备好之后，就可以运行编写好的IoT 设备程序，来验证程序是否真的把消息发送到了指定的Azure IoT Hub 服务上面。

## 如何快速开始？

了解了上内容后，如何从零开始构建一个简单的IoT 设备端应用程序呢？ 您可以遵循下面的步骤：

1. 利用apt-get 下载IoT C SDK，命令如下：

> sudo add-apt-repository ppa:aziotsdklinux/ppa-azureiot   
> sudo apt-get install -y azure-iot-sdk-c-dev

2. 在操作系统任意位置创建一个文件夹，从GitHub 上下载IoT C SDK 的[快速启动代码](https://github.com/micli/learning/tree/master/src/IoT-C-SDK)到这个文件夹：

![IoT快速开始代码文件夹](https://github.com/micli/learning/blob/master/images/IoT-C-SDK/IoT-Quick-Start-Folder.png 'IoT快速开始代码文件夹')

3. 在IoTSimpleSample.c 文件的头部，修改消息的声明，使之适应数据上传的要求

4. 创建一个Azure IoT Hub 服务，获取管理服务端的联接串

5. 用iothub-explorer 向IoT Hub 服务端注册一个IoT 设备，并获取链接字符串。在IoTSimpleSample.c 文件中修改g_IoTHubConnectionString 变量的IoT Hub 连接串

6. 打开代码的 IoTSimpleSample.c 文件, 在InitSensors() 函数中填入初始化设备代码，在RetrieveDataFromSensors()函数中填入从传感器提取数据并给消息对象赋值的代码

7. 利用CMake 构建makefile 文件，再利用makefile 构建应用程序
> cmake ./  
> make

通过以上五个步骤就可以快速创建出一个用于向Azure IoT Hub 服务端上传数据的简单应用程序了。

程序真正部署到IoT 设备上运行时，往往需要自动启动，可考虑使用systemd 配置应用程序自动启动，Supervisor 对于IoT 设备来说
可能显得太重了。

## 关于交叉编译

交叉编译的前提主要有四个步骤：

1. 在开发板操作系统上安装IoT C SDK 的依赖库
2. 把目标操作系统的工具链部署到编译计算机上
3. 目标操作系统的头文件和库文件同步到编译计算机上
4. 从Github 上获取IoT C SDK

在准备好以上步骤以后就可以开始交叉编译了。以树莓派使用的操作系统Raspbian(Debian的变种)为例，需要在开发板操作系统上用下面命令安装依赖库

> sudo apt-get install -y build-essential curl libcurl4-openssl-dev libssl-dev uuid-dev

在编译计算机上，需要从Github上下载工具链：
> cd ~  
> mkdir RPiTools  
> cd RPiTools  
> git clone https://github.com/raspberrypi/tools.git

下载完成后，需要把Raspbian 上的头文件和库同步到编译计算机上。这需要开发人员首先要有一个可以正常使用的开发板，
然后让开发板通过网络与编译计算机相连。连接成功后，在开发板上打开远程访问功能，最后执行下面的命令：

> cd ~/RPiTools/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/arm-linux-gnueabihf   
> rsync -rl --safe-links pi@<树莓派设备名称或者IP地址>:/{lib,usr} .

rsync 命令会递归地把开发板上的/lib 和/usr 的内容复制到编译计算机的树莓派工具链目录下。 由于内容过多，这个复制的过程会比较漫长，
通常在网络较好的情况下也要持续40分钟以上。需要耐心等待。

在以上动作完成之后，需要把Raspbian 工具链编译器所在文件夹路径设置到环境变量RPI_ROOT 中
> cd ~/RPiTools/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/arm-linux-gnueabihf  
> export RPI_ROOT=$(pwd)

在编译计算机上，从Github上获取IoT C SDK 命令如下:
> git clone --recursive https://github.com/Azure/azure-iot-sdks.git

并修改位于IoT C SDK 文件夹所在目录/c/build_all/linux 中创建一个名为toolchain-rpi.cmake 的文件，并加入下面的内容：

```cmake
INCLUDE(CMakeForceCompiler)

SET(CMAKE_SYSTEM_NAME Linux)     # this one is important
SET(CMAKE_SYSTEM_VERSION 1)     # this one not so much

# this is the location of the amd64 toolchain targeting the Raspberry Pi
SET(CMAKE_C_COMPILER $ENV{RPI_ROOT}/../bin/arm-linux-gnueabihf-gcc)

# this is the file system root of the target
SET(CMAKE_FIND_ROOT_PATH $ENV{RPI_ROOT})

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
```

这个cmake 文件指定了Raspbian 系统工具链中编译器所在的文件路径。以及编译时的“根目录”(RPI_ROOT)。

最后终于可以开始编译了，命令如下：
> cd ~/Source/azure-iot-sdks/c/build_all/linux   
> ./build.sh --toolchain-file toolchain-rpi.cmake -cl --sysroot=$RPI_ROOT

在编译时，需要为编译脚本指定树莓派专有cmake 文件和编译时的根目录。对于一些版本比较低的ARM编译器，需要显式地指定使用C99规范编译源代码。
并关闭"Warning treated as error" 开关。为ARM gcc 编译器指定编译参数需要使用-cl，如下：

> cd ~/Source/azure-iot-sdks/c/build_all/linux   
> ./build.sh --toolchain-file toolchain-rpi.cmake -cl --sysroot=$RPI_ROOT -cl -w -cl --std=c99

