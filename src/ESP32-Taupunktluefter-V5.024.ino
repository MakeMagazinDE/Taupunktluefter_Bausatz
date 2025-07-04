//////////////////////////////////////////////////////////////////////////////
// Das Taupunkt-Lüftungssystem 
// mit dem ESP32 Wroom 32
// 
//
// Ulrich Schmerold
// 9/2024
//////////////////////////////////////////////////////////////////////////////
#define Software_version "5.24"

#define use_OLED true

// Dieser Code benötig zwingend die folgenden Libraries:
#include <ToneESP32.h>
#include <DHT.h>
#include <time.h>
#include <ESP32Time.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>
#include <GyverOLED.h>
#include <esp_task_wdt.h>

GyverOLED<SSH1106_128x64> oled;
AsyncWebServer server(80);

ESP32Time rtc;  
String stamp;  // Variable für den Zeitstempel.
ToneESP32 buzzer(19, 0);

//************************************** WIFI ***************************************************************
char ssid[] = "FritzUS";                  // Hier die eigene SSID eintragen (RouterName)
char password[] = "48094564919405773644"; // Hier das eigene WLan-Passwort einfügen

IPAddress local_IP(192, 168, 178, 112);       // Diese IP-Adrese bekommt der Taupunkt-Lüfter
IPAddress gateway (192, 168, 178, 1);         // Gateway ins Internet
IPAddress subnet  (255, 255, 255, 0);         // Subnet-Mask
IPAddress dns1    (192, 168, 178, 1);         // eigener DMS-Server
IPAddress dns2    (  8,   8,   8, 8);         // DNS-Server von Google

//************************************* Prototyping (geordet nach Karteireiter  *******************************
//------------------------ Chronik -----------------------
unsigned int SPIFFS_print_space();
String wert_hinzufuegen(String, float);
void chronik_update_all();
void chronik_load_all();
String chronic_update (String , float);
bool format_SPIFFS();
//------------------------ Debugging ---------------------
void Serial_Debugging_println(String);
void Serial_Debugging_print(String);
void soft_reset(byte = 0);
void hw_wdt_disable();
void print_Reset_reason();
void print_Core_task_is_running(String);
void hw_wdt_set_timeout(byte WDT_TIMEOUT);
void delay_Timer(String Text = "" , int interval = 1000);
void list_all_SPIFFS_files();
void Serial_print_Optionen();
void Watchdog_reset();
//------------------------------ HTML -----------------------
String processor(const String& var);
//------------------------------ OLED -----------------------
void OLED_init();
void OLED_update();
void OLED_clear();
void OLED_print(String Text);
void OLED_println(String Text, byte zeile = 0, byte Spalte = 0); 
void drawSonderzeichen(byte, bool = false);
void OLED_setCursor(byte Zeile , byte Spalte = 0 );
void show_Kontaktbildschirm();
void show_Wertebildschirm();
void show_Status();
void OLED_Draw_Bitmap();
void OLED_Draw_Logo();
//------------------------------ Wifi -------------------------
unsigned long get_Local_Time();
String make_time_stamp ();
void WIFI_Connect();
void WiFiEvent(WiFiEvent_t event);
void WIFI_watch_Task();
void own_WIFI_reconnect();
// ----------------------- RotaryEncoder-----------------------
void Encoder_initialisieren();
volatile int get_Encoder_pos(int min, int max, bool ueberlauf);
void reset_Encoder(int pos = 0);
void checkPosition();
void checkButton();
void timer_initialisieren();
void IRAM_ATTR onTimer();
void Timer_off();
void Timer_on();
//------------------------ Ampel -------------------------------
void setup_Ampel();
void blink_LED(byte n, byte z);
void LED_Test();
void Heardbeat();
void set_Ampel_1(byte color);
void set_Ampel_2(byte color, byte Helligkeit);
void Set_LED1();
void Set_LED2();
void Set_LED3();
// ----------------------- Optionen ---------------------------
void show_menue();
void show_SubMenue();
void  show_Eingabe_Bildschirm();
void alle_Optionen_speichern();
bool Save_one_Item(byte item);
void Optionen_laden();
void Optionen_loeschen();
void Optionen_Serial_ausgeben();
void Optionen_zu_Variablen();
byte String_to_MenuePos(String s);
bool Wartung(byte MenueItem);
void Optionen_reset();
void SPIFFS_formatieren();
//------------------------- WerteWandel ------------------------
bool StringVergleich(String A, String B);
String float_to_String(float f);
float String_to_float(String s);
String String_laenge_ergaenzen(String S, byte N);
int String_to_int( String S);
bool Bool(String s);
//-------------------------Taupunktberechnung ------------------
void Taupunkte_berechnen();

