# 在Linux 操作系统的设备上使用Azure IoT C SDK

## 文档目标

> 编制本文档主要是为了简化IoT C SDK 的使用步骤，降低编译、开发和使用的学习难度，用最快的速度了解和掌握IoT C SDK 的功能和方法。
> 把Azure IoTHub 快速地集成到解决方案中去，尽快的达成使用目标。

## 文档内容和适用范围

> 文档适用于刚刚接触Azure IoT 解决方案，并使用Linux 操作系统作为设备操作系统，需要将设备采集的数据上报到Azure IoT Hub 服务的场景。

## 目录

+ 什么样的设备支持接入Azure IoTHub？
+ IoT C SDK 的获取
+ IoT C SDK 概览
+ IoT C SDK 的编译和测试
+ IoT C SDK 的交叉编译
+ 使用IoT C SDK 创建自己的设备应用
+ 参考链接

## 什么样的设备支持接入Azure IoT Hub？

由于使用场景多种多样，IoT 解决方案中的终端设备硬件、联接方式以及计算能力也是千差万别，那么什么样的设备可以通过IoT Hub SDK 接入Azure IoT Hub 服务呢？
其实这并没有一个硬性的硬件兼容列表给到大家。由于IoT Hub C SDK 具有很大的灵活性和可移植性，因此对硬件的限制是相对较少的。概括地来说，
可以支持IoT C SDK 运行的设备需要具备以下几个条件:

+ **设备需要支持通过IP联接** 设备需要通过IP地址来联接位于云端的IoT Hub 服务。
+ **具备实时时钟或者具备连接NTP服务器的能力** 设备需要获取实时的时间用于安全校验，这对于建立TLS链接和安全令牌都是必须的。
+ **支持TLS通信** 处于安全考虑，IoT Hub要求设备在与IoT Hub 服务之间发送和接收消息时必须要架构在加密的安全链路上。
+ **支持SHA-256加密** 用于生成服务器端验证身份所使用的安全令牌，SHA-256是各种方式中最安全的，如采用其它验证方式则SHA-256能力则不是必须的。
+ **设备至少有64MB 的RAM** 这和IoT C SDK的体积相关，这是一个推荐值。实际内存占用和设备采用何种方式与IoT Hub服务通信相关，不同的协议占用内存数量略有不同。

