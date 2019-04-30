# netFSM
Finite State Machine handles WiFi, NTP &amp; broker connection w/SmartConfig

I stole the bones of this from NeoPixelClock_V2 with many changes, additions.

Still a work in progress.

This has a lot of "dependencies", in the sense of items which ought/must be defined prior to its insertion in code:

 WiFi client instance
 mqttClient instance
 MDNS instance
 httpServer instance
 WebServer instance (?)
 
 const char* host
 const char* clientID
  
 void subscribeTopics(void)
 
 conditionally:
  LEDPIN, (if not defined, LED will not be used) and if so,
  LEDON, LEDOFF. (state of LED, allows for positive/negative logic in both senses)
  
# Usage

1. define all dependencies (the way you like them)
   There is no longer any need to supply SSID or passphrase for networking.
2. #include <FSM.h>
3. in loop(), call FSM()
