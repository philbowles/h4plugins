#include <H4Plugins.h>

//**** Set these variables to match local.
#define LocalTZO 60  // Number of minutes that your local time is ahead (+) or behind (-) UTC/GMT.  Do not use FP maths here, just pre-calculate the number of minutes.
#define DSTEU // Does your local time require a DST adjustment? EU/USA only supported currently. Comment out line if DST not required. EU also = UK.

#define NTP1 "0.pool.ntp.org"  // First NTP server to query time information from.
#define NTP2 "1.pool.ntp.org"  // Second NTP server to query time information from.


void onWiFiConnect( void );
void onWiFiDisconnect( void );

H4_USE_PLUGINS(74880, 20, false) // Serial baud rate, Q size, SerialCmd autostop
//H4P_SerialLogger h4sl;  // Serial logger.
H4P_WiFi h4wifi("XXXXXXXX", "XXXXXXXX", "NTPClock", onWiFiConnect, onWiFiDisconnect);  // Configure and actyivate the WiFi Interface.
H4P_Timekeeper h4tk(NTP1, NTP2, LocalTZO);  // Time support.

boolean ClockValid = false;  // Flag indicating whether to display the clock or not.
boolean WiFiValid = false;  // Flag indicating a valid WiFi Connection is active.

//******************************************************************************
//*** Start displaying the time once we're sync'd.
//******************************************************************************
void onRTC(){

  Serial.println("Clock valid!");
  
  ClockValid = true;
  Serial.printf("\nReceived NTP time: %s (UTC)\n\n", CSTR(h4tk.strfDateTime( "%a %Y-%m-%d %H:%M:%S", h4tk.clockEPOCHLocal()+ h4tk.DST_EU( h4tk.clockEPOCHLocal() )) ));

//Serial.print("TZO = ");
//Serial.println(h4tk._tzo);

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
    Serial.printf(  "  DST Offset: %d (seconds) for EU/UK\n", h4tk.DST_EU( h4tk.clockEPOCHLocal() ));
    Serial.printf(  "DST Time  : %d = %s\n", h4tk.clockEPOCHLocal(), CSTR(h4tk.strfDateTime( "%a %Y-%m-%d %H:%M:%S", h4tk.clockEPOCHLocal() + h4tk.DST_EU( h4tk.clockEPOCHLocal() ) )) );
#endif
#ifdef DSTUSA
    Serial.printf(  "  DST Offset: %d (minutes) for USA\n", h4tk.DST_USA( h4tk.clockEPOCHLocal() ));
    Serial.printf(  "DST Time  : %d = %s\n", h4tk.clockEPOCHLocal(), CSTR(h4tk.strfDateTime( "%a %Y-%m-%d %H:%M:%S", h4tk.clockEPOCHLocal() + h4tk.DST_USA( h4tk.clockEPOCHLocal() ) )) );
#endif
      
//    Serial.printf("Clock time: %d - %d - %s - %s\n", h4tk.clockEPOCH(), h4tk.clockEPOCHLocal(), CSTR(h4tk.strfDateTime( "%a %Y-%m-%d %H:%M:%S", h4tk.clockEPOCHLocal()) ), CSTR(h4tk.strfDateTime( "%H:%M:%S", h4tk.clockEPOCHLocal()+ h4tk.DST_EU( h4tk.clockEPOCHLocal() )) ));
  } else {
    Serial.println("No NTP!");
  }

}

//******************************************************************************
//*** Main setup routine.
//******************************************************************************

void h4setup( void ) {

  //**** Activate the heartbeat with the onboard LED.
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, LOW);

  //**** Activate a task to flash the local LED.
  h4.every(100, [](){
    LEDHeartbeat();
  });
  
  //**** Activate a task to display the time.
  h4.everyRandom(1000, 5000, [](){ 
    ShowTime();
  });

}

//******************************************************************************
//*** Breifly pulse the LED to indicate good operation.
//******************************************************************************
void LEDHeartbeat( void ) {
  
  static uint8_t LEDCount = 0;

  //**** Reset the count if it has overflowed.
  if (++LEDCount >=  10) LEDCount = 0;

  //**** Set the LED depending on the current count.
  switch (LEDCount) {
    case 0:
      digitalWrite( LED_BUILTIN, LOW );  // LED ON.
      break;
    default:
      digitalWrite( LED_BUILTIN, HIGH );  // LED OFF.
      break;
  }

}

//******************************************************************************
//*** 
//******************************************************************************

//******************************************************************************
//*** 
//******************************************************************************

//******************************************************************************
//*** 
//******************************************************************************
