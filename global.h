#ifndef GLOBAL_H
#define GLOBAL_H

String FIRMWARE_VERSION = "v2.1";
/* 
2019_04_16 19:59 v1.1 Programos naujinimas, pataisyti užšalimo tikrinimo, temperatūrų matavimo algoritmai,
pridėtas nuorinimas
2019_09_25 v2.0 pridėta akumuliacinės talpos, boilerio, KKK valdymas
2022_12_29 v2.1 ištaisyta WiFi dalis, veikia tiek DHCP, tiek statinis adresų priskyrimas
*/
 
WebServer server(80);                  // The Webserver
//ESP8266HTTPUpdateServer httpUpdater;
const char Naujinimas[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
  <script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>
  <div class="smartphone">
  <div class="content">
  <form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>
  <input type='file' name='update'>
  <input type='submit' style="width:100px" class="myButton" value='Naujinti'>
  </form>
  <div id='prg'>progress: 0%</div>
  <hr>
<center><a href="admin.html" style="width:150px" class="myButton">Valdiklio konfigūravimas</a><br>
<a href="/"style="width1250px"  class="myButton" >Valdiklio būsena</a></center>
  </div>
  <script>
window.onload = function ()
{	load("style.css","css", function() 
	{		load("microajax.js","js", function() 
		{				// Do something after load...
		});
	});
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}

  $('form').submit(function(e){
  e.preventDefault();
  var form = $('#upload_form')[0];
  var data = new FormData(form);
   $.ajax({
  url: '/update',
  type: 'POST',
  data: data,
  contentType: false,
  processData:false,
  xhr: function() {
  var xhr = new window.XMLHttpRequest();
  xhr.upload.addEventListener('progress', function(evt) {
  if (evt.lengthComputable) {
  var per = evt.loaded / evt.total;
  $('#prg').html('progress: ' + Math.round(per*100) + '%');
  }
  }, false);
  return xhr;
  },

  success:function(d, s) {
  console.log('success!')

 },

 error: function (a, b, c) {

 }
 });
 });

</script>
)=====";


boolean firstStart = true;                    // On firststart = true, NTP will try to get a valid time
int AdminTimeOutCounter = 0;                  // Counter for Disabling the AdminMode
strDateTime DateTime;                     // Global DateTime structure, will be refreshed every Second
WiFiUDP UDPNTPClient;                     // NTP Client
unsigned long UnixTimestamp = 0;                // GLOBALTIME  ( Will be set by NTP)
boolean Refresh = false; // For Main Loop, to refresh things like GPIO / WS2812
int cNTP_Update = 0;                      // Counter for Updating the time via NTP
Ticker tkSecond;                        // Second - Timer for Updating Datetime Structure
boolean AdminEnabled = true;    // Enable Admin Mode for a given Time
byte Minute_Old = 240;        // Helpvariable for checking, when a new Minute comes up (for Auto Turn On / Off)

// Generally, you should use "unsigned long" for variables that hold time to handle rollover
unsigned long previousMillis = 0;        // will store last temp was read
unsigned long previousMillis1 = 0;       // will store last temp was read, emoncms write data
unsigned long previousMillis2 = 0;       // will store last temp was read, emoncms write data
 long interval = 20000;              // interval at which to read sensor

struct strConfig {
  String ssid;
  String password;
  byte  IP[4];
  byte  DNS[4];
  byte  Netmask[4];
  byte  Gateway[4];
  boolean dhcp;
  String ntpServerName;
  long Update_Time_Via_NTP_Every;
  int timezone;
  boolean daylight;
  String DeviceName;
  boolean AutoTurnOff;
  boolean AutoTurnOn;
  byte TurnOffHour;
  byte TurnOffMinute;
  byte TurnOnHour;
  byte TurnOnMinute;
/* ********** kintamieji saulės kolektoriui ******************* */
 float k_skirtumas;
 float k_uzsalimo_t;
 int k_intervalas;
 boolean k_uzsalimas;
 boolean k_nuorinimas;
 boolean k_ziema;

