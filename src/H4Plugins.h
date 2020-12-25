#ifndef H4P_PLUGINS_H
#define H4P_PLUGINS_H

#include<config.h>

#include<H4P_CmdErrors.h>
#include<H4P_Heartbeat.h>
#include<H4P_SerialCmd.h>
#include<H4P_SerialLogger.h>
#include<H4P_ExternalSqWave.h>
#include<H4P_TaskSniffer.h>
#include<H4P_HeapWarn.h>
#include<H4P_LoopCount.h>
#include<H4P_QueueWarn.h>
#include<H4P_GPIOManager.h>
#include<H4P_FlasherController.h>
#include<H4P_ToneController.h>

#include<H4P_PersistentStorage.h>
#include<H4P_WiFi.h>
#include<H4P_AsyncWebServer.h>
#include<H4P_AsyncMQTT.h>
#ifdef H4P_LOG_EVENTS
    #include<H4P_LocalLogger.h>
    #include<H4P_MQTTLogger.h>
    #include<H4P_MQTTHeapLogger.h>
    #include<H4P_MQTTQueueLogger.h>
    #include<H4P_HttpMySQLLogger.h>
#endif
#include<H4P_RemoteUpdate.h>
#include<H4P_udpLogger.h>
#include<H4P_UPNPServer.h>
#include<H4P_PresenceDetector.h>
#include<H4P_Timekeeper.h>
#include<H4P_Sunrise.h>

#include<H4P_BinaryThing.h>
#include<H4P_BinarySwitch.h>
#include<H4P_MultiFunctionButton.h>

//force  static initialisation

#define H4_USE_PLUGINS(s,q,b) \
    H4 h4(s,q); \
    std::vector<H4Plugin*>   H4Plugin::_plugins; \
    H4_CMD_MAP          H4Plugin::_commands; \
    H4P_SerialCmd       h4cmd(b); \
    H4GM_PINMAP         H4P_GPIOManager::pins; 
    uint32_t            H4Plugin::_nxtSubCmd=H4PC_MAX-1; \
    H4P_CONFIG_BLOCK    H4Plugin::_cb={{"pmv",PANGO_VERSION},{h4pvTag(),H4P_VERSION}};
#endif