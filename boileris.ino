void Boilerio_sildymas(){
  #ifdef DEBUGboileris
  if (config.Bo_Siurblio_busena == true) Serial.println("Boilerio siublio busena: IJUNGTAS *** ON ***");
    if (config.Bo_Siurblio_busena == false) Serial.println("Boilerio siublio busena: ISUNGTAS *** OFF ***");
  #endif 

// Jei  boilerio viršus šaltesnis už boilerio šildymo išjungimo temperatūrą, 
// o katilo išėjime yra daugiau šilumos, negu nustatyta boilerio šildymo pradžios  
// temperatūra +10 C, tai įjungiamas boilerio siurblys
     if ((Boileris < config.Bo_ON_T || Boileris < config.Bo_OFF_T) && Katilas > config.Bo_ON_T + 10 && config.Bo_Siurblio_busena == false){
       if (Katilas > Boileris + 10){
       digitalWrite(BoSiurblys, HIGH); 
       config.Bo_Siurblio_busena = true;
#ifdef DEBUGboileris
Serial.println("Boilerio siublys IJUNGTAS ***_ON_ ***__PRIEZASTIS:");
Serial.print("B isjungimo temperatura- ");Serial.print(config.Bo_OFF_T);Serial.print(char(186));Serial.println("C");
Serial.print("B ijungimo temperatura- ");Serial.print(config.Bo_ON_T);Serial.print(char(186));Serial.println("C");
Serial.print("I isejime temperatura- ");Serial.print(Katilas);Serial.print(char(186));Serial.println("C");
#endif 
   }}
// Jei boilerio viršuje yra tiek šilumos, kiek nustatyta, arba katilo išėjime yra mažiau šilumos, 
// negu boilerio viršuje, tai siurblys išjungiamas
     if ((Boileris >= config.Bo_OFF_T || Katilas <= Boileris + 8) && (config.Bo_Siurblio_busena == true)) { 
       digitalWrite(BoSiurblys, LOW); 
       config.Bo_Siurblio_busena = false;
#ifdef DEBUGboileris
Serial.println("Boilerio siublys ISUNGTAS ***_OFF_***__PRIEZASTIS:");
Serial.print("B isjungimo temperatura- ");Serial.print(config.Bo_OFF_T);Serial.print(char(186));Serial.println("C");
Serial.print("B ijungimo temperatura- ");Serial.print(config.Bo_ON_T);Serial.print(char(186));Serial.println("C");
Serial.print("I isejime temperatura- ");Serial.print(Katilas);Serial.print(char(186));Serial.println("C");
#endif      
   }     
}

//**************************************************************************************************************
void Boilerio_termostatas(){
// Jei boilerio viršus šaltesnis negu nustatyta ijungimo temperatura, arba boilerio viršus šaltesnis
// už boilerio išjungimo temperatūrą, ir įjungtas termostatas, tai įjungiamas elektrinis boilerio šildymas
if ((Boileris < config.Bo_ON_T ) && (config.Bo_Termostatas == true)){
       digitalWrite(BoTermostatas, HIGH); 
//       config.Bo_Termostato_busena = true; // zyme, kad termostatas dabar veikia
       Bo_thermostatState = "Įjungta";    // kai ijungiamas elektrinis boilerio sildymas, pakeičiama įjungimo žymė
#ifdef DEBUGboileris
Serial.print("Boilerio sildymas elektra I_J_U_N_G_T_A_S ***_ON_ *** iki- "); 
Serial.print(config.Bo_OFF_T);Serial.print(char(186));Serial.println("C");
Serial.println();
#endif 
   }
// Jei boilerio virsuje yra tiek šilumos, kiek nustatyta,  tai termostatas išjungia boilerio šildymą elektra
     if (((Boileris >= config.Bo_OFF_T) && (Bo_thermostatState == "Įjungta")) or config.Bo_Termostatas == false){ 
       digitalWrite(BoTermostatas, LOW);
//       config.Bo_Termostato_busena = false;
       Bo_thermostatState = "Išjungta";    // kai išjungiamas elektrinis boilerio šildymas, pakeičiama išjungimo žymė
#ifdef DEBUGboileris
Serial.println("Ijungtas boilerio sildymas elektra I*S*J*U*N*G*T*A*S ***_OFF_ *** nuo- ");
Serial.print(Boileris);Serial.print(char(186));Serial.println("C");
#endif      
   }     
}
