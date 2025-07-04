/////////////////////////////////////////////////////////////////////////////
// Das Taupunkt-Lüftungssystem 
// mit dem ESP32-CAM
// 
// veröffentlicht im MAKE Sonderheft 2/2023
//
// Ulrich Schmerold
// 8/2023
//////////////////////////////////////////////////////////////////////////////

#define HTML_auto_refresh_rate 60  // alle 60 Sekunden wird die HTML-Seite automatisch aktuallisiert

  //**************************************************************************************************      
  // Hinweise Sonderzeichen:ä = &aumln / ü = &uuml / ß = &szlig / °C = &degC / Pfeil = &#10144 / Delts = &#916  **
  //**************************************************************************************************

char index_html[] PROGMEM = R"rawliteral(
 <Sende HTML Page>
 <HTTP/1.1 200 OK>
 <Content-Type: text/html>
 <Connection: close>
 <!DOCTYPE html>
  <html>   <!--------------------- hier beginnt die tabellarische Darstellung der Werte ------------------------------->    
   <head>
     <meta http-equiv="refresh" content="%HTML_auto_refresh%">
     <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">
     <div align=center> 
       <style>body{background-color: #A2C2E8;}</style>
       <table class=MsoTableGrid border=1 cellspacing=3 cellpadding=0>
        <FONT SIZE="6"><b>TauPunktL&uumlfter<br></b></FONT>
        <FONT SIZE="4 "><b> Version: %Version%<b></FONT>
         <tr>
          <td width=174 style='width:130pt;background:#7A4EBA; '>
            <p class=MsoNormal align=center style='margin-bottom:0cm;text-align:center;line-height:normal'><b><span style='font-size:24pt;color:white;'>Innen</span></b></p>
          </td>
          <td width=174 style='width:130pt;background:#41B853;padding:0cm 5pt 0cm 5pt'> 
            <p class=MsoNormal align=center style='margin-bottom:0cm;text-align:center;line-height:normal'><b><span style='font-size:24pt;color:white;'>Au&szligen</span></b></p>
          </td>
         </tr>
          <td width=347 colspan=2 style='width:260pt;background:#C6D9F1;padding:0cm 5pt 0cm 5pt;height:35pt'> 
            <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><b><span style='font-size:20pt;color:black;'>Temperatur:</span></b></p> 
          </td>
         <tr>
          <td width=174 style='width:130pt;background:#7A4EBA;padding: 0cm 5pt 0cm 5pt'>
            <p class=MsoNormal align=center style='margin-bottom:0cm;text-align:center; line-height:normal'><span style='font-size:24pt;color:white;'>%T1% &degC </span></p>
          </td>
          <td width=174  style='width:130pt;background:#41B853;padding:  0cm 5pt 0cm 5pt'>
            <p class=MsoNormal align=center style='margin-bottom:0cm;text-align:center; line-height:normal'><span style='font-size:24pt;color:white;'>%T2% &degC</span></p>
          </td>
         </tr>
         <tr>
          <td width=347 colspan=2  style='width:260pt;background:#C6D9F1; padding:0cm 5pt 0cm 5pt;height:40pt'>
            <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><b><span style='font-size:20pt;color:black;'>Luftfeuchtigkeit:</span></b></p>
         </td>
          </tr>
         <tr>
          <td width=174 style='width:130pt;background:#7A4EBA;padding:  0cm 5pt 0cm 5pt'>
           <p class=MsoNormal align=center style='margin-bottom:0cm;text-align:center;line-height:normal'><span style='font-size:22pt;color:white;'>%H1% &#37</span></p>
          </td>
          <td width=174 style='width:130pt;background:#41B853;padding: 0cm 5pt 0cm 5pt'>
           <p class=MsoNormal align=center style='margin-bottom:0cm;text-align:center;line-height:normal'><span style='font-size:22pt;color:white;'>%H2% &#37</span></p>
          </td>
         </tr>
         <tr>
          <td width=347 colspan=2 style='width:260pt;background:#C6D9F1; padding:0cm 5pt 0cm 5pt;height:35pt'>
            <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><b><span style='font-size:22pt;color:black;'>Taupunkt:</span></b></p>
          </td>
         </tr>
         <tr>
          <td width=174  style='width:130pt;background:#7A4EBA;padding: 0cm 5pt 0cm 5pt'>
            <p class=MsoNormal align=center style='margin-bottom:0cm;text-align:center;line-height:normal'><span style='font-size:22pt;color:white;'>%TP1% &degC </span></p>
          </td>
          <td width=174  style='width:130pt;background:#41B853;padding:  0cm 5pt 0cm 5pt'>
            <p class=MsoNormal align=center style='margin-bottom:0cm;text-align:center;line-height:normal'><span style='font-size:22pt;color:white;'>%TP2% &degC </span></p>
          </td>
         </tr>
         <tr>
          <td width=347 colspan=2 style='width:260pt;background:#C6D9F1;padding:0cm 5pt 0cm 5pt;height:42pt'>
            <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><b><span style='font-size:22pt;color:black;'>Delta TP: %DeltaTP% &degC </span></b></p>
          </td> 
         </tr>
          <tr>
            <td width=347 colspan=2 style='width:260pt;border:none;border-bottom:solid white 1pt; background:%Color-1%;padding:0cm 5pt 0cm 5pt;height:42pt'>
              <p class=MsoNormal align=center style='margin-bottom:0cm;text-align:center; line-height:normal'><b><span style='font-size:22pt;color:white;'> %Rel% </span></b></p>
            </td>
           </tr>
       </table>
    </div>
    <p class=MsoNormal style='text-align:center;'><b><span style='font-size:12pt; color:black;'> Letzte Aktualisierung: %Stamp% </span></b></p>
  </html>
  <!--------------------- ------------------------hier beginnt Highcharts --------------------------------------------------------->        
<!DOCTYPE HTML><html>  
  <head> 
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <script src="https://code.highcharts.com/highcharts.js"></script> 
    <style> 
     body { min-width: 310px; max-width: 800px; height: 300px; margin: 0 auto;}
      h2 {font-family: Arial; font-size: 2.5rem; text-align: center; } 
    </style>
  </head>
    <script>
      Highcharts.setOptions({
       plotOptions: {
         series: { animation: false }},
         time: { timezoneOffset: -120 },
         lang: {
           months: ['Januar', 'Februar', 'M&aumlrz', 'April', 'Mai', 'Juni', 'Juli', 'August', 'September', 'Oktober', 'November', 'Dezember'],
           weekdays: ['Sonntag', 'Montag', 'Dienstag', 'Mittwoch', 'Donnerstag', 'Freitag', 'Samstag'],
           shortMonths: ['Jan', 'Feb', 'M&aumlrz', 'Apr', 'Mai', 'Jun', 'Jul', 'Aug', 'Sep', 'Okt', 'Nov', 'Dez'],
           resetZoom: "Zoom zur&uumlcksetzen",
           resetZoomTitle: "Zoom zur&uumlcksetzen" }
      });
   </script>
   <!--------------------- ------------------------ Chart-1 --------------------------------------------------------->
   <body> 
    <div id="Container-1" class="container"></div> 
  </body> 
 <script>
  var chartT = new Highcharts.Chart({ 
  chart:{ 
      renderTo : 'Container-1',
      animation : false,
      resetZoomButton: {position: { x: -5, y: -40}},
      zoomType: 'x'},
  title: {text: 'Temperatur' },
  xAxis: {type: 'datetime'}, 
  yAxis: { title: { text: 'Temperatur &degC' }}, 
  rangeSelector: { 
    buttons: [{ text: '+', 
    events:
      { click() { return false }}},
      { text: '-', events: {click() { return false}}}]},
  series: [ 
      {name: 'Temperatur innen(%Chart_Anzahl_Werte_ti% Werte)', 
      data: [%Chart_temp_in%], color:'red'   },
     {name: 'Temperatur au&szligen(%Chart_Anzahl_Werte_ta% Werte)', 
      data: [%Chart_temp_out%], color:'green' } ]});                                    
   </script>
   </html>
 <!--------------------- ------------------------ Chart-2 --------------------------------------------------------->  
  <body> 
    <div id="Container-2" class="container"></div> 
  </body> 
 <script>
  var chartT = new Highcharts.Chart({ 
  chart:{ 
      renderTo : 'Container-2',
      animation : false,
      resetZoomButton: {position: { x: -5, y: -40}},
      zoomType: 'x'},
  title: {text: 'Luftfeuchtigkeit' },
  xAxis: {type: 'datetime'}, 
  yAxis: { title: { text: 'Luftfeuchtigkeit' }}, 
  rangeSelector: { 
    buttons: [{ text: '+', 
    events:
      { click() { return false }}},
      { text: '-', events: {click() { return false}}}]},
  series: [ 
    {name: 'Luftfeuchtigkeit innen(%Chart_Anzahl_Werte_hi% Werte)',
      data: [%Chart_air_in%], color:'red'   },
    {name: 'Luftfeuchtigkeit au&szligen(%Chart_Anzahl_Werte_ha% Werte)', 
      data: [%Chart_air_out%], color:'green' } ]});                                    
   </script>
   </html>
<!--------------------- ------------------------ Chart-3 --------------------------------------------------------->  
  <body> 
    <div id="Container-3" class="container"></div> 
  </body> 
 <script>
  var chartT = new Highcharts.Chart({ 
  chart:{ 
      renderTo : 'Container-3',
      animation : false,
      resetZoomButton: {position: { x: -5, y: -40}},
      zoomType: 'x'},
   title: {text: 'Taupunkte' },
   xAxis: {type: 'datetime'}, 
   yAxis: { title: { text: 'Taupunkttemperatur &degC' }}, 
   rangeSelector: { 
   buttons: [{ text: '+', 
    	events:
      		{ click() { return false }}},
      		{ text: '-', events: {click() { return false}}}]},
   series: [ 
    {name: 'Taupunkt innen(%Chart_Anzahl_Werte_tpi% Werte)',  
      data: [%Chart_tp_in%], color:'red' },
    {name: 'Taupunkt au&szligen(%Chart_Anzahl_Werte_tpa% Werte)',
      data: [%Chart_tp_out%], color:'green'},
    {name:'Delta Taupunkt(%Chart_Anzahl_Werte_tpd% Werte)',
      data: [%Chart_tp_delta%], color:'blue' } ]});                                    
   </script>
    <FONT SIZE="3"><b><br> &nbsp;&nbsp;&nbsp;&copy Programmtechnik Ulrich Schmerold <br> </b> 
                      <tt>  &nbsp;&nbsp;&nbsp; Bahnhofstra&szlige 6, 86836 Graben <br>
                            &nbsp;&nbsp;&nbsp; Tel: 08232 - 908698 <br>
                            &nbsp;&nbsp;&nbsp; Mail: info@taupunktluefter.de <br>&nbsp;
       </FONT>
   </html>
   <%HTML%>
)rawliteral"; 

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