如果您的设备具备以上能力，那么基本上都可以通过IoT C SDK 接入IoT Hub 服务。目前有越来越多的厂商利用IoT Hub 创建了解决方案，
并把相关信息上传到了微软的网站上，您可以点击 **[这里](https://catalog.azureiotsuite.com)** 查询是否有与您相近的硬件已经成功地集成了Azure IoT Hub 服务。 

## IoT C SDK 的获取

IoT Client C SDK 的获取目前有两种途径，一种是通过添加信任的源，使用apt-get 直接下载安装；另一种是通过GitHub 网站下载SDK源代码
进行编译和使用。

### 通过信任的源进行下载

首先要向apt-get 添加azureiotsdk 的PPA 源，命令如下：

> sudo add-apt-repository ppa:aziotsdklinux/ppa-azureiot

如图所示：

![参考图片](https://github.com/micli/learning/blob/master/images/IoT-C-SDK/apt-get-ppa.png '为apt-get 添加PPA 源')

然后更新apt-get
> sudo apt-get update

最后通过apt-get 安装IoT C SDK
> sudo apt-get install -y azure-iot-sdk-c-dev

如图所示:

![参考图片](https://github.com/micli/learning/blob/master/images/IoT-C-SDK/apt-get-install.png '安装IoT C SDK')

安装成功后，IoT C SDK 的相关头文件被放置在/usr/include/azureiot/ 目录下，如图所示：

![参考图片](https://github.com/micli/learning/blob/master/images/IoT-C-SDK/sdk-include.png '安装IoT C SDK')

其中在/usr/include/azureiot/inc/ 子目录下含有一个azure_c_shared_utility 目录，这个目录中含有一个C 运行时的最小集。
用来支持Azure 相关的C 语言代码的运行。包括一些通用的宏定义，字符串的实现等等。目的是为C SDK 提供最大的可移植性。IoT C SDK
编译好的静态库文件被放置在了/usr/lib/ 目录下。

通过PPA 源的方式获取的IoT C SDK 都是开发团队经过测试的正式发布版本，如果需要Daily build 特殊版本，就需要直接从GitHub上获取源代码进行编译。

### 直接从GitHub上获取源代码进行编译

首先创建一个文件夹，用来保存GitHub 上获取下来的IoT C SDK 源代码，比如: ~/Azure-Iot-C-SDK。然后在这个目录下，
复制Github 上的源代码到本地
> git clone --recursive https://github.com/Azure/azure-iot-sdk-c.git

请务必记得要使用--recursive 参数，否则不会自动获取C SDK 必须的azure_c_shared_utility 源代码。

当源代码工程获取成功后，跳转到~/Azure-Iot-C-SDK/azure-iot-sdk-c/build_all/linux 目录下，运行
> ./setup.sh   
./build.sh

即可实现对IoT C SDK 的编译，以便生成.a静态库文件。

IoT C SDK 的代码编译需要使用gcc 4.9以上版本，以及CMake 3.2 以上版本。对于这两部分的设置，请参考**在Linux 上编译IoT C Client SDK 的先决条件**一节。

## IoT C SDK 概览

通过添加PPA 源的方式安装IoT C SDK 仅包含开发所需要的头文件和库文件。分别放置在/usr/include/ 和/usr/lib/ 目录下。如果
是通过git clone 的全部源代码工程，那么代码中还带有如何调用Azure IoT C SDK 的实例代码以及编译脚本等内容。

在IoT C SDK GitHub 项目文件夹中，有一个名为build_all 的文件夹，该文件夹内保存有SDK 针对所有平台编译的脚本，如Windows、
Linux 和Arduino 等等平台。在对应平台上编译时，需要运行这些脚本。

IoT C SDK的核心最底层API层的实现的可以在iothub_client文件夹中找到。这一系列API提供了设备与Azure IoT Hub联接，发送和接收消息的能力。
开发人员只需要将要传递的消息序列化成为字节流，然后调用API就可以将消息发送给Azure IoT Hub了。iothub_client_ll.h 含有这一组
底层API 的声明，鉴于是底层函数，所以函数名称都增加了_ll 后缀(low-level)。iothub_client.h 是对底层API 函数的再封装。

IoT C SDK 的serializer 文件夹中包含有对于消息的序列化(内存结构体变成字节流)的过程实现代码(src文件夹)和样例（samples文件夹）。
开发人员可以参考相关样例把定义好的消息(C 语言结构体)序列化为字节流，同时也可以把Azure IoT Hub 传送来的数据反序列化为消息。
对消息结构体的声明，serializer 提供了一系列的辅助宏定义，通过这些宏定义可以支持程序员声明和构建用于传递的消息体。

IoT Hub 默认支持HTTP，AMQP，MQTT 三种传输协议。amqp 文件夹和mqtt 文件夹就是对相应的协议封装实现代码所在的文件夹。

parson 文件夹中含有一个用C 语言写成的轻量级的JSON 格式分析函数库。

iothub_service_client 文件夹的内容相对独立。文件夹中含有负责与服务器端Azure IoT Hub 服务交互的API 函数实现。通过IoT Hub service client API，
开发人员可以从IoT Hub 服务中读取设备上报来的消息，向指定设备下发消息，并支持对设备的管理功能。通过iothub_service_client 的功能，
可以高效地、批量地把设备上送的消息从Azure IoT Hub 中提取出来进行后续的操作。 iothub_service_client 也可以根据预设的代码逻辑，
动态的允许或禁止设备联接Azure IoT Hub 服务。

## IoT C SDK 的编译和测试

### 在Linux 上编译IoT C Client SDK 的先决条件

在Linux 上编译IoT C Client SDK 的先决条件主要有三条：

+ 添加依赖库 build-essential curl libcurl4-openssl-dev libssl-dev uuid-dev
+ gcc 编译器要达到4.9 以上版本
+ CMake 要达到3.2以上版本

添加依赖库的命令如下：
> sudo apt-get install -y build-essential curl libcurl4-openssl-dev libssl-dev uuid-dev

验证gcc 的版本，可以使用gcc --version 命令，如果版本低于4.9那么需要您更新gcc 编译版本到最新。具体方法如下：
>sudo add-apt-repository ppa:ubuntu-toolchain-r/test  
sudo apt-get update  
sudo apt-get install gcc-4.9 g++-4.9  
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.9 60 --slave /usr/bin/g++ g++ /usr/bin/g++-4.9

在以上命令中，先添加Ubuntu 测试PPA，然后通过apt-get 安装gcc 和g++ 4.9版本，最后通过update-alternatives 命令映射路径，
将gcc编译器指向4.9版本。

同样地，验证CMake 版本可通过cmake --version 命令，如果CMake 版本低于3.2 请升级CMake版本，命令如下：
>sudo add-apt-repository ppa:george-edison55/cmake-3.x  
sudo apt-get update

### 在Linux 上编译IoT C Client SDK

在通过./build.sh 脚本编译时，可通过输入参数的方法控制编译的生成结果。具体参数列表如下：

参数      | 意义
---------|----------
--run-e2e-tests | 运行端到端测试
--run-unittests | 运行单元测试
--run-longhaul-tests |  运行长途测试
--no-amqp | 不构建AMQP协议相关部分
--no-http | 不构建HTTP协议相关部分
--no-mqtt | 不构建MQTT协议相关部分
--no_uploadtoblob | 不上传到blob
--no-make | 在cmake不运行make，即只生成对应平台的代码工程不编译
--use-websockets | 开启AMQP over WebSockets 选项
--toolchain-file 文件路径 | 在交叉编译时指定工具链路径
--install-path-prefix | 指定 make install的替代前缀
--build-python python 版本 | 给指定python 版本构建Python C包装模块(需要boost)
--build-javawrapper | 构建Java C 封装模块
-rv, --run_valgrind | 加载valgrind 检查模块来运行ctest 单元测试
--no-logging | 禁用日志

如果需要对SDK 代码进行单元测试，可在编译时指定--run-unittests 参数，如果需要对SDK 的功能和代码进行长时间的稳定性测试，
可指定--run-longhaul-tests 参数。其实，对于一款确定的设备来说，它所选用的通讯协议也是唯一确定的，也可以只构建指定的协议，
不必所有协议全都构建，这样可以有效地缩减程序体积。

在工程文件夹中，amqp，mqtt和serializer 文件夹内都有相应的samples文件夹，内含相关样例程序源代码用于指定场景和指定协议的测试。
在使用这些样例代码时，需要预先设置IoT Hub 设备连接串。以serializer 文件夹下的samples/simplesample_amqp 样例为例。
源代码内并没有设置有效的IoT Hub 设备连接串，需要手工修改有效连接串后再进行构建和测试运行。一般来说IoT Hub 设备连接串的声明位于
源代码文件.c 文件中，如下图所示：
![样例代码中的IoT Hub 设备连接串声明](https://github.com/micli/learning/blob/master/images/IoT-C-SDK/device-connstr.png)

在运行程序之前，必须要对connectionString 进行有效地赋值。创建设备连接字符串的方法请参考下面一节内容。

### 获取IoT Hub 设备联接串

IoT SDK 内含一个用Node.js 开发的命令行工具来帮助开发人员在命令行下管理Azure IoT Hub 服务。
这个命令行工具名为iothub-explorer，安装方法如下：
> sudo npm install -g iothub-explorer

如果在安装后遇到"/usr/bin/env: node: No such file or directory" 错误，请运行下面的命令：
> sudo ln -s /usr/bin/nodejs /usr/bin/node

这是由于Node.js 在后期版本中修改了文件夹名称导致的。

在一切就绪后，可以通过下面的命令创建IoT Hub 设备连接字符串：
> iothub-explorer login "[IoT Hub 服务连接字符串]"  
> iothub-explorer create <设备ID> --connection-string

如下图所示:
![参考图片](https://github.com/micli/learning/blob/master/images/IoT-C-SDK/iothub-explorer-create.png '创建设备ID')

上图中高亮的connectionString 的值需要复制到样例项目中，作为connectionString 字符串指针的值。
重新构建之后，样例程序即可上送消息到IoT Hub 服务中。

### 参考链接

+ [IoT Hub C SDK Introduction](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-device-sdk-c-intro)
+ [iothub-explorer 工具](https://github.com/azure/iothub-explorer)
+ [More about IotHubClient](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-device-sdk-c-iothubclient)
+ [More about Serializer](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-device-sdk-c-serializer)
+ [IoT C SDK API 参考](http://azure.github.io/azure-iot-sdks/c/api_reference/index.html)