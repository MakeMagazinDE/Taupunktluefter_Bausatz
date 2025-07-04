#include <Preferences.h>
Preferences preferences;

bool ButtonState = LOW;
long encoderPos = 0;
String ABC ="";         // Hier wird der String gespeichert, der Bearbeitet/Editiert wird
long Button_count;
long last_Button_count;
byte SubMenue = 0;
byte counter = 0;
bool Case = true;
bool torgel;


struct my_point {
    byte pos;    // Position im Menü
    byte x;      // x-Koordinate
    byte y;      // y-Koordinate
};
  my_point point{0,1,0};
  my_point old_point{1,1,1};

char Tastatur_40[43] = {"0123456789.,-:ABCDEFGHIJKLMNOPQRSTUVWXYZ "};     // Buchstaben-Tastatur  
char Tastatur_40_k[43]={"!%&/()=?*#{}<>abcdefghijklmnopqrstuvwxyz "};     // Buchstaben-Tastatur -klein
char Tasten[4][7] ={" - "," Exit "," Del ","  OK  "};                    // Steuerungstasten


typedef struct {
 char Bezeichnung[16];
 byte Menue;
 char Type;       // B = bool, F = float, S = String, M = Menü
char Zeichenfolge[21]; 
}param;



param Parameter[] = {       

// Bezeichnung(max 15 Z.) Menü,   Type,   Zeichenfolge (max 21 Z.)
  {"Funktionen",          1,       'M',   "-"}, // Funktionen
  {"WIFI",                1,       'B',   "1"},
  {"Datenspeicher",       1,       'F',   "400"},
  {"Highcharts",          1,       'B',   "1"},
  {"Ampel",               1,       'B',   "1"},
  {"Debuggingmodus",      1,       'B',   "1"},
  
  {"Sensor Korrekt.",     2,       'M',   "0"}, //Sensoren Korrektur
  {"Temp. innen",         2,       'F',   "0.0"},
  {"Temp. aussen",        2,       'F',   "0.0"},
  {"Luftf. innen",        2,       'F',   "0.0"},
  {"Luftf. aussen",       2,       'F',   "0.0"},
  
  {"Temp. Optionen",      3,       'M',   "-"}, // Temperatur Optionen
  {"Min delta TP",        3,       'F',   "2"},
  {"Min Temp innen",      3,       'F',   "10.0"},
  {"Max Temp innen",      3,       'F',   "30.0"},
  {"Min Temp aussen",     3,       'F',   "-30.0"},
  {"Max Temp aussen",     3,       'F',   "30.0"},
  
  {"Internet",            4,       'M',   "-"},   // Internet
  {"IP-Adresse",          4,       'S',   "192.168.178.112"},
  {"Gateway",             4,       'S',   "192.168.178.1"},
  {"Mask",                4,       'S',   "255.255.255.0"},
  {"SSID",                4,       'S',   "FritzUS"},
  {"Passwort",            4       ,'S',   "48094564919405773644"},  
  {"DNS",                 4,       'S',   "192.168.178.1"},          // ACHTUNG!!! DNS2 = (8.8.8.8) fest vergeben!

  {"Wartung",             5,       'M',   "-"},   // Wartung
  {"SPIFFS loeschen",     5,       'B',   "0"},
  {"Optionen reset",      5,       'B',   "0"},
  {"CPU Neustart",        5,       'B',   "0"}
};

bool Option_Aenderung = false;  // wurde an den Optionen etwas geändert?
byte Anzahl_Menues = 5;
byte Anzahl_Elemente = 28; 

