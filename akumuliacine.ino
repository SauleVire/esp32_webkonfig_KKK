
void Akumuliacine_talpa () {
  // jei katilo išėjime yra daugiau šilumos, negu nustatyta akumuliacinės talpos šildymo įjungimo temperatūra,
  // tai įjungiamas akumuliacinės talpos siurblys
 if ((Katilas >= config.At_ON_T) && (config.At_Siurblio_busena == false))  {
   digitalWrite(AtSiurblys, LOW); // Įjungiamas akumuliacinės talpos siurblys
   config.At_Siurblio_busena = true;
// jei AT išjungimo temperatūra nustatoma didesnė, negu įjungimo, tai ji keičiama į mažesnę
if (config.At_OFF_T > config.At_ON_T) config.At_OFF_T = config.At_ON_T - 1;
#ifdef DEBUGakumuliacine
Serial.print("AT ijungimo temperatura- ");Serial.print(config.At_ON_T);Serial.print(char(186));Serial.println("C");
Serial.println("Akumuliacines talpos siublys IJUNGTAS *** ON ***");
#endif
              }
              // jei katilo išėjime yra mažiau šilumos, negu nustatyta akumuliacinės talpos šildymo išjungimo temperatūra,
              // tai išjungiamas akumuliacinės talpos siurblys
              if ((Katilas <= config.At_OFF_T) && (config.At_Siurblio_busena == true)){
              digitalWrite(AtSiurblys, HIGH); // Išjungiamas akumuliacinės talpos siurblys
              config.At_Siurblio_busena = false;
              #ifdef DEBUGakumuliacine
              Serial.print("AT isjungimo temperatura- ");Serial.println(config.At_OFF_T);Serial.print(char(186));Serial.println("C");
              Serial.println("Akumuliacines talpos siublys ISJUNGTAS *** OFF ***");
              #endif
                   } 
}
/*
// Praėjus nustatytam laikui ir jei tenkinama sąlyga išjungimui pagal temperatūras
// įrašoma žymė, kad baigėsi rankinio siurblio įjungimo veikimas
void Akumuliacine_talpa_rankinis (){
  if ((millis() < AT_rankinio_ijungimo_laikas + AT_rankinio_ijungimo_trukme ) && (KI <= At_OFF_T)){
  } else {AT_rankinis_ijungimas = false;}
}
*/
