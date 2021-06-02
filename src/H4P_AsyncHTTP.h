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

#include<H4Service.h>
#include<ArmadilloHTTP.h>

class H4P_AsyncHTTP: public H4Service, public ArmadilloHTTP {
    protected:
        virtual void    _errorHandler(int e,int i){ h4psysevent(_me,H4PE_SYSWARN,"e=%d i=%d",e,i); }
    public:
        H4P_AsyncHTTP(const std::string& name="http",uint32_t filter=H4PE_NOOP);

        virtual void    DELETE(const std::string& url,ARMA_FN_HTTP rx,const uint8_t* fingerprint=nullptr,uint32_t phase=ARMA_PHASE_EXECUTE) override;
        virtual void    GET(const std::string& url,ARMA_FN_HTTP rx,const uint8_t* fingerprint=nullptr,uint32_t phase=ARMA_PHASE_EXECUTE) override;
        virtual void    PATCH(const std::string& url,const VARK_NVP_MAP& fields,ARMA_FN_HTTP rx,const uint8_t* fingerprint=nullptr,uint32_t phase=ARMA_PHASE_EXECUTE) override;
        virtual void    POST(const std::string& url,const VARK_NVP_MAP& fields,ARMA_FN_HTTP rx,const uint8_t* fingerprint=nullptr,uint32_t phase=ARMA_PHASE_EXECUTE) override;
        virtual void    PUT(const std::string& url,const VARK_NVP_MAP& fields,ARMA_FN_HTTP rx,const uint8_t* fingerprint=nullptr,uint32_t phase=ARMA_PHASE_EXECUTE) override;

                void    globalsFromSimpleJson(ARMA_HTTP_REPLY r);
};