byte ParameterSelected = 0;
long Last_click = millis(); // wie lange wurde der Encoder nicht geklickt
#define Zeit_menue 120000    // zurück bei zwei Minuten Untätigkeit


  void show_menue()
  {
    OLED_clear();
    int ii = 0;
    if ((Last_click +Zeit_menue) < millis()) focus = 0; // automatisch in den Startbildschirm zurückspringen nach Zeit_menue Sekunden
    encoderPos = get_Encoder_pos(1 , Anzahl_Menues+1 , true);
    
    if (ii == 0 ) OLED_println(String_laenge_ergaenzen("Einstellungen", 21) ,ii,0); // ------------- Überschrift

    for (int i = 0; i< Anzahl_Elemente ;i++)
     {
      if (Parameter[i].Type == 'M' )
      {
        ii++;
        if (encoderPos == ii) oled.invertText(true); 
          else oled.invertText(false);
        OLED_println(Parameter[i].Bezeichnung,ii,0);
      }
     
    if (encoderPos == Anzahl_Menues+1 ) { oled.invertText(true); Print_zurueck(7,true);}
      else { oled.invertText(false); Print_zurueck(7,false);}
    }
    OLED_update();
    //------------------------------------------ Button auswerten ----------------------
    if (last_Button_count != Button_count) 
    {
      last_Button_count = Button_count;
      if(encoderPos == (ii+1))
      {
        focus = 0;                         //----------Startbildschirm
        oled.invertText(false);
      }else{
      SubMenue = encoderPos ;
      focus = 'B';                        //---------------- SubMenü 
      counter = 0; 
      reset_Encoder(1); 
    }
   }
  }

  void show_SubMenue()
  {
    int ii = 0;
    byte Anzahl_Items = 0;
    if ((Last_click +Zeit_menue) < millis()) focus = 0; // automatisch in den Startbildschirm zurückspringen nach Zeit_menue Sekunden
 
    for (int i = 0; i< Anzahl_Elemente ;i++)
       if (Parameter[i].Menue == SubMenue ) Anzahl_Items++;

    OLED_clear();
    encoderPos = get_Encoder_pos(1,Anzahl_Items ,true);
    for (int i = 0; i< Anzahl_Elemente ;i++)
    {
       if (Parameter[i].Menue == SubMenue )
       {
        if (ii == 0)  OLED_println(String_laenge_ergaenzen(Parameter[i].Bezeichnung, 21) ,ii,0); // ------------- Überschrift
         else {
          if (encoderPos == ii) 
          {
             oled.invertText(true);
            ParameterSelected = i;
           // Serial.println("ParameterSelected Submenü : " + String(ParameterSelected));
          } 
          else oled.invertText(false);
          OLED_println( Parameter[i].Bezeichnung,ii,0);
         }
         ii++;
        }        
       }
 
    if (encoderPos == Anzahl_Items) {
      oled.invertText(true); 
      Print_zurueck(7, true);
    }else {
      oled.invertText(false);
      Print_zurueck(7 , false);
    }
    
     //------------------------------------------------------ Click auswerten / zuweisen
     if (last_Button_count != Button_count) 
    {
      last_Button_count = Button_count; 
      //Serial.println(String(encoderPos));
      if(encoderPos == Anzahl_Items)
      {
        focus = 'A';              // ------------------------Zurück ==> Optionsmenü
        reset_Encoder(1);          
        oled.invertText(false);
      }else  if (Wartung(ParameterSelected) == true  ){Serial.println("Wartungsmenü = true");} //---------------------------- Wartung
       else{
        focus = 'C';              // ------------------------Item ausgewählt ==> Eingabetastatur
        reset_Encoder(0); 
        OLED_clear();
      }
    }
    
  OLED_update();  
  }

  
 void show_Eingabe_Bildschirm()
{
  point = {255,0,0};
  old_point = point;
  if ((Last_click +Zeit_menue) < millis()) focus = 0; // automatisch in den Startbildschirm zurückspringen nach Zeit_menue Sekunden

  for (int i = 0 ; i < 41 ; i++) //---------  alle Buchstaben und Zahlen schreiben
  {
    if (Case)
    {
      if (i == 40)  oled.invertText(true); // Leerzeichen visualisieren
      if (i <= 13)OLED_print_char( Tastatur_40[i],0 ,i);                    // OLED Zeile 0
      if ((i > 13) and (i < 28)) OLED_print_char( Tastatur_40[i],1 ,i-14);  // OLED Zeile 1
      if ((i > 27) and (i < 41)) OLED_print_char( Tastatur_40[i],2 ,i-28);  // OLED Zeile 2
    }else {
      if (i <= 13)OLED_print_char( Tastatur_40_k[i],0 ,i);                    // OLED Zeile 0
      if ((i > 13) and (i < 28)) OLED_print_char( Tastatur_40_k[i],1 ,i-14);  // OLED Zeile 1
      if ((i > 27) and (i < 41)) OLED_print_char( Tastatur_40_k[i],2 ,i-28);  // OLED Zeile 2      
    }
  }
  oled.invertText(false);
  if (counter == 0)
  { 
    counter = 1;
    OLED_println("--------------",3,0);                                    // OLED Zeile 3
    OLED_println(Parameter[ParameterSelected].Bezeichnung,4,0);            // OLED Zeile 4
     ABC = Parameter[ParameterSelected].Zeichenfolge;
    OLED_println(ABC,5,0);                                                 // OLED Zeile 5 
  }
  OLED_println("--------------------",6,0);                                // OLED Zeile 6
  
  torgel = not torgel;
  //if (torgel)  OLED_print_char('-', 6,ABC.length()); else OLED_print_char(' ', 6,ABC.length()); // Unterstrich blinkt an der Stelle, wo gerade editiert wird
  if (torgel) {  oled.invertText(false); OLED_print_char(' ', 5,ABC.length());} 
    else      { oled.invertText(true); OLED_print_char(' ', 5,ABC.length());} // Unterstrich blinkt an der Stelle, wo gerade editiert wird
  oled.invertText(false);
  String f = String(Tasten[0]) + String(Tasten[1]) + String(Tasten[2]) + String(Tasten[3]);    // OLED Zeile 7
  OLED_println(f,7);
 
  point.pos = get_Encoder_pos(0,44,true); // (min, max, überlauf)
   // --------------------------------------------------- Tasten markieren wenn getroffen
  if (point.pos != old_point.pos)
  {
        old_point.pos = point.pos;
        if(  point.pos < 41 )
        { 
          if (  point.pos <= 13 )                      point = { point.pos, 0,  point.pos};
          if (( point.pos > 13) and ( point.pos < 28)) point = { point.pos, 1,  point.pos-14};
          if (( point.pos > 27) and ( point.pos < 41)) point = { point.pos, 2,  point.pos-28};

          oled.invertText(true); 
         if (Case) OLED_print_char( Tastatur_40[point.pos],point.x ,point.y); else OLED_print_char( Tastatur_40_k[point.pos],point.x ,point.y);
         // oled.invertText(false); 
          if (point.pos < 40) oled.invertText(false); 
          oled.setScale(4); 
          if (Case) OLED_print_char( Tastatur_40[point.pos],0,17); else  OLED_print_char( Tastatur_40_k[point.pos],0,17); 
         oled.setScale(1); 
         oled.invertText(false); 
        }
       if (point.pos > 40) //------die Tasten Groß/klein, Exit, Del und Ok markieren wenn getroffen
       {
        if (point.pos == 41) point = { point.pos, 7,  0}; // ---------- Groß/Kleinschrift
        if (point.pos == 42) point = { point.pos, 7,  3}; // ---------- Exit
        if (point.pos == 43) point = { point.pos, 7,  9}; // ---------- Del
        if (point.pos == 44) point = { point.pos, 7,  14};// ---------- OK
        oled.invertText(true); 
        OLED_println( String(Tasten[point.pos-41]) ,point.x ,point.y ); 
        oled.invertText(false);
       }
        
        // ------------------------------------ den vergrößerten Buchstaben/Zahl löschen
        if (point.pos > 40) 
        {
          oled.setScale(4); 
          OLED_print_char( ' ',0,17); 
          oled.setScale(1); 
        }

       // -------------------------- Button Click bearbeiten----------------------------
       if (last_Button_count != Button_count)
        { 
          last_Button_count = Button_count;

          if (point.pos < 41)  //------------- Buchstabe ist markiert ==> Buchstabe übernehmen
          {
            if (point.pos == 40) ABC = ABC + ' ';
            else if (Case) ABC = ABC + String(Tastatur_40[point.pos]); else  ABC = ABC + String(Tastatur_40_k[point.pos]);
            OLED_println(ABC,5,0);
          }else if (point.pos == 41)// --------------- Groß/Kleinschrift
          {
            if (Case) strcpy(Tasten[0], " + "); else strcpy(Tasten[0], " - ");
            Case = not Case;
          }
          else if (point.pos == 42)// ----------  Exit
          {
            focus = 'B';      //---------------------- Exit ==> SubMenü (ohne Kennzeichen Änderung)
            ABC = ""; 
            reset_Encoder(1);   
            counter = 0;       
           }else if (point.pos == 43)   //------------ Del ==> letzten Buchstaben löschen
          {
           oled.invertText(false);
           ABC = ABC.substring(0, (ABC.length()-1));
            OLED_println(ABC + String("  "),5,0);
          } else if (point.pos == 44)  //------------- OK ==> SubMenü (mit Kennzeichen Änderung)
          {
            OLED_clear();
            OLED_println("OK",4,9);
            delay(500);
              char tmp[21];
              ABC.toCharArray(tmp,21);
              if(StringVergleich(tmp , Parameter[ParameterSelected].Zeichenfolge ) == false)// Speichern, wenn sich etwas geändert hat
              {
                strcpy(Parameter[ParameterSelected].Zeichenfolge, tmp); //--------- Speichern
                Save_one_Item(ParameterSelected); 
              }              
            focus = 'B';                    // ------------------------- SubMenü 
            ABC = ""; 
            reset_Encoder(1);  
            counter = 0;
          }
       } 
  }
   OLED_update();
}




