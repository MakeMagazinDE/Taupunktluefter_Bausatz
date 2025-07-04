//////////////////////////////////////////////////////////////////////////////
// Das Taupunkt-Lüftungssystem 
// mit dem ESP32-CAM
// 
// veröffentlicht im MAKE Sonderheft 2/2023
//
// Ulrich Schmerold
// 8/2023
//////////////////////////////////////////////////////////////////////////////
//
// Die folgenden Prozeduren werden zur Fehlersuche eingesetzt (bzw. können zur Fehlersuche eingesetzt werden

void Serial_Debugging_println(String f){ if (debugging == true) Serial.println(f);}
void Serial_Debugging_println_and_wait(String f, int t){ if (debugging == true) {Serial.println(f);delay(t);}}
void Serial_Debugging_print(String f){ if (debugging == true) Serial.print(f);}
void Serial_Debugging_write(char c){ if (debugging == true) Serial.write(c);}
//-----------------------------------------------------------------------------------------------------------

void list_all_SPIFFS_files()
{
    if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  } 
  File root = SPIFFS.open("/"); 
  File file = root.openNextFile();
   
  while(file){ 
      Serial.print("FILE: ");
      Serial.print(file.name()); 
      Serial.print(" | Filesize: ");
      Serial.println(file.size());
      file = root.openNextFile();
  }
}

void print_Stack_size()
{
  Serial.printf("Arduino Stack was set to %d bytes\n", getArduinoLoopTaskStackSize());
  // Print unused stack for the task that is running setup()
  Serial.printf("Free Stack Space: %d\n", uxTaskGetStackHighWaterMark(NULL));
 
 // SET_LOOP_TASK_STACK_SIZE(16*1024); // 16KB
}

void Stack_Size()
{
  ///////////////////////////////////////////////////////
  // Hinweis: SET_LOOP_TASK_STACK_SIZE(16*1024); // 16KB
  // würde die Größe des Stacks auf 15KB vergrößern.
  // muss vor Setup() eingefügt werden
  //////////////////////////////////////////////////////
 void * StackPtrAtStart;
 void * StackPtrEnd;
 UBaseType_t watermarkStart;
 {
   void* SpStart = NULL;
   StackPtrAtStart = (void *)&SpStart;
   watermarkStart =  uxTaskGetStackHighWaterMark(NULL);
   StackPtrEnd = StackPtrAtStart - watermarkStart;

  Serial.printf("\r\n\r\nAddress of Stackpointer near start is:  %p \r\n",  (void *)StackPtrAtStart);
  Serial.printf("End of Stack is near: %p \r\n",  (void *)StackPtrEnd);
  Serial.printf("Free Stack near start is:  %d \r\n",  (uint32_t)StackPtrAtStart - (uint32_t)StackPtrEnd);
 
 void* SpActual = NULL;
 Serial.printf("Free Stack at actual position is: %d \r\n", (uint32_t)&SpActual - (uint32_t)StackPtrEnd);
 }
}
//-----------------------------------------------------------------------------------------------------------

void delay_Timer(String Text , int interval )
{
  int n = interval/100; 
  interval = interval/10;
  Serial.print("--> Wait without delay for "+Text + ' '+ String(n) +"sec.:");
  while (n>0)
  {
    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis > interval)
    {
     previousMillis = currentMillis;
     Serial.print(String(n) + '-'); 
     n--;
    }
  }Serial.println(" fertig!");
}
//-----------------------------------------------------------------------------------------------------------
/*
void soft_reset(byte Timeout)
{
  for (Timeout; Timeout > 0; Timeout--)
  { 
    Serial.println(" Neustart wird in " + String(Timeout) + " Sekunden ausgeführt");
    delay(1000);
  }
  ESP.restart();
}*/
//-------------------------------------------

void soft_reset(byte Timeout)
{
   oled.invertText(false);
   oled.setScale(1);
   OLED_clear();
   OLED_println("CPU Neustart in",1,2); 

  for (Timeout; Timeout > 0; Timeout--)
  { 
    oled.setScale(2);
    OLED_println(String(Timeout - 1),3,7);
    oled.setScale(1);
    OLED_println("Sekunden",6,4);
    OLED_update();
    delay(1000);
  }
  OLED_clear();
  OLED_update();
  ESP.restart();
}
//-----------------------------------------------------------------------------------------------------------


void print_Core_task_is_running(String Hinweistext)  //-Ermittelt, auf welchem Core die aktuelle Task läuft
{
  Serial.println(Hinweistext+ "Core-"+String(xPortGetCoreID()));
}
//-----------------------------------------------------------------------------------------------------------


