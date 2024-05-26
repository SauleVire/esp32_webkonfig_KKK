//------------------------------------------
//Convert device id to String
String GetAddressToString(DeviceAddress deviceAddress){
  String str = "";
  for (uint8_t i = 0; i < 8; i++){
    if( deviceAddress[i] < 16 ) str += String(0, HEX);
    str += String(deviceAddress[i], HEX);
  }
  return str;
}

//Loop measuring the temperature
/* void TempLoop(long now){
  if( now - lastTemp > durationTemp ){ //Take a measurement at a fixed time (durationTemp = 5000ms, 5s)
    for(int i=0; i<numberOfDevices; i++){
      float tempC = DS18B20.getTempC( devAddr[i] ); //Measuring temperature in Celsius
      tempDev[i] = tempC; //Save the measured value to the array
    }
    DS18B20.setWaitForConversion(false); //No waiting for measurement
    DS18B20.requestTemperatures(); //Initiate the temperature measurement
    lastTemp = millis();  //Remember the last time measurement
  }
}
*/
//Setting the temperature sensor
void SetupDS18B20(){
  DS18B20.begin();

  Serial.print("Parasite power is: "); 
  if( DS18B20.isParasitePowerMode() ){ 
    Serial.println("ON");
  }else{
    Serial.println("OFF");
  }
  
  numberOfDevices = DS18B20.getDeviceCount();
  Serial.print( "Device count: " );
  Serial.println( numberOfDevices );

  lastTemp = millis();
  DS18B20.requestTemperatures();

  // Loop through each device, print out address
  for(int i=0;i<numberOfDevices; i++){
    // Search the wire for address
    if( DS18B20.getAddress(devAddr[i], i) ){
      //devAddr[i] = tempDeviceAddress;
      Serial.print("Found device ");
      Serial.print(i, DEC);
      Serial.print(" with address: " + GetAddressToString(devAddr[i]));
      Serial.println();
    }else{
      Serial.print("Found ghost device at ");
      Serial.print(i, DEC);
      Serial.print(" but could not detect address. Check power and cabling");
    }

    //Get resolution of DS18b20
    Serial.print("Resolution: ");
    Serial.print(DS18B20.getResolution( devAddr[i] ));
    Serial.println();

    //Read temperature from DS18b20
    float tempC = DS18B20.getTempC( devAddr[i] );
    Serial.print("Temp C: ");
    Serial.println(tempC);
  }
}

// reads the temp and humidity from the DHT sensor and sets the global variables for both

void TemteraturosMatavimas() {
  Kolektorius = DS18B20.getTempC(devAddr[config.Kid]);
/*    if(DS18B20.getTempC(devAddr[config.Kid]) == DEVICE_DISCONNECTED_C) {    
    Serial.println("!! Klaida ! Ds18B20 Kolektorius rodmenys neteisingi");
    Serial.println("DEVICE_DISCONNECTED_C - "+DEVICE_DISCONNECTED_C);
    return;                                } */
  Boileris = DS18B20.getTempC(devAddr[config.Bid]);
  OrasL = DS18B20.getTempC(devAddr[config.OLid]);
  OrasK = DS18B20.getTempC(devAddr[config.OKid]);
  AkumuliacineV = DS18B20.getTempC(devAddr[config.AVid]);
  AkumuliacineA = DS18B20.getTempC(devAddr[config.AAid]);
  PVoztuvas = DS18B20.getTempC(devAddr[config.PVid]);
  Katilas = DS18B20.getTempC(devAddr[config.KKKid]);

  // Check if any reads failed and exit
  
/*  if (Kolektorius == -127 or Kolektorius == 85 or Kolektorius > 127 ) {
    Kolektorius = KolektoriusOld;
    Serial.println("Klaida! Ds18B20 Kolektorius rodmenys neteisingi");  }
    else { KolektoriusOld = Kolektorius;}

  if (Boileris == -127 or Boileris == 85 or Boileris > 127 ) {
    Boileris = BoilerisOld;
    Serial.println("Klaida! Ds18B20 Boileris rodmenys neteisingi");  }
    else { BoilerisOld = Boileris;}
    */
/*
  if (OrasL == -127 or OrasL == 85 or OrasL > 127 ) {
    OrasL = OrasLOld;
    Serial.println("Klaida! Ds18B20 OrasL rodmenys neteisingi");  }
    else { OrasLOld = OrasL;}

  if (OrasK == -127 or OrasK == 85 or OrasK > 127 ) {
    OrasK = OrasKOld;
    Serial.println("Klaida! Ds18B20 OrasK rodmenys neteisingi");  }
    else { OrasKOld = OrasK;}

  if (Katilas == -127 or Katilas == 85 or Katilas > 127 ) {
    Katilas = KatilasOld;
    Serial.println("Klaida! Ds18B20 Katilas rodmenys neteisingi");  }
    else { KatilasOld = Katilas;}

  if (AkumuliacineV == -127 or AkumuliacineV == 85 or AkumuliacineV > 127 ) {
    AkumuliacineV = AkumuliacineVOld;
    Serial.println("Klaida! Ds18B20 AkumuliacineV rodmenys neteisingi");  }
    else { AkumuliacineVOld = AkumuliacineV;}

  if (AkumuliacineA == -127 or AkumuliacineA == 85 or AkumuliacineA > 127 ) {
    AkumuliacineA = AkumuliacineAOld;
    Serial.println("Klaida! Ds18B20 AkumuliacineA rodmenys neteisingi");  }
    else { AkumuliacineAOld = AkumuliacineA;}

  if (PVoztuvas == -127 or PVoztuvas == 85 or PVoztuvas > 127 ) {
    PVoztuvas = PVoztuvasOld;
    Serial.println("Klaida! Ds18B20 PVoztuvas rodmenys neteisingi");  }
    else { PVoztuvasOld = PVoztuvas;}
*/
  DS18B20.requestTemperatures();
  
}
// saulės kolektoriaus siurblio valdymas
void Siurblys(){
  if (config.k_uzsalimas == 1) {
    FreezingState = "Įjungta"; }
  else {
    FreezingState = "Išjungta"; }

  if (config.k_nuorinimas == 1) {
    Nuorinimas = "Įjungta"; } // collector refactiveness
  else {
    Nuorinimas = "Išjungta"; }

  if (config.k_ziema == 0) {
    Ziema = "Ne!";  // summer time


// ************ Siurblio įjungimas ********************
if (Kolektorius > Boileris + config.k_skirtumas) 
      { digitalWrite(SKSiurblys, HIGH); 
      CollectorState = "Įjungtas";

//       Serial.print("\nSiurblio rele įjungta ON (Siurblio ciklas)\n");
      }
      if (((Kolektorius < config.k_uzsalimo_t) & (config.k_uzsalimas == 1)) or (config.k_nuorinimas == 1))
      {  digitalWrite(SKSiurblys, HIGH); 
      CollectorState = "Įjungtas";
//      k_uzsalimas_status = "Įjungta";
      }
      // ************* Siurblio išjungimas ******************
      if ((Kolektorius < Boileris + config.k_skirtumas) & (config.k_nuorinimas == 0 ) & (Kolektorius > config.k_uzsalimo_t)) 
      {  digitalWrite(SKSiurblys, LOW); 
      CollectorState = "Išjungtas";
//      k_nuorinimas_status = "Išjungta";
      }
}
  else {
    Ziema = "Taip!"; 
    digitalWrite(SKSiurblys, LOW); 
    CollectorState = "Išjungtas";}

}