#pragma once

#include"config_plugins.h"

#if H4P_USE_WIFI_AP
    #pragma message("Using Wifi AP - Make sure you have made arrangements!")
#endif

#include<H4P_AsyncHTTP.h>
#include<H4P_AsyncMQTT.h>
#include<H4P_BinarySwitch.h>
#include<H4P_BinaryThing.h>
#include<H4P_EmitHeap.h>
#include<H4P_EmitLoopCount.h>
#include<H4P_EmitQ.h>
#include<H4P_EmitTick.h>
#include<H4P_EventListener.h>
#include<H4P_GateKeeper.h>
#include<H4P_HeapWarn.h>
#include<H4P_Heartbeat.h>
#include<H4P_LinkMaster.h>
#include<H4P_LocalLogger.h>
#include<H4P_MQTTLogger.h>
#include<H4P_PinMachine.h>
#include<H4P_RemoteLogger.h>
#include<H4P_QueueWarn.h>
#include<H4P_RemoteUpdate.h>
#include<H4P_SerialCmd.h>
#include<H4P_Signaller.h>
#include<H4P_TaskSniffer.h>
#include<H4P_Timekeeper.h>
#include<H4P_ToneController.h>
#include<H4P_UPNPServer.h>
#include<H4P_WiFi.h>

#include<h4proxy.h>

class H4Service;
#define H4_USE_PLUGINS(s,q,b) \
    H4 h4(s,q); \
    H4P_PROXY_MAP h4pGlobal={ {binTag(),{binTag(),__FILE__,false}} }; \
    H4P_SerialCmd h4p(b);

std::unordered_map<std::string,H4Service*> h4pmap;
bool                h4punlocked=true;

H4P_CMDMAP          h4pCmdMap;
H4P_FLASHMAP        h4pFlashMap;
H4P_PINMAP          h4pPinMap;

#if H4P_VERBOSE
    H4_INT_MAP cmdErrors={
        {H4_CMD_OK,"OK"},
        {H4_CMD_UNKNOWN,"Unknown cmd"},
        {H4_CMD_TOO_FEW_PARAMS,"Too few parameters"},
        {H4_CMD_TOO_MANY_PARAMS,"Too many parameters"},
        {H4_CMD_NOT_NUMERIC,"Numeric value expected"},
        {H4_CMD_OUT_OF_BOUNDS,"Value out of range"},
        {H4_CMD_NAME_UNKNOWN,"Name not known"},
        {H4_CMD_PAYLOAD_FORMAT,"Incorrect Payload Format"},
        {H4_CMD_NOT_NOW,"Can't do now"}
    };

    H4_INT_MAP eventTypes={
        {H4PE_NOOP,"NO-OP"},
        {H4PE_SYSINFO,"INFO"},
        {H4PE_SYSWARN,"WARN"},
        {H4PE_SYSFATAL,"FATAL"},
        {H4PE_BOOT,"BOOT"},
        {H4PE_SERVICE,"SVC"},
        {H4PE_MSG,"LOG"},
        {H4PE_REBOOT,"REBOOT"},
        {H4PE_UIADD,"UIADD"},
        {H4PE_FACTORY,"FACTORY"},
        {H4PE_CMD,"CMD"},
        {H4PE_HEAP,"HEAP"},
        {H4PE_Q,"Q"},
        {H4PE_LOOPS,"LOOPS"},
        {H4PE_PRESENCE,"PRESENCE"},
        {H4PE_GVCHANGE,"GVCHG"},
        {H4PE_UISYNC,"UISYNC"},
        {H4PE_STAGE2,"STAGE2"},
        {H4PE_GPIO,"GPIO"},
        {H4PE_SIGNAL,"SIGNAL"},
        {H4PE_VIEWERS,"EYES"},
        {H4PE_RTC,"CLOK"},
        {H4PE_ALARM,"ALARM"},
        {H4PE_GRID,"GRID"},
        {H4PE_UPNP,"UPNP"},
        {H4PE_UIMSG,"UIMSG"},
        {H4PE_HEARTBEAT,"PING"}
    };

    H4_INT_MAP ledColors={
        {H4P_UILED_RED,"RED"},
        {H4P_UILED_ORANGE,"ORANGE"},
        {H4P_UILED_YELLOW,"YELLOW"},
        {H4P_UILED_GREEN,"GREEN"},
        {H4P_UILED_BLUE,"BLUE"},
        {H4P_UILED_BI,"RED/GREEN"}
    };

//    extern const char* giveTaskName(uint32_t id);

    std::string h4pGetErrorMessage(uint32_t e){ return cmdErrors.count(e) ? cmdErrors[e]:std::string("No such error (")+stringFromInt(e)+")"; }
    std::string h4pGetEventName   (H4PE_TYPE e){ return eventTypes.count(e) ? eventTypes[e]:""; }
    std::string h4pGetLedColor    (uint8_t c){ return ledColors.count(c) ? ledColors[c]:"BLACK"; }
    void        h4pClearEvent     (H4PE_TYPE e){ eventTypes.erase(e); }
#else
    std::string h4pGetErrorMessage(uint32_t e){ return std::string("Err:"+stringFromInt(e)); }
    std::string h4pGetEventName   (H4PE_TYPE e){ return stringFromInt(e,"0x%08x"); }
    std::string h4pGetLedColor    (uint8_t c){ return "BLACK"; }
    void        h4pClearEvent     (H4PE_TYPE e){}
#endif