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
+ 让设备从IoT Hub 接收数据

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