  String emoncmsSrv;
  String apikey;
  String reiksme1;
  String reiksme2;
  String reiksme3;
  String katalogas;
  long intervalasEmon;
  boolean emoncmsOn;
  byte Kid; //Kolektoriaus Ds18b20 id
  byte Bid; //Boilerio Ds18b20 id
  byte OLid; //Oro lauko Ds18b20 id
  byte OKid; //Oro kambario Ds18b20 id
/* ********** kintamieji Boileriui ******************* */
  int Bo_ON_T; // temperatūra boilerio siurbliui įjungti
  int Bo_OFF_T; // temperatūra boilerio siurbliui įšjungti
  boolean Bo_Rankinis_ijungimas; // Žymė rankiniam Boilerio siurblio valdymui
  boolean Bo_Termostatas; // Žymė rankiniam termostato įjungimui
  boolean Bo_Termostato_busena; // Žymė termostato busenai
  boolean Bo_Siurblio_busena; // Žymė termostato busenai
  /* ********** kintamieji Akumuliacinei talpai ******************* */
  byte At_ON_T;//   temperatūra akumuliacines talpos siurbliui įjungti
  byte At_OFF_T;//  temperatūra akumuliacines talpos siurbliui įšjungti
  byte At_Rankinis_ijungimas; // žymė rankiniam Akumuliacinės talpos siurblio valdymui
  boolean At_Siurblio_busena; //akumuliacinės talpos siurblio būsena  0-išjungta, 1-įjungta
  byte AVid; //Akumuliacinės viršaus Ds18b20 id
  byte AAid; //Akumuliacinės apačios Ds18b20 id
/* ************** kintamieji KKK ************************************ */
  byte KKKid; //KKK Ds18b20 id
/* ************** kintamieji PID algoritmui ************************************ */
  double Kp;
  double Ki;
  double Kd;
  int WindowSize;
/* ************************************************** */


/* ********** kintamieji Pamaišymo vožtuvui ******************* */
  byte  PV_palaikoma_T; // PV palaikoma temperatūra 
  float PV_tolerancijos_T; // PV palaikomos temperatūros riba 
  byte  PV_ON_T; // PV įjungimo temperatūra 
  byte  PV_OFF_T; // PV išjungimo temperatūra 
  byte  PV_rankinis_ijungimas; // Žymė rankiniam PV valdymui
  byte  PV_rezimas; // 1- rankinis, 2- automatinis
  byte  PVid; //Pamaišymo vožtuvo Ds18b20 id

/* ********************************************************************** */
}   config;

/*
**
********* temperatūros jutikliai ds18b20 **************
**
*/

//------------------------------------------
//DS18B20
#define ONE_WIRE_BUS 14 //Pin to which is attached a temperature sensor
#define ONE_WIRE_MAX_DEV 10 //The maximum number of devices

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
int numberOfDevices; //Number of temperature devices found
DeviceAddress devAddr[ONE_WIRE_MAX_DEV];  //An array device temperature sensors
float tempDev[ONE_WIRE_MAX_DEV]; //Saving the last measurement of temperature
float tempDevLast[ONE_WIRE_MAX_DEV]; //Previous temperature measurement
long lastTemp; //The last measurement
const int durationTemp = 5000; //The frequency of temperature measurement

//kintamieji saugoti ismatuotoms temperaturu reiksmems
// Katilas- katilo pagaminta šiluma
// Boileris- boilerio viršaus temperatūra
// PVoztuvas- temperatūra po pavaišimo vožtuvo
// LOras, KOras- lauko, kambario oras
float Kolektorius, Boileris, OrasL, OrasK, Katilas, AkumuliacineV, AkumuliacineA, PVoztuvas;
float KolektoriusOld=0, BoilerisOld=0, OrasLOld=0, OrasKOld=0, KatilasOld=0, AkumuliacineVOld=0, AkumuliacineAOld=0, PVoztuvasOld=0;

// Pamaišymo vožtuvo darbiniai kintamieji
  float   PV_klaida;
  boolean PV_atidarytas;
  boolean PV_uzdarytas;
  boolean PV_atidarinejamas;
  boolean PV_uzdarinejamas;
  boolean PV_pusiausvyra;
  boolean PV_stop;
  /* ********** kintamieji siurbliams ******************* */
boolean SK_siurblys = false;
boolean PV_siurblys = false;
boolean AT_siurblys = false;
boolean B_siurblys = false;
#define REQUEST_freezing 5000   // 5000 millis= 5 sekundės
//static long timer_freezing=0;   // apsaugos nuo užšalimo tikrinimo laikas

unsigned long PV_pauze =20000;
unsigned long PV_pauzes_pertrauka =20000;
unsigned long PV_atidarinejimo_laikas =0;
unsigned long PV_uzdarinejimo_laikas =0;
unsigned long PV_atidarinejimo_pertrauka =6000;
unsigned long PV_uzdarinejimo_pertrauka =6000;
unsigned long Boilerio_siurblio_ijungimo_laikas;
unsigned long Boilerio_siurblio_pertrauka =60000;
byte PV_darinejimas = 60;

