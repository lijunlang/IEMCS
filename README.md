# 基于STM32的室内环境测控系统(毕业设计）
- 开发并实现了一套基于STM32的室内环境测控系统。该系统以STM32 F103 CBT6嵌入式微控制器为主控下位机，采集细颗粒物（PM2.5）、温湿度、可燃/有害气体浓度、光照强度等传感器数据，通过esp8266 Wi-Fi芯片连接无线局域网并使用EDP协议接入中移物联网的设备云开放平台。同时，利用PHP服务器从OneNET平台上获取环境信息，并下发到手机app上。安卓app作为上位机通过接收服务器下发的环境信息或上传控制数据从而实现了诸如远程控制家电、语音识别控制和传感器数据采集显示等功能。

## 1.系统总体方案
![image](https://github.com/lijunlang/IEMCS/blob/master/pic/%E7%B3%BB%E7%BB%9F%E6%80%BB%E4%BD%93%E6%96%B9%E6%A1%88.png)
### 1.1嵌入式微控制器及外围硬件部分
- 该系统硬件部分由32位嵌入式微控制器STM32 F103 CBT6，通过I2C接口连接DHT12温湿度传感器，通过UART接口连接灰尘传感器GP2Y1050AU0F以及esp8266串口WiFi模块，通过GPIO接口连接MQ-2烟雾传感器、光敏电阻传感器、水滴传感器、MOS管开关、发光二极管、继电器、电机等组成。
### 1.2嵌入式微控制器软件部分
- 嵌入式微控制器软件设计部分由STM32F103CBT6微控制器为主控下位机，运行FreeRTOS嵌入式实时操作系统,采集细颗粒物（PM2.5）、温湿度、可燃/有害气体浓度、光照强度等传感器数据，通过esp8266串口WiFi连接无线局域网并使用EDP协议接入中移物联网的设备云开放平台。实现数据的采集、发送、接收。
### 1.3客户端软件部分
- 客户端软件设计部分利用PHP服务器从OneNET平台上获取环境信息，并下发到手机app上。安卓app作为上位机通过接收服务器下发的环境信息或上传控制数据从而实现了诸如远程控制家电、语音识别控制和传感器数据采集显示等功能。

## 2.硬件连接
![image](https://github.com/lijunlang/IEMCS/blob/master/pic/%E7%A1%AC%E4%BB%B6%E8%BF%9E%E6%8E%A5%E5%9B%BE1.png)
![image](https://github.com/lijunlang/IEMCS/blob/master/pic/%E7%A1%AC%E4%BB%B6%E8%BF%9E%E6%8E%A5%E5%9B%BE2.png)

## 3.APP截图
![image](https://github.com/lijunlang/IEMCS/blob/master/APP/%E6%89%8B%E6%9C%BA%E6%88%AA%E5%9B%BE1.png)
![image](https://github.com/lijunlang/IEMCS/blob/master/APP/%E6%89%8B%E6%9C%BA%E6%88%AA%E5%9B%BE2.png)

## 4.系统演示
- https://www.bilibili.com/video/av23413946/