void alle_Optionen_speichern()
{ 
  byte result;
 if (debugging == true) Optionen_Serial_ausgeben();
  
 preferences.begin("Optionen", false); 
 Serial_Debugging_println(" Speichern der Optionen/Parameter");
 for(int i = 0 ; i < Anzahl_Elemente ; i++)
 {
  result = preferences.putString(Parameter[i].Bezeichnung, Parameter[i].Zeichenfolge);

  //------------------------------- Debugging Ausgabe --------------------------
  Serial_Debugging_print("preferences.put: ");
  Serial_Debugging_print(Parameter[i].Bezeichnung);
  Serial_Debugging_print("  Type=" + String(Parameter[i].Type));
  Serial_Debugging_print( "  Wert=" + String(Parameter[i].Zeichenfolge)); 
  Serial_Debugging_print("   Result=");
  Serial_Debugging_println(String(result));
 }
 //-------------------------------------------------------------------------------
preferences.end();
Option_Aenderung = false;
}

bool Save_one_Item(byte item)
{
  bool result;
  preferences.begin("Optionen", false); 
  result = preferences.putString(Parameter[item].Bezeichnung, Parameter[item].Zeichenfolge);
  preferences.end();
  return result;
}

void Optionen_laden()
{
  String f;
  preferences.begin("Optionen", false); 
  for(int i = 0 ; i < Anzahl_Elemente ; i++)
 {
    f = preferences.getString(Parameter[i].Bezeichnung, Parameter[i].Zeichenfolge); 
    f.toCharArray(Parameter[i].Zeichenfolge, 22);
 }
preferences.end();
Optionen_zu_Variablen();
if (debugging == true) Optionen_Serial_ausgeben();
}