String processor(const String& var)
{  
  Watchdog_reset(); 
 //-----------------------------------------  Variablen für die Tabelle in den HTML-Code einfügen
  
   if(var == "HTML_auto_refresh") return String(HTML_auto_refresh_rate);        
   if(var == "Version") return Software_version;
   if(var == "T1")      return String(t1);
   if(var == "T2")      return String(t2);
   if(var == "H1")      return String(h1);
   if(var == "H2")      return String(h2);
   if(var == "TP1")     return String(Taupunkt_1);
   if(var == "TP2")     return String(Taupunkt_2);
   if(var == "DeltaTP") return String(DeltaTP);
   if(var == "Stamp")   return make_time_stamp();
   
   if(var == "Color-1") if(rel == true) return "#009231"; else return "#364592";
   if(var == "Rel")     if(rel == true) return "L&uumlfter ist in Betrieb"; else return "L&uumlfter ist nicht in Betrieb";
   
   //-------------------------------------------- Sicherstellen, dass gerade keine Routine auf die Chronik-Daten zugreift -----
   if (chronik_is_busy)
   {
    HTML_processor_is_working = false; 
    return " ";
   }

   //--------------------------------------------------------------- Chronik Datensätze in den HTML-Code einfügen -------------
   if (use_Highcharts == false) return "";
   else {
        if(var == "Chart_temp_in")  return Chart_temp_in ;
        if(var == "Chart_temp_out") return Chart_temp_out ;
   
        if(var == "Chart_air_in")   return Chart_air_in ;  
        if(var == "Chart_air_out")  return Chart_air_out ;  

        if(var == "Chart_tp_in")    return Chart_tp_in ;  
        if(var == "Chart_tp_out")   return Chart_tp_out ;  
        if(var == "Chart_tp_delta") return Chart_tp_delta ; 
        
        if(var == "Chart_Anzahl_Werte_ti") return String(Chart_temp_in.length() / laenge_Datensatz);
        if(var == "Chart_Anzahl_Werte_ta") return String(Chart_temp_out.length() / laenge_Datensatz);
        if(var == "Chart_Anzahl_Werte_hi") return String(Chart_air_in.length() / laenge_Datensatz);
        if(var == "Chart_Anzahl_Werte_ha") return String(Chart_air_out.length() / laenge_Datensatz);
        if(var == "Chart_Anzahl_Werte_tpi") return String(Chart_tp_in.length() / laenge_Datensatz);
        if(var == "Chart_Anzahl_Werte_tpa") return String(Chart_tp_out.length() / laenge_Datensatz);
        if(var == "Chart_Anzahl_Werte_tpd") return String(Chart_tp_delta.length() / laenge_Datensatz);
 
      };
   //------------------------------------------------------------------------------------------------------------------------------
   if(var == "HTML"){  HTML_processor_is_working = false; 
                       Serial_Debugging_println("HTML_processor ist fertig!");                
                       return "/html";
                    };   
}
//----------------------------------------------------------------------------------------------------------------------------------
