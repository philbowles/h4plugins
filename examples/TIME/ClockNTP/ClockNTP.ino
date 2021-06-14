#include <H4Plugins.h>
H4_USE_PLUGINS(115200, 20, false) // Serial baud rate, Q size, SerialCmd autostop

//**** Set these variables to match local.
#define LocalTZO 60  // Number of minutes that your local time is ahead (+) or behind (-) UTC/GMT.  Do not use FP maths here, just pre-calculate the number of minutes.
#define DSTEU // Does your local time require a DST adjustment? EU/USA only supported currently. Comment out line if DST not required. EU also = UK.

#define NTP1 "0.pool.ntp.org"  // First NTP server to query time information from.
#define NTP2 "1.pool.ntp.org"  // Second NTP server to query time information from.

//void onWiFiConnect( void );
//void onWiFiDisconnect( void );
H4P_WiFi h4wifi("XXXXXXXX", "XXXXXXXX", "NTPClock");  // Configure and actyivate the WiFi Interface.
H4P_Timekeeper h4tk(NTP1, NTP2, LocalTZO);  // Time support.

boolean ClockValid = false;  // Flag indicating whether to display the clock or not.
boolean WiFiValid = false;  // Flag indicating a valid WiFi Connection is active.

//******************************************************************************
//*** Start displaying the time once we're sync'd.
//******************************************************************************
void onRTC(){

  Serial.println("Clock valid!");
  
  ClockValid = true;
  Serial.printf("\nReceived NTP time: %s (UTC)\n\n", CSTR(h4tk.strfDateTime( "%a %Y-%m-%d %H:%M:%S", h4tk.clockEPOCHLocal()+ h4tk.H4P_DST_EU( h4tk.clockEPOCHLocal() )) ));

}

//******************************************************************************
//*** WiFi Connection Established.
//******************************************************************************
void onWiFiConnect( void ) {

  Serial.println("WiFi Connected!");
  WiFiValid = true;

  Serial.printf( "IP4 Address = " );
  Serial.println( WiFi.localIP() );
  
}

//******************************************************************************
//*** WiFi Connection Lost.
//******************************************************************************
void onWiFiDisconnect( void ) {

  Serial.println("WiFi Disconnected!");
  WiFiValid = false;

}

//******************************************************************************
//*** Show the time again.
//******************************************************************************

void ShowTime( void ) {

  //**** Dsiplay the current time.
  if (ClockValid) {
    Serial.printf("\nEPOCH Time: %d = %s (UTC)\n", h4tk.clockEPOCH(), CSTR(h4tk.strfDateTime( "%a %Y-%m-%d %H:%M:%S", h4tk.clockEPOCH() )) );
    Serial.printf(  "Local Time: %d = %s\n", h4tk.clockEPOCHLocal(), CSTR(h4tk.strfDateTime( "%a %Y-%m-%d %H:%M:%S", h4tk.clockEPOCHLocal() )) );
#ifdef DSTEU
    Serial.printf(  "  DST Offset: %d (seconds) for EU/UK\n", h4tk.H4P_DST_EU( h4tk.clockEPOCHLocal() ));
    Serial.printf(  "DST Time  : %d = %s\n", h4tk.clockEPOCHLocal(), CSTR(h4tk.strfDateTime( "%a %Y-%m-%d %H:%M:%S", h4tk.clockEPOCHLocal() + h4tk.H4P_DST_EU( h4tk.clockEPOCHLocal() ) )) );
#endif
#ifdef DSTUSA
    Serial.printf(  "  DST Offset: %d (minutes) for USA\n", h4tk.H4P_DST_USA( h4tk.clockEPOCHLocal() ));
    Serial.printf(  "DST Time  : %d = %s\n", h4tk.clockEPOCHLocal(), CSTR(h4tk.strfDateTime( "%a %Y-%m-%d %H:%M:%S", h4tk.clockEPOCHLocal() + h4tk.H4P_DST_USA( h4tk.clockEPOCHLocal() ) )) );
#endif
      
//    Serial.printf("Clock time: %d - %d - %s - %s\n", h4tk.clockEPOCH(), h4tk.clockEPOCHLocal(), CSTR(h4tk.strfDateTime( "%a %Y-%m-%d %H:%M:%S", h4tk.clockEPOCHLocal()) ), CSTR(h4tk.strfDateTime( "%H:%M:%S", h4tk.clockEPOCHLocal()+ h4tk.H4P_DST_EU( h4tk.clockEPOCHLocal() )) ));
  } else {
    Serial.println("No NTP!");
  }

}

//******************************************************************************
//*** Main setup routine.
//******************************************************************************
h4pOutput builtin(LED_BUILTIN,H4P_ASSUMED_SENSE,OFF,H4P_ASSUMED_COLOR);


H4P_EventListener clok(H4PE_RTC | H4PE_ALARM,[](const std::string& svc,H4PE_TYPE t,const std::string& msg){
  switch(t){
    H4P_SERVICE_ADAPTER(WiFi);
    H4P_FUNCTION_ADAPTER_VOID(RTC);
  }
});

void h4setup( void ) {

  //**** Activate a task to flash the local LED.
  h4.every(250, [](){
    builtin.toggle();
  });
  
  //**** Activate a task to display the time.
  h4.everyRandom(1000, 5000, [](){ 
    ShowTime();
  });

}