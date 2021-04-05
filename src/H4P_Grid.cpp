/*
 MIT License

Copyright (c) 2020 Phil Bowles <h4plugins@gmail.com>
   github     https://github.com/philbowles/esparto
   blog       https://8266iot.blogspot.com     
   groups     https://www.facebook.com/groups/esp8266questions/
              https://www.facebook.com/Esparto-Esp8266-Firmware-Support-2338535503093896/
                			  

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

#include<H4P_Grid.h>

using H4P_GRID_MAP   = std::unordered_map<string,string>;
H4P_GRID_MAP        h4pGrid;

constexpr const char* xh4Tag(){ return "X-H4-Device"; }

void H4P_Grid::_handleEvent(const string& svc,H4PE_TYPE t,const string& msg){
    switch(t){
        case H4PE_UPNP:
            auto vs=split(msg,",");
            if(vs[1]==xh4Tag()){
                string ip=vs[0];
//                Serial.printf("H4PE_GRID MOVEMENT %s %s\n",CSTR(svc),CSTR(ip));
//                for(auto const& g:h4pGrid) reply("  %s %s",CSTR(g.first),CSTR(g.second));
                bool news=ip.size();
                bool known=h4pGrid.count(svc);
//                Serial.printf("H4PE_GRID MOVEMENT %d %d XOR %d\n",news,known,news^known);
                if(news) { if(!known) h4pGrid[svc]=ip; }
                else { if(known) h4pGrid.erase(svc); }
                if(news ^ known) {
                    h4psysevent(svc,H4PE_GRID,"%s",CSTR(ip));
                    h4p["grid"]=flattenMap(h4pGrid);
                }
            }
            break;
    }
}

void H4P_Grid::_init(){ 
    _pUPNP->_listenTag(xh4Tag(),"*");
    h4puiAdd("grid",H4P_UI_DROPDOWN,"s","Grid=H4");
}

#if H4P_LOG_MESSAGES
void H4P_Grid::info(){ 
    H4Service::info();
    if(h4pGrid.size()){
        reply(" H4 Grid:");
        for(auto const& g:h4pGrid) reply("  %s %s",CSTR(g.first),CSTR(g.second));
    }
}
#endif