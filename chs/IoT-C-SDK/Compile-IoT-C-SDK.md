# 在Linux 操作系统的设备上使用Azure IoT C SDK

## 文档目标

> 编制本文档主要是为了简化IoT C SDK 的使用步骤和编译的学习难度，用最快的速度了解和掌握IoT C SDK 的功能和方法。

## 文档内容和适用范围

> 文档适用于刚刚接触Azure IoT 解决方案，并使用Linux 操作系统作为设备操作系统的场景。

## 目录

+ IoT C SDK 的获取
+ IoT C SDK 概览
+ IoT C SDK 的编译和测试
+ IoT C SDK 的交叉编译
+ 使用IoT C SDK 创建自己的设备应用
+ 参考链接

## IoT C SDK 的获取

IoT Client C SDK 的获取目前有两种途径，一种是通过添加信任的源，使用apt-get 直接下载安装；另一种是通过GitHub 网站下载SDK源代码
进行编译和使用。

### 通过信任的源进行下载

首先要向apt-get 添加azureiotsdk 的PPA 源，命令如下：

> sudo add-apt-repository ppa:aziotsdklinux/ppa-azureiot

如图所示：

![参考图片](https://github.com/micli/learning/images/IoT-C-SDK/apt-get-ppa.png, '为apt-get 添加PPA 源')

然后更新apt-get
> sudo apt-get update

最后通过apt-get 安装IoT C SDK
> sudo apt-get install -y azure-iot-sdk-c-dev

如图所示:

![参考图片](https://github.com/micli/learning/images/IoT-C-SDK/apt-get-install.png, '安装IoT C SDK')

安装成功后，IoT C SDK 的相关头文件被放置在/usr/include/azureiot/ 目录下，如图所示：

![参考图片](https://github.com/micli/learning/images/IoT-C-SDK/sdk-include.png, '安装IoT C SDK')

其中在/usr/include/azureiot/inc/ 子目录下含有一个azure_c_shared_utility 目录，这个目录中含有一个C 运行时的最小集。
用来支持Azure 相关的C 语言代码的运行。包括一些通用的宏定义，字符串的实现等等。目的是为C SDK 提供最大的可移植性。IoT C SDK
编译好的静态库文件被放置在了/usr/lib/ 目录下。

通过PPA 源的方式获取的IoT C SDK 都是开发团队经过测试的正式发布版本，如果需要Daily build 特殊版本，就需要直接从GitHub上获取源代码进行编译。

### 直接从GitHub上获取源代码进行编译

首先创建一个文件夹，用来保存GitHub 上获取下来的IoT C SDK 源代码，比如: ~/Azure-Iot-C-SDK。然后在这个目录下，
复制Github 上的源代码到本地
> git clone --recursive https://github.com/Azure/azure-iot-sdk-c.git

请务必记得要使用--recursive 参数，否则不会自动获取C SDK 必须的azure_c_shared_utility 源代码。

当获取成功后，跳转到~/Azure-Iot-C-SDK/azure-iot-sdk-c/build_all/linux 目录下，运行
> ./setup.sh   
./build.sh

即可实现对IoT C SDK 的编译。

IoT C SDK 的代码编译需要使用gcc 4.9以上版本，以及CMake 3.2 一上版本。对于这两部分的设置，请参考。[!未完]

## IoT C SDK 概览

通过添加PPA 源的方式安装IoT C SDK 仅包含开发所需要的头文件和库文件。分别放置在/usr/include/ 和/usr/lib/ 目录下。如果
是通过git clone 的全部源代码工程，那么代码中还带有如何调用Azure IoT C SDK 的实例代码以及编译脚本等内容。

在IoT C SDK GitHub 项目文件夹中，有一个名为build_all 的文件夹，该文件夹内保存有SDK 针对所有平台编译的脚本，如Windows、
Linux 和Arduino 等等平台。在对应平台上编译时，需要运行这些脚本。

IoT C SDK的核心最底层API层的实现的可以在iothub_client文件夹中找到。这一系列API提供了设备与Azure IoT Hub联接，发送和接收消息的能力。
开发人员只需要将要传递的消息序列化成为字节流，然后调用API就可以将消息发送给Azure IoT Hub了。

IoT C SDK 的serializer 文件夹中包含有对于消息的序列化(内存结构体变成字节流)的过程实现代码(src文件夹)和样例（samples文件夹）。
开发人员可以参考相关样例把定义好的消息(C 语言结构体)序列化为字节流，同时也可以把Azure IoTHub 传送来的数据反序列化为消息。
对消息结构体的声明，serializer 提供了一系列的辅助宏定义，通过这些宏定义可以支持程序员声明和构建用于传递的消息体。

IoT Hub 默认支持HTTP，AMQP，MQTT 三种传输协议。amqp 文件夹和mqtt 文件夹就是对相应的协议封装实现代码所在的文件夹。

parson 文件夹中含有一个用C 语言写成的轻量级的JSON 格式分析函数库。

iothub_service_client 文件夹的内容相对独立。文件夹中含有负责与服务器端Azure IoTHub 服务交互的API 函数实现。通过IoT Hub service client API，
开发人员可以从IoT Hub 服务中读取设备上报来的消息，向指定设备下发消息，并对消息