// int WindowSize = 30000;
unsigned long windowStartTime;
/////////////////////////////////////////////////////////////////////////////////
boolean Laikmatis = false;
/////////////////////////////////////////////////////////////////////////////////

/* ******************** Relays *********************************** */

String Nuorinimas = "išjungta";
String CollectorState = "išjungta";
String FreezingState = "išjungta";
String Bo_thermostatState = "išjungta";
String Ziema = "išjungta";

#define SKSiurblys 33  // Saulės kolektoriaus siurblys, Relė 1 (iš kairės) IO33
#define AtSiurblys 32 // išvadas ak. talpos siurblio junginėjimui, Relė 2 (iš kairės) IO32
#define BoTermostatas 13 // išvadas boilerio termostatui, Relė 3 (iš kairės) IO13
#define BoSiurblys 12 // išvadas boilerio siurbliui, Relė 4 (iš kairės) IO12
#define PVsiurblys 21 // išvadas pamaišymo vožtuvo siurbliui, Relė 5 (iš kairės) IO21
#define PVuzdarymas 19 // PV uždarymas Relė 6 (iš kairės) IO19
#define PVatidarymas 18 // PV atidarymas Relė 7 (iš kairės) IO18
#define Laisva 5 // Laisva Relė 8 (iš kairės) IO5
/*
**
** emoncms duomenų siuntimas
**
*/
// viskas konfigūruojama per naršyklę!
//
//const char* emoncmshost = "xxx"; //Enter the EmonCMS Server address here
//const char* apikey = "xxx"; //Enter api key here
/*
**
** CONFIGURATION HANDLING
**
*/
void ConfigureWifi()
{
  Serial.println("Configuring Wifi");
  WiFi.begin (config.ssid.c_str(), config.password.c_str());
  WiFi.setHostname("Vire");
  Serial.println(WiFi.localIP().toString());
  if (!config.dhcp)
  {
		WiFi.config(
      IPAddress(config.IP[0],config.IP[1],config.IP[2],config.IP[3] ),  
      IPAddress(config.Gateway[0],config.Gateway[1],config.Gateway[2],config.Gateway[3] ) , 
      IPAddress(config.Netmask[0],config.Netmask[1],config.Netmask[2],config.Netmask[3] ) ,
      IPAddress(config.DNS[0],config.DNS[1],config.DNS[2],config.DNS[3] )
               );
    Serial.print("\nIP- ");Serial.print(WiFi.localIP());
    Serial.print("\nDNS- ");Serial.print(WiFi.dnsIP());
    Serial.print("\nKauke- ");Serial.print(WiFi.subnetMask());
    Serial.print("\nGateway: ");Serial.println(WiFi.gatewayIP());
  
  Serial.print("SSID'as: " + String(config.ssid) + "\n");
  Serial.print("Slaptažodis: " + String(config.password) + "\n\n");
  }
}