void WiFiEvent(WiFiEvent_t event)
{
    //Serial.printf("[WiFi-event] event: %d\n", WiFiEvent);

    switch (event) {
        case ARDUINO_EVENT_WIFI_READY: 
            Serial.println("WiFi interface ready");
            break;
        case ARDUINO_EVENT_WIFI_SCAN_DONE:
            Serial.println("Completed scan for access points");
            break;
        case ARDUINO_EVENT_WIFI_STA_START:
            Serial.println("WiFi client started");
            break;
        case ARDUINO_EVENT_WIFI_STA_STOP:
            Serial.println("WiFi clients stopped");
            break;
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
            Serial.println("Connected to access point");
            break;
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            Serial.println("Disconnected from WiFi access point");
            break;
        case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE:
            Serial.println("Authentication mode of access point has changed");
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            Serial.print("Obtained IP address: ");
            Serial.println(WiFi.localIP());
            break;
        case ARDUINO_EVENT_WIFI_STA_LOST_IP:
            Serial.println("Lost IP address and IP address is reset to 0");
            break;
        case ARDUINO_EVENT_WPS_ER_SUCCESS:
            Serial.println("WiFi Protected Setup (WPS): succeeded in enrollee mode");
            break;
        case ARDUINO_EVENT_WPS_ER_FAILED:
            Serial.println("WiFi Protected Setup (WPS): failed in enrollee mode");
            break;
        case ARDUINO_EVENT_WPS_ER_TIMEOUT:
            Serial.println("WiFi Protected Setup (WPS): timeout in enrollee mode");
            break;
        case ARDUINO_EVENT_WPS_ER_PIN:
            Serial.println("WiFi Protected Setup (WPS): pin code in enrollee mode");
            break;
        case ARDUINO_EVENT_WIFI_AP_START:
            Serial.println("WiFi access point started");
            break;
        case ARDUINO_EVENT_WIFI_AP_STOP:
            Serial.println("WiFi access point  stopped");
            break;
        case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
            Serial.println("Client connected");
            break;
        case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
            Serial.println("Client disconnected");
            break;
        case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
            Serial.println("Assigned IP address to client");
            break;
        case ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED:
            Serial.println("Received probe request");
            break;
        case ARDUINO_EVENT_WIFI_AP_GOT_IP6:
            Serial.println("AP IPv6 is preferred");
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
            Serial.println("STA IPv6 is preferred");
            break;
        case ARDUINO_EVENT_ETH_GOT_IP6:
            Serial.println("Ethernet IPv6 is preferred");
            break;
        case ARDUINO_EVENT_ETH_START:
            Serial.println("Ethernet started");
            break;
        case ARDUINO_EVENT_ETH_STOP:
            Serial.println("Ethernet stopped");
            break;
        case ARDUINO_EVENT_ETH_CONNECTED:
            Serial.println("Ethernet connected");
            break;
        case ARDUINO_EVENT_ETH_DISCONNECTED:
            Serial.println("Ethernet disconnected");
            break;
        case ARDUINO_EVENT_ETH_GOT_IP:
            Serial.println("Obtained IP address");
            break;
        default: break;
    }
  }

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(IPAddress(info.got_ip.ip_info.ip.addr));
}

void print_Reset_reason()
{
     esp_reset_reason_t reason = esp_reset_reason();

    switch (reason) {
        case ESP_RST_UNKNOWN:
          Serial.println("Reset reason can not be determined");
        break;

        case ESP_RST_POWERON:
          Serial.println("Reset due to power-on event");
        break;

        case ESP_RST_EXT:
          Serial.println("Reset by external pin (not applicable for ESP32)");
        break;

        case ESP_RST_SW:
          Serial.println("Software reset via esp_restart");
        break;

        case ESP_RST_PANIC:
          Serial.println("Software reset due to exception/panic");
        break;

        case ESP_RST_INT_WDT:
          Serial.println("Reset (software or hardware) due to interrupt watchdog");
        break;

        case ESP_RST_TASK_WDT:
          Serial.println("Reset due to task watchdog");
        break;

        case ESP_RST_WDT:
          Serial.println("Reset due to other watchdogs");
        break;                                

        case ESP_RST_DEEPSLEEP:
          Serial.println("Reset after exiting deep sleep mode");
        break;

        case ESP_RST_BROWNOUT:
          Serial.println("Brownout reset (software or hardware)");
        break;
        
        case ESP_RST_SDIO:
          Serial.println("Reset over SDIO");
        break;
        
        default:
        break;
    }

}


//--------------------------------------- Watchdog --------------------------------------------
void Watchdog_reset()
{
  esp_task_wdt_reset();
}

void hw_wdt_set_timeout(byte seconds)
{
  esp_task_wdt_init(seconds, true);
}

void hw_wdt_disable(){
   Serial_Debugging_println(" Der Watchdog (  hw_wdt ) wird ausgeschaltet");
  *((volatile uint32_t*) 0x60000900) &= ~(1); // Hardware WDT OFF
}

void hw_wdt_enable(){
    Serial_Debugging_println(" Der Watchdog ( hw_wdt ) wird eingeschaltet");
  *((volatile uint32_t*) 0x60000900) |= 1; // Hardware WDT ON
}

//---------------------------- Internet Einstellungen auf Serial-Monitor ausgeben ----------
void Serial_print_Optionen()
{
  Serial.print("SSID:      ");Serial.println(ssid);
  Serial.print("password   ");Serial.println(password);
  Serial.print("local_IP:  ");Serial.println(local_IP);
  Serial.print("gateway:   ");Serial.println(gateway);
  Serial.print("dns1:      ");Serial.println(dns1);
  Serial.print("dns2:      ");Serial.println(dns2);
}
  
