//////////////////////////////////////////////////////////////////////////////
// Das Taupunkt-Lüftungssystem 
// mit dem ESP32-CAM
// 
// veröffentlicht im MAKE Sonderheft 2/2023
//
// Ulrich Schmerold
// 8/2023
//////////////////////////////////////////////////////////////////////////////

#include "SPIFFS.h"

#define chronik_interval 3600 // 3600 = eine Stunde, 900 =15 Minuten, 60 = 1 Minuten
#define MAX__ANZAHL_WERTE 600 // Anzahl der möglichen Datensätze ==> 1MB SPIFFS: 6809, 2 MB SPIFFS: 13618, 3 MB SPIFFS: 20427
#define laenge_Datensatz 21 //[1690024171000, 16.4],   //Highcharts erwartet 13 Stellen für Zeit
#define oldest_UNIX_time 1690024171  // UNIX_Time hat 10 Stellen

unsigned long last_save = 0;
unsigned long Unix_Timestamp = 0;
int  Anzahl_Werte = 0;


String wert_hinzufuegen(String f, float wert)
{   Watchdog_reset();
  //--------------------------------------------------------------------------------
 Anzahl_Werte = ( f.length() / laenge_Datensatz );
 Serial_Debugging_println(" ########## Prozedur: wert_hinzufuegen(String f, float wert); ######## Wert: " + String(wert) +"  Unix_Timestamp: "+String(Unix_Timestamp) + "  Anzahl gespeicherte Werte: "+ String(Anzahl_Werte));

  // ----------------------- UNIX Zeit überprüfen, ob ausgegebener Wert sein kann ------------------------------------
  Unix_Timestamp = rtc.getEpoch();
  if (Unix_Timestamp < oldest_UNIX_time)
  {
   if (get_Local_Time()< oldest_UNIX_time)
   {
    Serial_Debugging_println("FEHLER! Hatte keine gültige UNIX Zeit beim Messwert hinzufügen");
    return f;
   }
  }
  //-----------------------Prüfen ob der "Wert" richtig  sein kann --------------------------------------------------------
  if ((isnan(wert)) or (wert > 100) or (wert < -40))
  {
    Serial_Debugging_println("FEHLER! "+ String(f) + "ist kein gültiger Wert! Wert wird nicht hinzugefügt.");
    return f; // Routine abbrechen und alten String zurückgeben
  }
  //-----------------------Prüfen auf gültige Datensätze und  maximale Datenlänge-----------------------------------------
   
 while (Anzahl_Werte > MAX__ANZAHL_WERTE)
 {
   f.remove(0,1);
    while (((f.substring(0,1)!="[") or (f.substring(20,21)!="]") or (f.substring(14,15)!="," ) or (f.substring(21,22)!="," ))and (f.length()>0) )
    {
     f.remove(0,1);         
    }
    Anzahl_Werte = ( f.length() / laenge_Datensatz );
 }
  //--------------------------------Datensatz zusammenbauen-----------------------------------------------------------------
  if( f.length() >= (laenge_Datensatz)-1) f = f + ',';   // Trennzeichen für nächsten Wert
  f = f + '[' +  rtc.getEpoch() + "000,";                // für ein UNIX-Datum muss der Epoch-Wert mal 1000 genommen werden
  if((fabs(wert)) < 10)  f = f + ' ';                    // Leerstelle wenn die Zahl kleiner als 10 ist
  if (wert >= 0)      f = f + ' ';                       // Leerstelle für das Vorzeichen
 
  char buffer[6];
  sprintf(buffer, "%.1f", wert);          // Nachkommastellen bis auf eine Stelle streichen;
 
   f = f + buffer +']';
  return f;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------

unsigned int SPIFFS_print_space()
{
 if (!SPIFFS.begin(true)) 
 {
  Serial_Debugging_println("An Error has occurred while mounting SPIFFS");
  return 0;
 }
  int tBytes = SPIFFS.totalBytes(); 
  int uBytes = SPIFFS.usedBytes();
  Serial_Debugging_println(" SPIFFS.totalBytes: "+ String(tBytes)); 
  Serial_Debugging_println(" SPIFFS.usedBytes: "+ String(uBytes));
  Serial_Debugging_println(" SPIFFS.freeBytes: "+ String(tBytes-uBytes));
 return (tBytes-uBytes);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------

void write_SPIFFS(String name, String daten)
{  
 Watchdog_reset(); 
 OLED_clear();
 OLED_println("Sensordaten speichern:", 0, 0);
  
 if (!SPIFFS.begin(true)) {
    Serial_Debugging_println("An Error has occurred while mounting SPIFFS");
    return;
  }
  
  File file = SPIFFS.open(name, FILE_WRITE);
   OLED_println("Schreibe: "+name+"     ", 3, 0);
   OLED_update();

if (file.print(daten)) {
  } else {
    Serial_Debugging_print("Fehler beim Schreiben der Datei "+name+" in den SPIFFS: ");
   OLED_println("Fehler: "+name+"     ", 3, 0);
   OLED_update();
   delay(500);
   return;
  }
  
  file.close();
  Serial_Debugging_println(name + " | " + daten);
  Serial_Debugging_println(name+" erfolgreich geschrieben.");
  OLED_println("Erfolgreich: "+name+"     ", 3, 0);
  OLED_update();
  delay(500);

}
//--------------------------------------------------------------------------------------------------------------------------------------------------

String read_SPIFFS(String name)
{
  String buffer = "";
   Watchdog_reset();

  if (!SPIFFS.begin(true)) {
    Serial_Debugging_println("An Error has occurred while mounting SPIFFS");
    return "";
  } 
 
 File file = SPIFFS.open(name);
  if(!file){
   Serial_Debugging_print("Failed to open file for reading: "+name);
   OLED_println("Fehler: "+name+"     ", 3, 0);
   OLED_update();
   return "";
  } else {
    Anzahl_Werte = file.size()/laenge_Datensatz;
   Serial_Debugging_print(name+"("+String(Anzahl_Werte)+")...");
   OLED_println("Lade: "+name+"     ", 3, 0);
   OLED_println("Anzahl Werte: "+ String(Anzahl_Werte)+"  ",4,0);
   OLED_update();
  }

  while(file.available()){
    buffer = buffer + char(file.read());
  }

  file.close();
  return buffer;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------

bool remove_SPIFFS(String name) 
{
  if (!SPIFFS.begin(true)) {
    Serial_Debugging_println("An Error has occurred while mounting SPIFFS");
  }
  return SPIFFS.remove(name);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------

bool format_SPIFFS() 
{ 
  if (!SPIFFS.begin(true)) {
   Serial_Debugging_println("An Error has occurred while mounting SPIFFS");
   return 0;
  } 
    Serial_Debugging_println("Formatiere SPIFFS ....");
    if (use_OLED == true)
    {
      OLED_clear();
      OLED_println("Formatiere SPIFFS",2,0); 
      OLED_println("Bitte warten...",4,0); 
      OLED_update();
    }
    
    bool formatted = SPIFFS.format(); 
    if(formatted){
          if (use_OLED == true)
            {
              OLED_clear();
              OLED_println("Erfolgreich!",4,0); 
              OLED_update();
            }

      Serial_Debugging_println("\n\nSuccess formatting");
    }else{
         if (use_OLED == true)
            {
              OLED_clear();
              OLED_println("Fehler aufgetreten!",4,0); 
              OLED_update();
            }
      Serial_Debugging_println("\n\nError formatting");
    }
    return formatted;
  }
//--------------------------------------------------------------------------------------------------------------------------------------------------

String chronic_update (String name, float wert)
{
  String f;
  f = read_SPIFFS(name);
  if (f.length() < 5) f="";
  f = wert_hinzufuegen(f, wert);
  write_SPIFFS(name, f );
   return f;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------

void chronik_load_all()
{
  int n=0;
  while ((HTML_processor_is_working == true )and (n < 50))
  {
    delay(100);
    n++;
  }
  if (HTML_processor_is_working == false)
  { 
   chronik_is_busy = true; 
   OLED_clear();
   OLED_println("Lade SPIFFS Daten", 0, 0);
    Serial_Debugging_print("Die Chronik wird aus dem SPIFFS geladen...");

    Chart_temp_in = read_SPIFFS("/temp_in.txt");  
    Chart_air_in = read_SPIFFS("/air_in.txt");
    Chart_tp_in = read_SPIFFS("/tp_in.txt"); 

    Chart_temp_out = read_SPIFFS("/temp_out.txt");
    Chart_air_out = read_SPIFFS("/air_out.txt");
    Chart_tp_out = read_SPIFFS("/tp_out.txt");

    Chart_tp_delta = read_SPIFFS("/tp_delta.txt");

   chronik_is_busy = false; 
   Serial_Debugging_println("fertig!");
  }
}
//--------------------------------------------------------------------------------------------------------------------------------------------------

void chronik_update_all()
{ 
  if( Datenspeicher > 0 ) // Datenspeicherung findet nur statt, wenn die Konstante "Datenspeicherung" mit einer Zahl > 0 belegt ist
  {
     if(last_save == 0){last_save = rtc.getEpoch();}      // wurde die chronik_update_all schon einmal ausgeführt?
     if (HTML_processor_is_working == false)              // ... um Überschneidungen mit dem Server zu verhindern
    {
      if (last_save + chronik_interval < rtc.getEpoch())  
      {
        chronik_is_busy = true; 
        last_save = rtc.getEpoch();
        Serial_Debugging_println("Die Chronik wird in dem SPIFFS gespeichert...");

        Chart_temp_in = chronic_update("/temp_in.txt", t1);
        Chart_air_in = chronic_update("/air_in.txt", h1);
        Chart_tp_in = chronic_update("/tp_in.txt", Taupunkt_1);

        Chart_temp_out = chronic_update("/temp_out.txt", t2);
        Chart_air_out = chronic_update("/air_out.txt", h2);
        Chart_tp_out = chronic_update("/tp_out.txt", Taupunkt_2);

        Chart_tp_delta = chronic_update("/tp_delta.txt",DeltaTP);

        chronik_is_busy = false;                             // Chronik daten wieder freigeben
        Serial_Debugging_println("fertig!");
      }
    }
  }
} 
//--------------------------------------------------------------------------------------------------------------------------------------------------
