#ifndef FSM_H
#define FSM_H

/*
 * Finite State Machine - for networking
 * 
 * stolen from NeoPixelClockV2
 * 
 * dependencies:
 * 	WiFi client instance
 *  mqttClient instance
 *  MDNS instance
 * 	httpServer instance
 * 	WebServer instance (?)
 *  WIFI_SSID, WIFI_PASS // replaced by SmartConfig
 *  const char* host
 * 	const char* clientID
 *  void subscribeTopics(void)
 * 
 * 	conditionally:
 * 		LEDPIN, and if so,
 * 		LEDON, LEDOFF.
 * 
 * ToDo:
 * 	clean up dependencies.
 */
 
#ifndef FSM_Serial
#define FSM_Serial false
#endif

boolean ntpInitialized = false;

// Connection FSM operational states
enum CONNECTION_STATE {
  CHECK_CONNECTION,
  CONNECT,
  CHECK_BROKER,
  BROKER_CONNECTED
};

// Current connection state
enum CONNECTION_STATE connectionState;

void SmartConfig() {
  WiFi.beginSmartConfig();
    while(1){
	  #if defined(LEDPIN)
        digitalWrite(LEDPIN, LEDON);
      #endif
      delay(500);
      #if defined(LEDPIN)
		digitalWrite(LEDPIN, LEDOFF);
	  #endif
      delay(500);
      if(WiFi.smartConfigDone()){
        if (FSM_Serial) Serial.println("SmartConfig Success");
        break;
      }
    }
}

void FSM(void) {
int i;

	if (FSM_Serial) Serial.printf("FSM(%d)\n",connectionState);
	
	switch (connectionState) {

	  case CHECK_CONNECTION :     // are we connected to WiFi?
		  if (WiFi.status() != WL_CONNECTED) {
			#if defined(LEDPIN)
				digitalWrite(LEDPIN,LEDOFF);   // Turn LED OFF
			#endif
			ntpInitialized = false;
			connectionState = CONNECT;
		  } else {
			connectionState = CHECK_BROKER;
		  }
		  break;

	  case CONNECT :              // not connected, attempt to connect, re-connect.
		  WiFi.mode(WIFI_STA);
		  #if defined (ESP32)
		    if(WiFi.setHostname(clientID)) {
			  Serial << "DHCP hostname: " << clientID << endl;      // set device hostname to clientID
			} else {
			  Serial << "Wifi.setHostname(" << clientID << ") failed!" << endl;
			}
		  #endif
		  #if defined (ESP8266)
			if (WiFi.hostname(clientID)) {
			  Serial << "DHCP hostname: " << clientID << endl;
			} else {
			  Serial << "Wifi.hostname(" << clientID << ") failed!" << endl;
			}
		  #endif
		  
//		  #if defined(ESP32)
//		  WiFi.begin(WIFI_SSID,WIFI_PASS);
//		  Serial << "Attempt to connect to WiFi network: " << WIFI_SSID << endl;
//		  #endif
		  
//		  #if defined(ESP8266)
		  WiFi.begin();	// attempt to use saved credentials
		  Serial << "Attempt to connect to WiFi network: " << WiFi.SSID() << endl;
//		  #endif
		  
		  for (i=30; ((WiFi.status() != WL_CONNECTED) && (i>0)); --i) {
			delay(400);
			Serial << ".";
		  }

		  if (i) {  // something left in i, we must be connected, do OTA stuff & advance connectionState
		  
		  Serial << "Connected to Wifi." << endl;

		  // DdelV added OTA stuff
		  MDNS.begin(clientID);
		  #if defined(ESP8266)
			httpUpdater.setup(&httpServer);
		  #endif
		  httpServer.begin();
		  MDNS.addService("http", "tcp", 80);

		  Serial << "OTA stuff set up." << endl;
		  
		  connectionState = CHECK_BROKER;
		  } else {	// go attempt SmartConfig, don't change state...
			  SmartConfig();
		  }
		  break;

	  case CHECK_BROKER :         // connected to WiFi, is MQTT broker connected?
			if (mqttClient.connected()) {
			  connectionState = BROKER_CONNECTED;
			} else {
			  if (mqttClient.connect(clientID, willTopic, willQoS, willRetain, willMsg)) {
				subscribeTopics();			// app-specific knows topics (could return boolean success)
				connectionState = BROKER_CONNECTED;
				Serial << "(Re-)connected to broker." << endl;
			  } else {
				connectionState = CHECK_CONNECTION;
				Serial << "Re-connect to broker failed!" << endl;
			  }
			}
			break;

		case BROKER_CONNECTED :     // Broker connected, do NTP
	          #if defined(LEDPIN)
				digitalWrite(LEDPIN, LEDON);          // turn LED ON
			  #endif
	// maybe this is the place to init NTP?
			   if (! ntpInitialized) {
				  // We are connected to the Internet for the first time so set NTP provider
				  initNTP();
				  ntpInitialized = true;
			   }
			  connectionState = CHECK_CONNECTION; // Check next time
			  break;
			  
		 default: // we ought never to reach here, but if we do,
			  connectionState = CHECK_CONNECTION;
			  break;
		   
	   }  // end switch
}
#endif