//************************************************** Prototyping Ende *********************************

#define wdt_timeout 60                  // Time-Out-Zeit des Hardware Watchdog in Sekunden
bool chronik_is_busy = false;           // Zeigt an, dass gerade die Chronik-Daten geladen oder gespeichert werden
bool HTML_processor_is_working = false; // Zeigt an, dass gerade eine Webseite übertragen wird

int delay_interval_2 = 4000; // Dauer allgemeine Anzeigen
int delay_interval_3 = 1500; // Dauer allgemeine Anzeigen

#define RELAIPIN 26       // Anschluss des LüfterRelais
#define DHTPIN_1 15       // Datenleitung für den DHT Sensor 1 (innen)
#define DHTPIN_2 4        // Datenleitung für den DHT Sensor 2 (außen)
#define DHT_Power_Pin 23  //Stromversorgung der DHT-Sensoren

#define RELAIS_EIN HIGH
#define RELAIS_AUS LOW
bool rel = false; 
bool fehler = true;
char focus = 0;
//************************************** Sensoren *****************************************
#define DHTTYPE_1 DHT22   // DHT 22 
#define DHTTYPE_2 DHT22   // DHT 22  
DHT dht1(DHTPIN_1, DHTTYPE_1);  //Der Innensensor wird ab jetzt mit dht1 angesprochen
DHT dht2(DHTPIN_2, DHTTYPE_2);  //Der Außensensor wird ab jetzt mit dht2 angesprochen
float t1, h1, t2, h2, Taupunkt_1, Taupunkt_2, DeltaTP;  // Variablen für die Sensor und Taupunktwerte

// ***************************   Korrekturwerte der einzelnen Sensoren*****  ***************
float Korrektur_t_1 = 0;   // Korrekturwert Innensensor Temperatur
float Korrektur_t_2 = 0;   // Korrekturwert Außensensor Temperatur
float Korrektur_h_1 = 0;   // Korrekturwert Innensensor Luftfeuchtigkeit
float Korrektur_h_2 = 0;   // Korrekturwert Außensensor Luftfeuchtigkeit

//*********************** Minnimal- / Maximalwerte vom Relais *****************************
float SCHALTmin =  3.0;   // minnimaler Taupuntunterschied, bei dem das Relais schaltet
float HYSTERESE =  1.0;   // Abstand von Ein- und Ausschaltpunkt        ==> fest, nicht einstellbar
float TEMP1_min = 10.0;   // Minnimale Innentemperatur, bei der die Lüftung aktiviert wird
float TEMP1_max = 30.0;   // Maximale Innentemperatur, bei der die Lüftung aktiviert wird
float TEMP2_min =-30.0;   // Minnimale Außentemperatur, bei der die Lüftung aktiviert wird
float TEMP2_max = 30.0;   // Maximale Außentemperatur, bei der die Lüftung aktiviert wird

//********************************* "Schalter" ********************************************
bool use_WIFI = false;
int Datenspeicher = 400; // Wie viele Datensätze sollen gespeichert werden / 0 = Datenspeicherung deaktiviert
bool use_Highcharts = false;
bool use_Ampel = false;
bool debugging = false;
// ***************************** Variablen für die Diagramme ******************************
String Chart_temp_in = "";
String Chart_air_in  = "";
String Chart_tp_in   = "";
String Chart_temp_out= "";
String Chart_air_out = "";
String Chart_tp_out  = "";
String Chart_tp_delta= "";
String Chart_Luefter = "";

