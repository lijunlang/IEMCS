# 基于STM32的室内环境测控系统(毕业设计）
- 开发并实现了一套基于STM32的室内环境测控系统。该系统以STM32 F103 CBT6嵌入式微控制器为主控下位机，采集细颗粒物（PM2.5）、温湿度、可燃/有害气体浓度、光照强度等传感器数据，通过esp8266 Wi-Fi芯片连接无线局域网并使用EDP协议接入中移物联网的设备云开放平台。同时，利用PHP服务器从OneNET平台上获取环境信息，并下发到手机app上。安卓app作为上位机通过接收服务器下发的环境信息或上传控制数据从而实现了诸如远程控制家电、语音识别控制和传感器数据采集显示等功能。
##系统演示：
<iframe src="http//player.bilibili.com/player.html?aid=23413946&cid=39020838&page=1" scrolling="no" border="0" frameborder="no" framespacing="0" allowfullscreen="true"> </iframe>
