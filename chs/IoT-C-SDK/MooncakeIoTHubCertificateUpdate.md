最近, Azure Mooncake 用户都收到了下面的消息, 通知Mooncake 用户IoT Hub TLS 证书将在 9月25日进行更改：
[中国区Azure IoT 中心服务的根证书变更通知](https://www.azure.cn/blog/2017/07/21/RootCertificateChangeNoticeforAzureIoTHubServiceinAzureinChina)
 
请不要忽略此通知, 因为此更改将影响到您的物联网设备连接到 IoT Hub服务。
 
# 为什么Azure Mooncake必须更改 TLS 证书？
 
原因是Azure Mooncake现在使用的 TLS 证书是从 WoSign 签发的。不幸的是, WoSign 将被世界主流 IT 公司 (如 Mozilla、谷歌、苹果和微软) 抛弃。因为 WoSign 不珍惜自己的名誉, 做了一些蠢事。WoSign TLS 证书将不再能保护设备和物联网服务之间的数据安全。这是一个严重的安全问题, 并且已蔓延超过10月了。其实，不仅仅是IoT Hub 服务，Azure Mooncake 上的其它服务也都会全面禁用WoSign证书，有些服务早已更换完成了，比如SQL 数据库PaaS服务。
 
有关WoSign证书安全问题的详细信息, 请查看以下链接:
* [microsoft dumps notorious chinese secure certificate vendor](http://www.zdnet.com/article/microsoft-dumps-notorious-chinese-secure-certificate-vendor/)
* [Mozilla: WoSign Issues](https://wiki.mozilla.org/CA:WoSign_Issues)
* [知乎: 如何看待 Mozilla 决定停止信任沃通 (WoSign) 和 StartCom 颁发的证书？](https://www.zhihu.com/question/51042407)

微软本次使用DigiCert 替换掉WoSign 证书也是本着对用户数据传输安全负责的态度，及时消除安全隐患而进行的。
 
# 作为Mooncake IOTHub 用户，需要做什么？
 
首先, 我们必须对本次TLS 证书的变更引起足够的重视，因为TLS 证书一旦失效，会影响到所有通过安全套接字层连接IoT Hub 服务的全部设备。这将导致仍在使用WoSign 证书的设备无法连接到IoT Hub 服务。其次, 作为Mooncake IoT Hub 的用户需要进行相应的更新操作来应对本次更新。
 
基于IoT Hub 的开发和使用阶段，我们可以分为四种情况:
 
情况一: 用户已将设备部署到生产环境中。设备正在使用已包含新的 TLS 证书的 SDK。
情况二: 用户正在计划使用IoT Hub 服务, 项目处在计划或 PoC 阶段。
情况三: 用户已将设备部署到生产环境中。用户的设备完全有能力实现自我更新(OTA)。
情况四: 用户已将设备部署到生产环境中。设备没有能力做自我更新。
 
### 对于第一种情况:
已包含新的 TLS 证书的很多 SDK 意味着IoT Hub 正在使用的是以下IoT Client SDK 版本:
* IoT C SDK **2017.3.10 或更新的版本**
* IoT Java SDK **2017.4.21 或更新的版本**
* IoT Python SDK **2017.06.06 或更新的版本**
 
如果用户不确定他们是否使用了正确的IoT Client SDK版本, 那么如何验证呢？
对于IoT Client C SDK, 请打开 <IoT C SDK root folder> / certs/certs.c 文件。
对于IoT Client Java SDK, 请打开 <IoT Java SDK root folder> / device/iot-device-client/src/main/java/com/microsoft/azure/sdk/iot/device/IotHubCertificateManager.java 文件。
对于 IoT Client Python SDK, 请打开 <IoT Python SDK root folder> / device/samples/iothub_client_cert.py 文件。
请查看文件的内容, 并检查其中是否有 DigiCert 证书的相关数据。类似下面的内容：
```/*DigiCert Global Root CA*/
"-----BEGIN CERTIFICATE-----\r\n"
"MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\r\n"
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\r\n"
"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\r\n"
"QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\r\n"
"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\r\n"
"b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\r\n"
"9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\r\n"
"CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\r\n"
"nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\r\n"
"43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\r\n"
"T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\r\n"
"gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\r\n"
"BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\r\n"
"TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\r\n"
"DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\r\n"
"hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\r\n"
"06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\r\n"
"PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\r\n"
"YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\r\n"
"CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\r\n"
"-----END CERTIFICATE-----\r\n"
```
 
如果 Java 开发人员使用了 Maven, 请确保 com.microsoft.azure.sdk.iot:iot-device-client JAR包版本是**1.1.25或者更新的版本**。
 
如果您的设备应用已满足上述标准, 您不需要做任何事情来应对本次更新。
 
### 对于第二种情况:
这意味着您的IoT设备没有真正地部署到生产环境中, 但是使用了版本较老的IoT Client SDK。您需要从 Github 获得最新的IoT Client SDK 源代码, 并与您编写的设备应用程序代码一起重新编译，以便确保DigiCert证书已经包含在您的设备应用程序中。然后您需要对您用于测试的设备重新部署刚刚编译好的设备应用程序。以下是IoT Client SDK 的Github 代码仓库地址:
* [IoT Client C SDK 项目Github 仓库](https://github.com/Azure/azure-iot-sdk-c)
* [IoT Client Java SDK 项目Github 仓库](https://github.com/Azure/azure-iot-sdk-java)
* [IoT Client Python SDK 项目Github 仓库](https://github.com/Azure/azure-iot-sdk-python)
 
### 对于第三种情况:
在这种情况下, 您必须从 Github 网站下载最新的IoT Client SDK 源代码。然后您必须用最新的IoT Client SDK与您的设备应用程序代码一起重新编译。最后, 您必须在**2017年9月25日之前**通过设备的自我更新功能(OTA)，升级IoT设备应用程序。如果您的IoT设备中的应用程序在9月25日还没有更新, 您的物联网设备将从2017年9月25日起失去与Mooncake IoT Hub服务的连接。
 
### 对于第四种情况:
这是最坏的情况。因为您的IoT 设备没有自动更新能力，应对本次TLS 证书更新会导致极大的工作量。与情况三类似, 您必须从 Github 下载最新的IoT Client SDK源代码, 然后使用最新的IoT Client SDK来编译应用程序。最后, 您必须在**2017年9月25日之前**逐个手动升级IoT设备。
 
**请Azure Mooncake IoT Hub服务的用户及时采取行动,以便确保本次证书更新不会影响到未来您IoT 设备与IoT Hub 服务的通信。**

Azure Mooncake 运维团队会在以下时间持续地提醒IoT Hub 服务用户：
* 更新之前的45天(已经邮件提醒)
* 更新之前的30天
* 更新之前的14天
* 更新之前的7天，每日提醒
