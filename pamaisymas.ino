/* ********** kintamieji Pamaišymo vožtuvui *******************
byte PV_palaikoma_T = 40; // PV palaikoma temperatūra
byte PV_atidarymo_T = 30; // PV palaikoma temperatūra
byte PV_rankinis_ijungimas = 0; // Žymė rankiniam PV valdymui
boolean PV_uzdarytas = true;
boolean PV_atidarytas = false;
boolean PV_atidarinejamas = false;
boolean PV_uzdarinejamas = false;
boolean PV_stop = true;
************************************************************************
#define PV_pilno_atidarymo_trukme 2500
unsigned long PV_pilno_atidarymo_laikas;
unsigned long PV_darinejimo_laikas;
/* ---PAMAISYMO VOZTUVO SIURBLIO ĮJUNGIMAS------- */
void PamaisymoVoztuvoSiusblys(){
  // Jei katilo temperatūra pakyla iki užduotos
if ((Katilas > config.PV_ON_T || AkumuliacineV > config.PV_ON_T) && (PV_siurblys == false)){ 
    digitalWrite(PVsiurblys, LOW); // įjungiamas siurblys 
    PV_siurblys = true; // pažymima, kad siurblys veikia
#ifdef DEBUGpv
Serial.print("PV ijungimo temperatura- ");Serial.print(config.PV_ON_T);Serial.print(char(186));Serial.println("C");
Serial.println("Pamaisymo voztuvo siublys IJUNGTAS *** ON ***");
#endif
}
    // Jei katilo temperatūra nukrenta žemiau užduotos
    if ((Katilas < config.PV_OFF_T) && (AkumuliacineV < config.PV_OFF_T) && (PV_siurblys == true)){ 
        digitalWrite(PVsiurblys, HIGH); // išjungiamas siurblys 
        PV_siurblys = false; // pažymima, kad siurblys neveikia
#ifdef DEBUGpv
Serial.print("PV isjungimo temperatura- ");Serial.print(config.PV_OFF_T);Serial.print(char(186));Serial.println("C");
Serial.println("Pamaisymo voztuvo siublys ISJUNGTAS *** OFF ***");
#endif
    }
}