void Optionen_loeschen()
{
  preferences.begin("Optionen", false); 
  preferences.clear();
  preferences.end();
}

void Optionen_zu_Variablen()
{
  int i;
  for ( i = 0 ; i < Anzahl_Elemente ; i++)
  { //-------------------------------------------------------------- Sensorenkorrektur ---------------------------------------------------------
    if( StringVergleich(String(Parameter[i].Bezeichnung), "Temp. innen"))        Korrektur_t_1 = String_to_float(Parameter[i].Zeichenfolge);
    if( StringVergleich(String(Parameter[i].Bezeichnung), "Temp. aussen"))       Korrektur_t_2 = String_to_float(Parameter[i].Zeichenfolge);
    if( StringVergleich(String(Parameter[i].Bezeichnung), "Luftf. innen"))       Korrektur_h_1 = String_to_float(Parameter[i].Zeichenfolge);
    if( StringVergleich(String(Parameter[i].Bezeichnung), "Luftf. aussen"))      Korrektur_h_2 = String_to_float(Parameter[i].Zeichenfolge);
    
    //------------------------------------------------------------- Temperatur Optionen --------------------------------------------------------
    if( StringVergleich(String(Parameter[i].Bezeichnung), "Min delta TP"))       SCHALTmin = String_to_float(Parameter[i].Zeichenfolge);
    if( StringVergleich(String(Parameter[i].Bezeichnung), "Min Temp innen"))     TEMP1_min = String_to_float(Parameter[i].Zeichenfolge);
    if( StringVergleich(String(Parameter[i].Bezeichnung), "Max Temp innen"))     TEMP1_max = String_to_float(Parameter[i].Zeichenfolge);
    if( StringVergleich(String(Parameter[i].Bezeichnung), "Min Temp aussen"))    TEMP2_min = String_to_float(Parameter[i].Zeichenfolge);
    if( StringVergleich(String(Parameter[i].Bezeichnung), "Max Temp aussen"))    TEMP2_max = String_to_float(Parameter[i].Zeichenfolge);

    //------------------------------------------------------------- Internet -------------------------------------------------------------------
    if( StringVergleich(String(Parameter[i].Bezeichnung), "IP-Adresse"))  local_IP.fromString(String(Parameter[i].Zeichenfolge));
    if( StringVergleich(String(Parameter[i].Bezeichnung), "Gateway"))     gateway.fromString(String(Parameter[i].Zeichenfolge));
    if( StringVergleich(String(Parameter[i].Bezeichnung), "Mask"))        subnet.fromString(String(Parameter[i].Zeichenfolge));
    if( StringVergleich(String(Parameter[i].Bezeichnung), "DNS"))         dns1.fromString(String(Parameter[i].Zeichenfolge));
    if( StringVergleich(String(Parameter[i].Bezeichnung), "Passwort"))    strcpy(password,Parameter[i].Zeichenfolge);
    if( StringVergleich(String(Parameter[i].Bezeichnung), "SSID"))        strcpy(ssid,Parameter[i].Zeichenfolge);
    
    //----------------------------------------------------------- Funktionen -------------------------------------------------------------------
    if( StringVergleich(String(Parameter[i].Bezeichnung), "WIFI"))            use_WIFI = Bool(Parameter[i].Zeichenfolge);
    if( StringVergleich(String(Parameter[i].Bezeichnung), "Datenspeicher"))   Datenspeicher = String_to_int(Parameter[i].Zeichenfolge);
    if( StringVergleich(String(Parameter[i].Bezeichnung), "Highcharts"))      use_Highcharts = Bool(Parameter[i].Zeichenfolge);
    if( StringVergleich(String(Parameter[i].Bezeichnung), "Ampel"))           use_Ampel = Bool(Parameter[i].Zeichenfolge);
    if( StringVergleich(String(Parameter[i].Bezeichnung), "Debuggingmodus"))  debugging = Bool(Parameter[i].Zeichenfolge);

    //---------------------------------------------------------- Wartung ----------------------------------------------------------------------
  }
 if (debugging) Serial.println("Debugging true"); else Serial.println("Debugging false");
}


