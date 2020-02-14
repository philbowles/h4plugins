#ifndef H4P_PLUGINS_H
#define H4P_PLUGINS_H

#include<H4PCommon.h>

#include<H4P_CmdErrors.h>
#include<H4P_SerialCmd.h>
#include<H4P_LocalLogger.h>
#include<H4P_SerialLogger.h>
#include<H4P_MQTTLogger.h>
#include<H4P_MQTTHeapLogger.h>
//#include<H4P_AsyncHttpRequest.h>
#include<H4P_ExternalSqWave.h>
#include<H4P_TaskSniffer.h>
#include<H4P_QueueWarn.h>
#include<H4P_GPIOManager.h>
#include<H4P_FlasherController.h>
#include<H4P_WiFi.h>
#include<H4P_MQTT.h>
#include<H4P_AsyncWebServer.h>
#include<H4P_BasicSwitch.h>
#include<H4P_UPNPSwitch.h>
#include<H4P_ThreeFunctionButton.h>

//force  static initialisation
#define H4_USE_PLUGINS std::vector<H4Plugin*>   H4Plugin::_plugins; \
    uint32_t H4Plugin::nextSubid=H4PC_SHOW; \
    H4P_CONFIG_BLOCK    H4Plugin::_cb; \ 
    H4_CMD_MAP          H4Plugin::commands; 
#endif