//*************************** Variablen für Zeitfunktionen  ******************************
unsigned long currentTime = millis();
unsigned long previousTime = 0; 
unsigned long previousMillis = 0;
long last_calculation = -10000;  // alle 10 Secunden Taupunkt berechnen

//---------------------------------------------------------------------------------------------------------------
//--------------------------------- SETUP ----- Hinweis: Diese Task läuft auf Core-1-----------------------------
//---------------------------------------------------------------------------------------------------------------

void setup() 
{ 
  pinMode(RELAIPIN, OUTPUT);         // Relaispin als Output definieren
  digitalWrite(RELAIPIN, RELAIS_EIN); // Relais einschalten

  Optionen_laden();
  if (use_WIFI == false) use_Highcharts = false;  //--- Highcharts geht nicht ohne WIFI!
  
  Serial.begin(115200);
  Serial_Debugging_println("");
  Serial_Debugging_println("----------------- Taupunktlüfter -----------------------------"); 
  Serial_Debugging_println("----------------  "  + String(Software_version) +  " ------------------------------");
  Serial_Debugging_println("");
 
  esp_task_wdt_init(wdt_timeout, false);   // Watchdog auf 60 Sekunden stellen
  Serial_Debugging_println("Der task_wdt wurde auf " + String(wdt_timeout) + "Sekunden gesetzt");
   
  print_Reset_reason(); // Zur Fehlersuche: Warum ist der ESP gerade neu gestartet?
  
  pinMode(DHT_Power_Pin, OUTPUT);    // Port, von dem die DHT-Sensoren den Strom bekommen
  digitalWrite(DHT_Power_Pin, HIGH); // Strom für DHT-Sensoren einschalten
  
  Encoder_initialisieren();
  timer_initialisieren();
  OLED_init();  // OLED initialisieren
  
   setup_Ampel();
  //---------------------------------------------------------------------------------------------------------------------
  
 if (use_WIFI == true) WIFI_Connect();  // WIFI-Verbindung herstellen und Server starten
  //--------------------------------------------- Sensoren starten -------------------------------------------------------
  dht1.begin(); 
  dht2.begin(); 
  //---------------------------------------------- Chronik / Highcharts ---------------------------------------------------

  if (Datenspeicher > 0){ chronik_load_all();  // Alle Datensätze aus dem SPIFFS-Speicher einlesen, für die Verwendung von Highcharts
  }
  digitalWrite(RELAIPIN, RELAIS_AUS); // Relais ausschalten
}//------------------------------------------------------------ Ende Setup  -----------------------------------------------------------------------------------


void loop() 
{     
 if (use_WIFI == true) WIFI_watch_Task(); // Kontrolliert alle 60000 ms, ob WIFI noch vorhanden ist
 if ((last_calculation + 10000) < millis()) Taupunkte_berechnen(); // alle 10 Sekunden die Werte neu ermitteln  
   
 if (focus < 4)  focus = get_Encoder_pos(0,3 ,true); // Durchlauf durch die einzelnen Bildschirme
 switch (focus) 
 {
  case 0: OLED_Draw_Logo(); //-------------- Taupunkt Logo anzeigen
    break;
    
  case 1:     //------------------------------ Temperatur- und Luftfeuchtigkeitswerte
     show_Wertebildschirm();
    break;
    
  case 2:   //-------------------------------- Status- Bildschirm
    show_Status();
    break;
    
  case 3:
    show_Kontaktbildschirm(); //---------------- Kontaktdaten
    break;
    
 case 'A':      //---------------------------- OptionsMenü
    show_menue();
    break;
    
 case 'B':
    show_SubMenue(); //---------------------- jeweiliges Untermenü
    break;
    
 case 'C':
    show_Eingabe_Bildschirm(); //------------ Eingabe der Optionswerte
    break;

  default:
     Set_LED1(); // dürfte eigentlich nie hier ankommen
    break; 
 }
}