byte String_to_MenuePos(String s)
{
  for( int i = 0; i < Anzahl_Elemente; i++)
  {
    if(StringVergleich(String(Parameter[encoderPos].Bezeichnung), s))
    {
      return i;
   }
  }
}

void Optionen_Serial_ausgeben()
{
  for(int i = 0; i < Anzahl_Elemente; i++)
  {
    String f = Parameter[i].Bezeichnung + String(" | ") + 
    String(Parameter[i].Menue)+ String(" | ") + 
    String(Parameter[i].Type)+ String(" | ") + 
    String(Parameter[i].Zeichenfolge);
    Serial.println(f);
  }
}

bool Wartung(byte MenueItem)
{
  if(StringVergleich(String(Parameter[MenueItem].Bezeichnung), "CPU Neustart")) 
  {
    soft_reset(6); 
    return true;
  } else 
  if(StringVergleich(String(Parameter[MenueItem].Bezeichnung), "SPIFFS loeschen")) 
  {
    SPIFFS_formatieren();
    return true;
  } else 
  if(StringVergleich(String(Parameter[MenueItem].Bezeichnung), "Optionen reset")) 
  {
   Optionen_reset();
    return true;
  }
  else 
  {
    return false;
  }
}


void Optionen_reset()
{
  byte pos;
   last_Button_count = Button_count;
   oled.invertText(false);
   oled.setScale(1);
   OLED_clear();
   OLED_println("-- Optionen reset --",0,0); 

   OLED_println("WARNUNG:",2,0); 
   OLED_println("Diese Funktion l",3,0); drawSonderzeichen(3); /* Sonderzeichen ö */  OLED_print("scht");
   OLED_println("alle Benutzer    ",4,0); 
   OLED_println("Einstellungen !",5,0); 
   OLED_println("Wirklich ausf",6,0);  drawSonderzeichen(2);/* Sonderzeichen ü*/  OLED_print("hren?");
   
   reset_Encoder(2); 
  while (last_Button_count == Button_count)
  {
   if ((Last_click +50) < millis())
    {
      pos = get_Encoder_pos(1,2,true); // (min, max, überlauf)
      if (pos == 1)  oled.invertText(true); else  oled.invertText(false);
      OLED_println("   Ja   ",7,0); 
      if (pos == 2)  oled.invertText(true); else  oled.invertText(false);
      OLED_println("  Nein  ",7,11); 
      OLED_update();
    }
  }
    // --------------------------------------------Button Auswerten
   
  if (pos == 1) 
    {
      Optionen_loeschen();   
      soft_reset(6); 
    }
    if (pos == 2)
    {
      focus = 'B';
      SubMenue = 5;
      last_Button_count = Button_count;   
    }
 }
 

