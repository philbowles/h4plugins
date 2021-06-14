//https://github.com/mcauser/Adafruit_SSD1306
#define H4P_VERBOSE 1
#include<H4Plugins.h>
//
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop

#include <Adafruit_SSD1306.h>

#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);

void onRTC(){
   display.clearDisplay();
   display.setTextSize(1);
   display.setTextColor(WHITE);
}

H4P_SerialLogger h4sl;
H4P_PinMachine h4gm;
H4P_Signaller h4fc;
H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX");
H4P_AsyncMQTT h4mqtt("http://192.168.1.4:1883");
H4P_Timekeeper h4tk("192.168.1.4","0.fr.pool.ntp.org",60,H4P_Timekeeper::H4P_DST_EU); // 60 minutes cos France is GMT+1
H4P_UPNPServer h4upnp;

void h4setup(){
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
  display.display();
   
  h4.every(1000,[](){ 
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(23,0);
    display.println("H4");
    display.setTextSize(1);
    display.println("  Plugins");
    display.setCursor(16,28);
    display.println(H4P_VERSION);
    display.setCursor(8,40);
    display.println(CSTR(h4tk.clockTime()));

    display.display();
    });
}