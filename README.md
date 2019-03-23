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
  
