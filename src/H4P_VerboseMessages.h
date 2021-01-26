/*
 MIT License

Copyright (c) 2020 Phil Bowles <H48266@gmail.com>
   github     https://github.com/philbowles/H4
   blog       https://8266iot.blogspot.com
   groups     https://www.facebook.com/groups/esp8266questions/
              https://www.facebook.com/H4-Esp8266-Firmware-Support-2338535503093896/


Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/
#pragma once

#include<H4PCommon.h>

extern const char* giveTaskName(uint32_t id);
class H4P_VerboseMessages: public H4Plugin {
    public:
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
            {H4P_EVENT_NOOP,"NO-OP"},
            {H4P_EVENT_MSG,"MSG"},
            {H4P_EVENT_SVC_UP,"SVCUP"},
            {H4P_EVENT_SVC_DOWN,"SVCDOWN"},
            {H4P_EVENT_CMD,"CMD"},
            {H4P_EVENT_USER,"USER"},
            {H4P_EVENT_HEAP,"HEAP"},
            {H4P_EVENT_Q,"Q"},
            {H4P_EVENT_LOOPS,"LOOPS"},
            {H4P_EVENT_PD_ENTER,"PDENTER"},
            {H4P_EVENT_PD_LEAVE,"PDLEAVE"},
            {H4P_EVENT_MQTT_ERROR,"MQTTERROR"},
            {H4P_EVENT_REBOOT,"REBOOT"},
            {H4P_EVENT_FACTORY,"FACTORY"},
            {H4P_EVENT_CMDREPLY,"CMDREPLY"},
            {H4P_EVENT_DLL,"DLL"},
            {H4P_EVENT_ON,"ON"},
            {H4P_EVENT_OFF,"OFF"},
            {H4P_EVENT_H4_ENTER,"H4ENTER"},
            {H4P_EVENT_H4_LEAVE,"H4LEAVE"},
            {H4P_EVENT_HEARTBEAT,"PING"}
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
            {H4P_TRID_PATN,"PATN"},
            {H4P_TRID_PP1x,"PP1X"},
            {H4P_TRID_PWM1,"PWM1"},
            {H4P_TRID_DBNC,"DBNC"},
            {H4P_TRID_RPTP,"RPTP"},
            {H4P_TRID_POLL,"POLL"},
            {H4P_TRID_MULT,"MULT"},
            {H4P_TRID_TRIG,"TRIG"},
            {H4P_TRID_HOTA,"HOTA"},
            {H4P_TRID_WFAP,"WFAP"},
            {H4P_TRID_MQMS,"MQMS"},
            {H4P_TRID_MQRC,"MQRC"},
            {H4P_TRID_REST,"REST"},
            {H4P_TRID_SOAP,"SOAP"},
            {H4P_TRID_UDPS,"UDPS"},
            {H4P_TRID_UDPU,"UDPU"},
            {H4P_TRID_NTFY,"UNFY"},
            {H4P_TRID_SCMD,cmdTag()},
            {H4P_TRID_HLOG,"HLOG"},
            {H4P_TRID_QLOG,"QLOG"},
            {H4P_TRID_MLRQ,"MLRQ"},
            {H4P_TRID_BTTO,"BTTO"},
            {H4P_TRID_IPPD,"IPPD"},
            {H4P_TRID_TIME,"TIME"},
            {H4P_TRID_SYNC,"SYNC"},
            {H4P_TRID_DALY,"DALY"},
            {H4P_TRID_SHOT,"AT_T"},
            {H4P_TRID_SSET,"SSET"}
        };
        H4P_VerboseMessages(): H4Plugin(H4PID_VM) {}
        string getErrorMessage(uint32_t e){
            return cmdErrors.count(e) ? cmdErrors[e]:string("No such error (")+stringFromInt(e)+")";
        }

        string getEventName(H4P_EVENT_TYPE e){
            return eventTypes.count(e) ? eventTypes[e]:string("No such event (")+stringFromInt(e)+")";
        }

        string getTaskType(uint32_t e){
            return taskTypes.count(e) ? taskTypes[e]:stringFromInt(e,"?%02d?");
        }

        string getTaskName(uint32_t e){
            return taskNames.count(e) ? taskNames[e]:giveTaskName(e);
        }
};