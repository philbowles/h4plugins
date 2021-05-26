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

    H4_INT_MAP taskTypes={
        {3,"evry"}, // 3
        {4,"evrn"}, // 4
        {5,"ntim"}, // 5
        {6,"ntrn"}, // 6
        {7,"once"}, // 7
        {8,"1xrn"}, // 8
        {9,"qfun"}, // 9
        {10,"rntx"}, // 10
        {11,"rnrn"}, // 11
        {12,"rptw"}, // 12
        {13,"rpwe"}  // 13
    };
        
    H4_INT_MAP taskNames{
        {H4P_TRID_THRB,"THRB"},
        {H4P_TRID_PATN,"PATN"},
        {H4P_TRID_PP1x,"PP1X"},
        {H4P_TRID_PWM1,"PWM1"},
        {H4P_TRID_DBNC,"DBNC"},
        {H4P_TRID_RPTP,"RPTP"},
        {H4P_TRID_POLL,"POLL"},
        {H4P_TRID_HOTA,"HOTA"},
        {H4P_TRID_MQMS,"MQMS"},
        {H4P_TRID_MQRC,"MQRC"},
        {H4P_TRID_REST,"REST"},
        {H4P_TRID_SOAP,"SOAP"},
        {H4P_TRID_UDPS,"UDPS"},
        {H4P_TRID_NTFY,"UNFY"},
        {H4P_TRID_SCMD,cmdTag()},
        {H4P_TRID_HLOG,"HLOG"},
        {H4P_TRID_QLOG,"QLOG"},
        {H4P_TRID_BTTO,"BTTO"},
        {H4P_TRID_IPPD,"IPPD"},
        {H4P_TRID_DALY,"DALY"},
        {H4P_TRID_SHOT,"AT_T"},
        {H4P_TRID_SSET,"SSET"},
        {H4P_TRID_SYNC,"SYNC"},
        {H4P_TRID_TIME,uppercase(timeTag())},
        {H4P_TRID_GATE,"GATE"},
        {H4P_TRID_BOOT,"BOOT"}
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

    extern const char* giveTaskName(uint32_t id);

    string h4pGetErrorMessage(uint32_t e){ return cmdErrors.count(e) ? cmdErrors[e]:string("No such error (")+stringFromInt(e)+")"; }
    string h4pGetEventName   (H4PE_TYPE e){ return eventTypes.count(e) ? eventTypes[e]:""; }
    string h4pGetTaskType    (uint32_t e){ return taskTypes.count(e) ? taskTypes[e]:stringFromInt(e,"?%02d?"); }
    string h4pGetTaskName    (uint32_t e){ return taskNames.count(e) ? taskNames[e]:giveTaskName(e); }
    void   h4pClearEvent     (H4PE_TYPE e){ eventTypes.erase(e); }
#else
    string h4pGetErrorMessage(uint32_t e){ return string("Err:"+stringFromInt(e)); }
    string h4pGetEventName   (H4PE_TYPE e){ return stringFromInt(e,"0x%08x"); }
    string h4pGetTaskType    (uint32_t e){ return stringFromInt(e,"%04d"); }
    string h4pGetTaskName    (uint32_t e){ return stringFromInt(e,"%04d"); }
    void   h4pClearEvent     (H4PE_TYPE e){}
#endif