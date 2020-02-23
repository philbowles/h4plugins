/*
 MIT License

Copyright (c) 2019 Phil Bowles <H48266@gmail.com>
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
#ifndef H4P_CmdErrors_HO
#define H4P_CmdErrors_HO

#include<H4PCommon.h>

class H4P_CmdErrors: public H4Plugin {
        H4_INT_MAP  cmdErrors={
            {H4_CMD_OK,"OK"},
            {H4_CMD_UNKNOWN,"Unknown cmd"},
            {H4_CMD_TOO_FEW_PARAMS,"Too few parameters"},
            {H4_CMD_TOO_MANY_PARAMS,"Too many parameters"},
            {H4_CMD_NOT_NUMERIC,"Numeric value expected"},
            {H4_CMD_OUT_OF_BOUNDS,"Value out of range"},
            {H4_CMD_NAME_UNKNOWN,"Name not known"},
            {H4_CMD_PAYLOAD_FORMAT,"Incorrect Payload Format"}
//            {H4_CMD_PROHIBITED,"Prohibited from here"}
        };
//
        H4_INT_MAP  logTypes={
            {H4P_LOG_H4,"H4"},
            {H4P_LOG_SVC_UP,"SVC UP"},
            {H4P_LOG_SVC_DOWN,"SVC DOWN"},
            {H4P_LOG_CMD,"CMD"},
            {H4P_LOG_USER,"USER"},
            {H4P_LOG_DEPENDFAIL,"DEPEND FAIL"},
            {H4P_LOG_MQTT_HEAP,"MQTT HEAP"},
            {H4P_LOG_MQTT_Q,"MQTT Q"},
            {H4P_LOG_ERROR,"ERROR"}
        };
    public:
        H4P_CmdErrors(){ _pid=cerrTag(); }//?

        string      getErrorMessage(uint32_t e){
            return cmdErrors.count(e) ? cmdErrors[e]:string("No such error (")+stringFromInt(e)+")";
        }

        string      getLogType(uint32_t e){
            return logTypes.count(e) ? logTypes[e]:string("No such type (")+stringFromInt(e)+")";
        }
};

extern __attribute__((weak)) H4P_CmdErrors h4ce;

#endif // H4P_CmdErrors_H