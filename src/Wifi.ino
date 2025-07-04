//////////////////////////////////////////////////////////////////////////////
// Das Taupunkt-Lüftungssystem
// mit dem ESP32-CAM
//
// veröffentlicht im MAKE Sonderheft 2/2023
//
// Ulrich Schmerold
// 8/2023
//////////////////////////////////////////////////////////////////////////////
#define interval_reconnect 60000000  // eine Minute
unsigned long Start_Millis = millis();
 
const char* ntpServer1 = "pool.ntp.org";                // 1. Zeitserver
const char* ntpServer2 = "time.nist.gov";               // 2. Zeitserver
const char* time_zone = "CET-1CEST,M3.5.0,M10.5.0/3";   // TimeZone rule for Europe/Rome including daylight adjustment rules (optional)

void notFound(AsyncWebServerRequest *request) {request->send(404, "text/plain", "Not found");}

void SerialprintWifiStatus()
{
  Serial.print("* SSID: ");
  Serial.println(WiFi.SSID());
  Serial.println("* IP Addresse: " + WiFi.localIP().toString());
  Serial.println("* Signalstärke (RSSI):" + String(WiFi.RSSI()) + " dBm");
}

unsigned long get_Local_Time()
{
  struct tm timeinfo;
  int n = 0;
  bool erfolg = true;

 
  configTzTime(time_zone, ntpServer1, ntpServer2);

  OLED_println("Suche Internetzeit", 6, 0);
  OLED_setCursor(7, 0);
  Serial_Debugging_println("Suche Internetzeit");

  while ( (n < 10) and (!getLocalTime(&timeinfo)))
  {   
    n++;
    OLED_print("X");
    OLED_update();
    Serial_Debugging_print("X");
    delay(500);
  }
  if (n > 9) {
    OLED_println("Keine Zeit ermittelt", 7, 0);
    Serial_Debugging_println("Keine Zeit ermittelt");
    erfolg = false;
  } else {
    rtc.setTimeStruct(timeinfo);
    make_time_stamp ();
    OLED_println(stamp, 6, 0);
    Serial_Debugging_println(stamp);
  }
  delay(delay_interval_3);
  if (erfolg) return rtc.getEpoch(); else return 0;
}

String make_time_stamp ()
{
 stamp= rtc.getTime("%d.%m.%Y %H:%M:%S");
 return stamp;
}



void WIFI_Connect()
{ 
  OLED_clear();
  OLED_println("WIFI-1", 0, 7);
  OLED_println("Verbinde: ",2,0);
  OLED_println(ssid, 3, 0);
  OLED_update();
  Serial_Debugging_println("Verbinde zu: " + String(ssid));
  WiFi.config(local_IP, gateway, subnet, dns1, dns2 ) ;
  WiFi.onEvent(WiFiEvent);
  WiFi.begin(ssid, password);
  
  OLED_setCursor(3, 0);
  //--------------------------------------------------   Verbindungsversuche ins Netz-----------------------------------------------
  byte n = 0;
  while ((WiFi.status() != WL_CONNECTED) and (n < 14))
  {    
    n++;
    OLED_setCursor(4,n-1);
    drawSonderzeichen(4,false);
    OLED_update();
    Serial_Debugging_print(".");
    delay(500);
  } Serial_Debugging_println("");

  if (WiFi.status() == WL_CONNECTED) //--------------------- WIFI wurde verbunden --------------------------------------------------
  {
    OLED_println("WIFI ist verbunden", 4, 0);
    Serial_Debugging_println("WIFI ist verbunden");

    // -------------------------------------------------  Zeit aus dem Internet holen und RTC damit einstellen----------------------
    get_Local_Time();
    // ---------------------------------------   Jetzt wird der Webserver gestartet und der request angelegt ------------------------
    Serial_Debugging_println("Server Starten");
    WiFi.onEvent(WiFiEvent);
    server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) 
     { 
        Serial.print("Received request from client with IP: ");
        Serial.println(request->client()->remoteIP());

       HTML_processor_is_working = true;
       Serial_Debugging_println("HTML-Seite wird angefordert");
       delay(1);  // um den Watchdog zurückzusetzen 
       request->send_P(200, "text/html", index_html, processor);
     });
    server.onNotFound(notFound);
    server.begin();

    if (debugging) SerialprintWifiStatus();

  } else { //------------------------------------------------ WIFI wurde nicht verbunden ---------------------------------------------
    OLED_println("nicht verbunden!", 4, 0);
    Serial_Debugging_println("WIFI ist nicht verbunden");
    use_WIFI = false;  // kein erneuter Versuch, ins Netz zu kommen
  }
  OLED_update();
 
  delay(delay_interval_2);
}
//-------------------------------------------------------------------------------------------------------------------------------------

void WIFI_watch_Task() {

  if(millis() - Start_Millis > interval_reconnect)
  {
     Start_Millis = millis(); // Zähler zurücksetzen
     own_WIFI_reconnect();
   }
} 
//-------------------------------------------------------------------------------------------------------------------------------------

void own_WIFI_reconnect()
{
     if (WiFi.status() != WL_CONNECTED)
      {
          WiFi.disconnect();
          Serial_Debugging_println("WIFI ist unterbrochen! Versuche neu zu verbinden");
          OLED_clear();
          OLED_println("WIFI ist unterbrochen", 0, 0);
          OLED_println("Versuche zu verbinden", 1, 0);
          Serial_Debugging_print("Verbindungsversuch: ");
          byte n=0;
          while ((WiFi.status() != WL_CONNECTED) and (n < 10))
          {    
            n++;
            Serial_Debugging_print( String(n) + "-");
            OLED_println("Versuch " + String(n), 3, 0);
            OLED_update();
            WiFi.config(local_IP, gateway, subnet, dns1, dns2 ) ;
            WiFi.begin(ssid, password);
            delay(1000);
          } 
          Serial_Debugging_println("");
          if (n > 9) {
            OLED_println("Leider kein Erfolg!", 4, 0);
            OLED_println("ESP32 Neustart.....",6,0);
            OLED_update();
            soft_reset(3);
          }
      }
}
//-------------------------------------------------------------------------------------------------------------------------------------