void WriteConfig()
{

  Serial.println("Writing Config");
  EEPROM.write(0,'C');  EEPROM.write(1,'F');  EEPROM.write(2,'G');
  
  EEPROM.write(10,config.Kid);
  EEPROM.write(11,config.Bid);
  EEPROM.write(12,config.OLid);
  EEPROM.write(13,config.emoncmsOn);
  EEPROM.write(14,config.k_nuorinimas);
  EEPROM.write(15,config.k_uzsalimas);
  EEPROM.write(16,config.dhcp);
  EEPROM.write(17,config.daylight);
  
  EEPROMWritelong(18,config.Update_Time_Via_NTP_Every); // 4 Byte
  EEPROM.write(22,config.timezone);  // 4 Byte

  EEPROM.write(32,config.IP[0]);
  EEPROM.write(33,config.IP[1]);
  EEPROM.write(34,config.IP[2]);
  EEPROM.write(35,config.IP[3]);
    EEPROM.write(36,config.DNS[0]);
    EEPROM.write(37,config.DNS[1]);
    EEPROM.write(38,config.DNS[2]);
    EEPROM.write(39,config.DNS[3]);
  EEPROM.write(40,config.Netmask[0]);
  EEPROM.write(41,config.Netmask[1]);
  EEPROM.write(42,config.Netmask[2]);
  EEPROM.write(43,config.Netmask[3]);
    EEPROM.write(44,config.Gateway[0]);
    EEPROM.write(45,config.Gateway[1]);
    EEPROM.write(46,config.Gateway[2]);
    EEPROM.write(47,config.Gateway[3]);
  
  EEPROM.write(48,config.k_intervalas); //
  EEPROM.write(52,config.intervalasEmon); 
  EEPROM.write(56,config.k_skirtumas);
  EEPROM.write(161,config.k_ziema);
  WriteStringToEEPROM(64,config.ssid);
  WriteStringToEEPROM(96,config.password);
  WriteStringToEEPROM(128,config.ntpServerName);//16

  EEPROM.write(144,config.AutoTurnOn);
  EEPROM.write(145,config.AutoTurnOff);
  EEPROM.write(146,config.TurnOnHour);
  EEPROM.write(147,config.TurnOnMinute);
  EEPROM.write(148,config.TurnOffHour);
  EEPROM.write(149,config.TurnOffMinute);

  EEPROM.write(150,config.Bo_ON_T);
  EEPROM.write(154,config.Bo_OFF_T);
  EEPROM.write(159,config.Bo_Rankinis_ijungimas);
  EEPROM.write(160,config.Bo_Termostatas);
//  EEPROM.write(161,config.Bo_Termostato_busena);

  EEPROM.write(162,config.At_ON_T);
  EEPROM.write(166,config.At_OFF_T);
  EEPROM.write(170,config.At_Rankinis_ijungimas);

  EEPROM.write(171,config.Kid);
  EEPROM.write(175,config.Bid);
  EEPROM.write(179,config.OLid);
  EEPROM.write(183,config.AAid);
  EEPROM.write(187,config.AVid);
  EEPROM.write(191,config.KKKid);

  EEPROM.write(195,config.PV_palaikoma_T);
  EEPROM.write(199,config.PV_ON_T);
  EEPROM.write(203,config.PV_OFF_T);
  EEPROM.write(207,config.PV_tolerancijos_T);
  EEPROM.write(215,config.PV_rankinis_ijungimas);

  WriteStringToEEPROM(306,config.DeviceName);
  WriteStringToEEPROM(321,config.reiksme1);
  WriteStringToEEPROM(328,config.reiksme2);
  WriteStringToEEPROM(335,config.reiksme3);
  WriteStringToEEPROM(342,config.katalogas);
  WriteStringToEEPROM(350,config.emoncmsSrv);
  WriteStringToEEPROM(366,config.apikey); 

  EEPROM.put(398,config.Kp);
  EEPROM.put(406,config.Ki);
  EEPROM.put(414,config.Kd);
  EEPROM.put(422,config.WindowSize); //laisva nuo 405

  EEPROM.commit();EEPROM.end();
}
boolean ReadConfig()
{

  Serial.println("Reading Configuration");
  if (EEPROM.read(0) == 'C' && EEPROM.read(1) == 'F'  && EEPROM.read(2) == 'G' )
  {
    Serial.println("Configurarion Found!");
    config.Kid =   EEPROM.read(10);
    config.Bid =   EEPROM.read(11);
    config.OLid =   EEPROM.read(12);
    config.emoncmsOn =   EEPROM.read(13);
    config.k_nuorinimas =   EEPROM.read(14);
    config.k_uzsalimas =   EEPROM.read(15);
    config.dhcp =   EEPROM.read(16);

    config.daylight = EEPROM.read(17);
    config.Update_Time_Via_NTP_Every = EEPROMReadlong(18); // 4 Byte
    config.timezone = EEPROM.read(22); // 4 Byte

    config.IP[0] = EEPROM.read(32);
    config.IP[1] = EEPROM.read(33);
    config.IP[2] = EEPROM.read(34);
    config.IP[3] = EEPROM.read(35);
      config.DNS[0] = EEPROM.read(36);
      config.DNS[1] = EEPROM.read(37);
      config.DNS[2] = EEPROM.read(38);
      config.DNS[3] = EEPROM.read(39);
    config.Netmask[0] = EEPROM.read(40);
    config.Netmask[1] = EEPROM.read(41);
    config.Netmask[2] = EEPROM.read(42);
    config.Netmask[3] = EEPROM.read(43);
      config.Gateway[0] = EEPROM.read(44);
      config.Gateway[1] = EEPROM.read(45);
      config.Gateway[2] = EEPROM.read(46);
      config.Gateway[3] = EEPROM.read(47);

    config.k_intervalas = EEPROM.read(48);
    config.intervalasEmon = EEPROM.read(52);
    config.k_skirtumas = EEPROM.read(56);
    config.k_ziema = EEPROM.read(161);

    config.ssid = ReadStringFromEEPROM(64);
    config.password = ReadStringFromEEPROM(96);
    config.ntpServerName = ReadStringFromEEPROM(128);//16
    
    config.AutoTurnOn = EEPROM.read(144);
    config.AutoTurnOff = EEPROM.read(145);
    config.TurnOnHour = EEPROM.read(146);
    config.TurnOnMinute = EEPROM.read(147);
    config.TurnOffHour = EEPROM.read(148);
    config.TurnOffMinute = EEPROM.read(149);

    config.Bo_ON_T = EEPROM.read(150);
    config.Bo_OFF_T = EEPROM.read(154);
    config.Bo_Rankinis_ijungimas = EEPROM.read(159);
    config.Bo_Termostatas = EEPROM.read(160);
//    config.Bo_Termostato_busena = EEPROM.read(161);

    config.At_ON_T = EEPROM.read(162);
    config.At_OFF_T = EEPROM.read(166);
    config.At_Rankinis_ijungimas = EEPROM.read(170);

    config.Kid = EEPROM.read(171);
    config.Bid = EEPROM.read(175);
    config.OLid = EEPROM.read(179);
    config.AAid = EEPROM.read(183);
    config.AVid = EEPROM.read(187);
    config.KKKid = EEPROM.read(191);

 config.PV_palaikoma_T = EEPROM.read(195);
 config.PV_ON_T = EEPROM.read(199);
 config.PV_OFF_T = EEPROM.read(203);
 config.PV_tolerancijos_T = EEPROM.read(207);
 config.PV_rankinis_ijungimas = EEPROM.read(215);

    config.DeviceName= ReadStringFromEEPROM(306);
    config.reiksme1= ReadStringFromEEPROM(321);
    config.reiksme2= ReadStringFromEEPROM(328);
    config.reiksme3= ReadStringFromEEPROM(335);
    config.katalogas= ReadStringFromEEPROM(342);
    config.emoncmsSrv= ReadStringFromEEPROM(350);
    config.apikey = ReadStringFromEEPROM(366);

  EEPROM.get (398,config.Kp);
  EEPROM.get (406,config.Ki);
  EEPROM.get (414,config.Kd);
  EEPROM.get(422,config.WindowSize); //laisva nuo 405

    return true;
    
  }
  else
  {
    Serial.println("Configurarion NOT FOUND!!!!");
    return false;
  }
}
/*
**
**  NTP 
**
*/
//const int NTP_PACKET_SIZE = 48; 
byte packetBuffer[ NTP_PACKET_SIZE]; 
void NTPRefresh()
{

  if (WiFi.status() == WL_CONNECTED)
  {
    IPAddress timeServerIP; 
    WiFi.hostByName(config.ntpServerName.c_str(), timeServerIP); 
    //sendNTPpacket(timeServerIP); // send an NTP packet to a time server


    Serial.println("sending NTP packet...");
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    packetBuffer[1] = 0;     // Stratum, or type of clock
    packetBuffer[2] = 6;     // Polling Interval
    packetBuffer[3] = 0xEC;  // Peer Clock Precision
    packetBuffer[12]  = 49;
    packetBuffer[13]  = 0x4E;
    packetBuffer[14]  = 49;
    packetBuffer[15]  = 52;
    UDPNTPClient.beginPacket(timeServerIP, 123); 
    UDPNTPClient.write(packetBuffer, NTP_PACKET_SIZE);
    UDPNTPClient.endPacket();


    delay(1000);
  
    int cb = UDPNTPClient.parsePacket();
    if (!cb) {
      Serial.println("NTP no packet yet");
    }
    else 
    {
      Serial.print("NTP packet received, length=");
      Serial.println(cb);
      UDPNTPClient.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
      unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
      unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
      unsigned long secsSince1900 = highWord << 16 | lowWord;
      const unsigned long seventyYears = 2208988800UL;
      unsigned long epoch = secsSince1900 - seventyYears;
      UnixTimestamp = epoch;
    }
  }
}

void Second_Tick()
{
  strDateTime tempDateTime;
  AdminTimeOutCounter++;
  cNTP_Update++;
  UnixTimestamp++;
  ConvertUnixTimeStamp(UnixTimestamp +  (config.timezone *  360) , &tempDateTime);
  if (config.daylight) // Sommerzeit beachten
    if (summertime(tempDateTime.year,tempDateTime.month,tempDateTime.day,tempDateTime.hour,0))
    {
      ConvertUnixTimeStamp(UnixTimestamp +  (config.timezone *  360) + 3600, &DateTime);
    }
    else
    {
      DateTime = tempDateTime;
    }
  else
  {
      DateTime = tempDateTime;
  }
  Refresh = true;
}
 

#endif
