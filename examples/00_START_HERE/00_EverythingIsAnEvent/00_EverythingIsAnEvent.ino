#define H4P_VERBOSE 1
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop

void h4pGlobalEventHandler(const string& svc,H4PE_TYPE t,const string& msg){
  Serial.printf("Big Brother sees EVERYTHING %s %s %s\n",CSTR(svc),CSTR(h4pGetEventName(t)),CSTR(msg));
  switch(t){
    case H4PE_NOOP:
      Serial.printf("Used for testing: you will never see this\n");
      break;
    // None of the SYSx events can be turned off / compiled out
    case H4PE_SYSINFO:
      Serial.printf("Important system information:\n");
      break;
    case H4PE_SYSWARN:
      Serial.printf("You need to check this out and fix it: App may not run correctly\n");
      break;
    case H4PE_SYSFATAL:
      Serial.printf("Impossible to continue: App enters flashing 'hangup loop'\n");
      break;
    case H4PE_MSG:
      Serial.printf("Probably the most common type you will see. Can be 'compiled out'\n");
      break;
    case H4PE_BOOT:
    case H4PE_STAGE2:
      Serial.printf("Used internally to start services - don't mess with it!\n");
      break;
    case H4PE_SERVICE:
      Serial.printf("A service has started (msg==\"1\") or stopped (msg==\"0\")\n");
      break;
    case H4PE_REBOOT:
      Serial.printf("App is about to reboot: save config (if any) etc\n");
      break;
    case H4PE_FACTORY:
      Serial.printf("App is about to 'Factory Reset': clear any saved config data etc\n");
      break;
    case H4PE_CMD:
      Serial.printf("A command came into the app via 'svc'\n");
      break;
    case H4PE_HEAP:
      Serial.printf("H4 publishes current heap value (usually for diagnostics)\n");
      break;
    case H4PE_Q:
      Serial.printf("H4 publishes current Queue size (usually for diagnostics)\n");
      break;
    case H4PE_LOOPS:
      Serial.printf("H4 publishes current NLoops/sec (usually for diagnostics / performance tuning)\n");
      break;
    case H4PE_PRESENCE:
      Serial.printf("A device has %s the network\n",STOI(msg) ? "joined":"left");
      break;
    case H4PE_GV_CHANGE:
      Serial.printf("A persistent storage variable %s has changed its value to %s\n",CSTR(svc),CSTR(msg));
      break;
    case H4PE_GPIO:
      Serial.printf("A hardware event hs occured on pin 'svc': its current value is in msg\n");
      break;
    case H4PE_SIGNAL:
      Serial.printf("Send flashing pattern to default signalling device, usually LED_BUILTIN\n");
      break;
    case H4PE_VIEWERS:
      Serial.printf("Web UI has 'msg' number of viewers. When 1, set up any resources, when 0 clear / reset them\n");
      break;
    case H4PE_HEARTBEAT:
      Serial.printf("H4 has sent a synchronous 1-second, on-the-second 'ping'\n");
      break;
    default:
      Serial.printf("Something terrible has gone wrong: no such type '%u'!\n",t);
  }
}