/* ******************************************************************************* */
void PamaisymoVoztuvoDarbas()
  { 
// Jei katilo temperatūra didesnė, negu nustatyta pamaišymo vožtuvo palaikymui,
// vožtuvas A_T_I_D_A_R_I_N_E_J_A_M_A_S arba U_Z_D_A_R_I_N_E_J_A_M_A_S
   if (Katilas > config.PV_palaikoma_T || AkumuliacineV > config.PV_palaikoma_T)   {
//  Serial.println("Pamaisymo voztuvo darbas, nes Katilas > PV_palaikoma_T || AkumuliacineV > PV_palaikoma_T");    
/* ********* JEI TEMPERATŪRA KRINTA IR VOŽTUVĄ REIKIA ATIDARYTI ***************** */
if (PVoztuvas <= config.PV_palaikoma_T - config.PV_tolerancijos_T && PV_stop == true && PV_uzdarinejamas == false && millis() > PV_atidarinejimo_laikas){
PV_klaida = config.PV_palaikoma_T - config.PV_tolerancijos_T - PVoztuvas;
if (PV_klaida <= 2){
        PV_stop = false; // pasižymime, kad vožtuvas jau juda
        PV_atidarinejamas = true; // pažymima, kad vožtuvas jau atidarinėjamas
        PV_pauze = millis() + (PV_darinejimas * 100) - 2000 + (PV_klaida * 1000);
//        PV_atidarinejimo_laikas = millis() + PV_atidarinejimo_pertrauka; // atsimenamas atidarymo įjungimo laikas
//        PV_atidarytas = true; // pažymima, kad vožtuvas nebeuždarytas
        digitalWrite(PVuzdarymas, HIGH); // signalas vožtuvui atidaryti
        digitalWrite(PVatidarymas, LOW); // signalas vožtuvui atidaryti
#ifdef DEBUGpv
Serial.print("PV_klaida= ");  Serial.println(PV_klaida);
Serial.print("laikas= ");  Serial.print(millis() / 1000); Serial.println(" s.");
Serial.print("PV_atidarinejimo_laikas- ");  Serial.print((PV_darinejimas * 100 - 2000 + (PV_klaida * 1000)) / 1000); Serial.println(" s., patrumpintas");
Serial.println("Pamaisymo voztuvas, pradetas ATIDARINEJIMAS, patrumpintas"); 
#endif
        } else {
        PV_stop = false; // pasižymime, kad vožtuvas jau juda
        PV_pauze = millis() + PV_darinejimas * 100;
//        PV_atidarinejimo_laikas = millis() + PV_atidarinejimo_pertrauka; // atsimenamas atidarymo įjungimo laikas
//        PV_atidarytas = true; // pažymima, kad vožtuvas nebeuždarytas
        PV_atidarinejamas = true; // pažymima, kad vožtuvas jau atidarinėjamas
        digitalWrite(PVuzdarymas, HIGH); // signalas vožtuvui atidaryti
        digitalWrite(PVatidarymas, LOW); // signalas vožtuvui atidaryti
 }
#ifdef DEBUGpv
Serial.print("PV_klaida= ");  Serial.println(PV_klaida);
Serial.print("laikas- ");  Serial.print(millis() / 1000); Serial.println(" s.");
Serial.print("Pause prasides po- "); Serial.print(PV_darinejimas *100 / 1000); Serial.println(" s.");
Serial.println("Pamaisymo voztuvas, pradetas ATIDARINEJIMAS"); 
#endif
   }
   /* ********* P A U Z E ***************** */
// sustabdomas pamaisymo vožtuvo reguliavimas po kiekvieno atidarymo/uzdarymo
   if (PV_stop == false && millis() > PV_pauze) { 
if (PVoztuvas > config.PV_palaikoma_T + 10) {
      PV_pauze = millis(); // tiek laiko turi testis pauzė
      PV_atidarinejimo_laikas = PV_pauze; // laikotarpis, po kurio vožtvas galės pradėti atsidarinėti
      PV_uzdarinejimo_laikas = PV_pauze; // laikotarpis, po kurio vožtvas galės pradėti užsidarinėti
      PV_stop = true; // pasižymime, kad vožtuvas jau stovi
      PV_atidarinejamas = false; // pažymima, kad vožtuvas jau nebeatidarinėjamas
      PV_uzdarinejamas = false; // pažymima, kad vožtuvas jau nebeuždarinėjamas
 //       digitalWrite(PVuzdarymas, HIGH); // signalas vožtuvui atidaryti
 //       digitalWrite(PVatidarymas, HIGH); // signalas vožtuvui atidaryti
#ifdef DEBUGpv
Serial.print("laikas- ");Serial.print(millis() / 1000);Serial.println(" s.");
Serial.println("PV PAUZE, truks 0 s., PALAIKOMA TEMTERATURA PER AUKSTA");
#endif

} else {
//      PV_pauze = millis() + PV_pauzes_pertrauka; // tiek laiko turi testis pauzė
      PV_atidarinejimo_laikas = millis() + (PV_pauzes_pertrauka * 100); // tiek laiko turi testis pauzė
      PV_uzdarinejimo_laikas = millis() + (PV_pauzes_pertrauka * 100); // tiek laiko turi testis pauzė
      PV_stop = true; // pasižymime, kad vožtuvas jau stovi
      PV_atidarinejamas = false; // pažymima, kad vožtuvas jau nebeatidarinėjamas
      PV_uzdarinejamas = false; // pažymima, kad vožtuvas jau nebeuždarinėjamas
        digitalWrite(PVuzdarymas, HIGH); // signalas vožtuvui sustabdyti
        digitalWrite(PVatidarymas, HIGH); // signalas vožtuvui sustabdyti
#ifdef DEBUGpv
Serial.print("laikas- ");Serial.print(millis() / 1000);Serial.println(" s.");
Serial.print("Darinejimas prisides po- "); Serial.print(PV_pauzes_pertrauka * 100 / 1000); Serial.println(" s.");
#endif
}

   }
/* ********* JEI TEMPERATŪRA KYLA IR VOŽTUVĄ REIKIA UŽDARYTI ***************** */
   // JEI TEMPERATŪRA KYLA IR VOŽTUVĄ REIKIA UŽDARYTI
 //Serial.print("PV_palaikoma_T + PV_tolerancijos_T= ");  Serial.println(PV_palaikoma_T + PV_tolerancijos_T);  
    if (PVoztuvas >= config.PV_palaikoma_T + config.PV_tolerancijos_T && PV_stop == true && PV_atidarinejamas == false  && millis() > PV_uzdarinejimo_laikas){
      PV_klaida = PVoztuvas - config.PV_palaikoma_T - config.PV_tolerancijos_T;
      if (PV_klaida < 2){
        PV_stop = false; // pasižymime, kad vožtuvas jau juda
        PV_uzdarinejamas = true;
//        PV_uzdarinejimo_laikas = millis() + PV_uzdarinejimo_pertrauka; // atsimenamas atidarymo įjungimo laikas
        PV_pauze = millis() + (PV_darinejimas * 100) - 2000 + (PV_klaida * 1000);
        PV_atidarytas = true; // pažymima, kad vožtuvas nebeuždarytas
      digitalWrite(PVatidarymas, HIGH); // stabdomas atidarymas
      digitalWrite(PVuzdarymas, LOW); // pradedamas uždarymas
#ifdef DEBUGpv
Serial.print("PV_klaida= ");  Serial.println(PV_klaida);
Serial.print("laikas- ");  Serial.print(millis() / 1000); Serial.println(" s.");
Serial.print("Pause prasides po - ");  Serial.print((PV_darinejimas * 100 - 2000 + (PV_klaida * 1000)) / 1000); Serial.println(" s., patrumpintas");
Serial.println("Pamaisymo voztuvas, pradetas UZDARINEJIMAS, patrumpintas"); 
#endif
      }else{
        PV_stop = false; // pasižymime, kad vožtuvas jau juda
        PV_uzdarinejamas = true;
//        PV_uzdarinejimo_laikas = millis() + PV_uzdarinejimo_pertrauka; // atsimenamas atidarymo įjungimo laikas
        PV_pauze = millis() + PV_darinejimas * 100;
        PV_atidarytas = true; // pažymima, kad vožtuvas nebeuždarytas
      digitalWrite(PVatidarymas, HIGH); // stabdomas atidarymas
      digitalWrite(PVuzdarymas, LOW); // pradedamas uždarymas
#ifdef DEBUGpv
Serial.print("PV_klaida= ");  Serial.println(PV_klaida);
Serial.print("laikas- ");  Serial.print(millis() / 1000); Serial.println(" s.");
Serial.print("Pause prasides po- ");  Serial.print(PV_darinejimas * 100 / 1000); Serial.println(" s.");
Serial.println("Pamaisymo voztuvas, pradetas UZDARINEJIMAS"); 
#endif
      }

    }
   }
  }