void SPIFFS_formatieren()
{
  byte pos;
   last_Button_count = Button_count;
   oled.invertText(false);
   oled.setScale(1);
   OLED_clear();
   OLED_println("--- Daten l",0,0);drawSonderzeichen(3); /* Sonderzeichen ö */  OLED_print("schen ---");

   OLED_println("WARNUNG:",2,0); 
   OLED_println("Diese Funktion l",3,0); drawSonderzeichen(3); /* Sonderzeichen ö */  OLED_print("scht");
   OLED_println("alle",4,8); 
   OLED_println("Datenaufzeichnungen!",5,0); 
   OLED_println("Wirklich ausf",6,0);  drawSonderzeichen(2);/* Sonderzeichen ü*/  OLED_print("hren?");
   
   reset_Encoder(2); 
   while (last_Button_count == Button_count)
  {
   if ((Last_click +50) < millis())
    {
      pos = get_Encoder_pos(1,2,true); // (min, max, überlauf)
      if (pos == 1)  oled.invertText(true); else  oled.invertText(false);
      OLED_println("   Ja   ",7,0); 
      if (pos == 2)  oled.invertText(true); else  oled.invertText(false);
      OLED_println("  Nein  ",7,11); 
      OLED_update();
    }
  }
    // --------------------------------------------Button Auswerten
  if (pos == 1) 
    {
      format_SPIFFS();
      soft_reset(6); 
    }
    if (pos == 2) 
    {
      focus = 'B';
      SubMenue = 5;
      last_Button_count = Button_count;   
    }
 }
