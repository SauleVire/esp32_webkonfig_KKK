/* 
  ESP_WebConfig 

  Copyright (c) 2015 John Lassen. All rights reserved.
  This is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Latest version: 1.1.3  - 2015-07-20
  Changed the loading of the Javascript and CCS Files, so that they will successively loaded and that only one request goes to the ESP.

  -----------------------------------------------------------------------------------------------
  History

  Version: 1.1.2  - 2015-07-17
  Added URLDECODE for some input-fields (SSID, PASSWORD...)

  Version  1.1.1 - 2015-07-12
  First initial version to the public

**************************************
WebConfig_KKK

 
  */


#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <NtpClientLib.h>
#include <WiFiClient.h>
#include <Ticker.h>
#include <WiFiUdp.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PID_v1.h>
#include <EEPROM.h>
#include <SimpleTimer.h>

#include <LiquidCrystal.h>
#include "MenuBackend.h"   
#include "definitions.h"
#include "helpers.h"
#include "global.h"
#include "ds18b20.h"

// #include "FS.h"

#include <SPI.h>
#include <TFT_eSPI.h>      // Hardware-specific library

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library
// This is the file name used to store the calibration data
// You can change this to create new calibration files.
// The SPIFFS file name must start with "/".
// #define CALIBRATION_FILE "/TouchCalData1"

// Set REPEAT_CAL to true instead of false to run calibration
// again, otherwise it will only be done once.
// Repeat calibration if you change the screen rotation.
// #define REPEAT_CAL false
  // --- definiujemy dla LCD własne znaki strzałek: dół, lewo, prawo, gora-dół i powrót ---
uint8_t arrowUpDown[8] = {0x4,0xe,0x15,0x4,0x15,0xe,0x4};
uint8_t arrowDown[8]  = {0x4,0x4,0x4,04,0x15,0xe,0x4};
uint8_t arrowRight[8] = {0x0,0x4,0x2,0x1f,0x2,0x4,0x0};
uint8_t arrowLeft[8] = {0x0,0x4,0x8,0x1f,0x8,0x4,0x0};
uint8_t arrowBack[8] = {0x1,0x1,0x5,0x9,0x1f,0x8,0x4};
uint8_t arrowUp[8]={ B00100,B01110,B11111,B00100,B00100,B00100,B00100,B00100};
    // definicja pinów dla LCD (sprawdź piny w swoim LCD)

LiquidCrystal lcd(33,32,13,12,21,19);    // definicja pinów dla LCD (sprawdź piny w swoim LCD)
/* Przykładowe Menu (podobieństwo do IDE Arduino): --------------------------------------------
// mamy 5 głównych opcji PLIK, EDYCJA, SZKIC, NARZEDZIA, POMOC
// w opcji SZKIC i NARZEDZIA mamy rozbudowane opcje dla 2 i 3 poziomu Menu
// --------------------------------------------------------------------------------------------
      NUSTATYMAI
         Pasvietimas
         Irasymas
         Numatytos reiksmesatymas
      AKUMULIACINE
         Ijungimo temp.
         Isjungimo temp.
         Rankinis vald.
      BOILERIS
         Ijungimo temp.
         Isjungimo temp.         
      SAULES KOLEKTORIUS
         Ijungimo t.skirt
         Isjungimo t.skirt
         Apsauga nuo salc
         Nuorinimas
*/
// --- tworzymy wszystkie opcje Menu: ---------------------------------------
// de facto tworzymy obiekty klasy MenuItem, które dziedziczą po klasie MenuBackend
    void menuChangeEvent(MenuChangeEvent changed);
    void menuUseEvent(MenuUseEvent used);

MenuBackend menu = MenuBackend(menuUseEvent,menuChangeEvent); // konstruktor 
   //                        ("                ")
   MenuItem P1 =  MenuItem("NUSTATYMAI        ",1);
      MenuItem P11 = MenuItem("Irasymas",2);
      MenuItem P12 = MenuItem("Numatytos reiksmes",2);
      MenuItem P13 = MenuItem("Pasvietimas",2);
      MenuItem P14 = MenuItem("Metai",2);
      MenuItem P15 = MenuItem("Menuo",2);
      MenuItem P16 = MenuItem("Diena",2);
      MenuItem P17 = MenuItem("Valanda",2);
      MenuItem P18 = MenuItem("Minute",2);
   MenuItem P2 = MenuItem("AKUMULIACINE      ",1);
      MenuItem P21 = MenuItem("AT ijungimo temp.",2);
      MenuItem P22 = MenuItem("AT isjungimo temp.",2);
      MenuItem P23 = MenuItem("AT rankinis vald.",2);
   MenuItem P3 = MenuItem("SAULES KOLEKTORIUS",1);
      MenuItem P31 = MenuItem("Ijungimo t.skirtum",2);
      MenuItem P32 = MenuItem("Isjungimo t.skirtu",2);
      MenuItem P33 = MenuItem("Apsauga nuo salcio",2);
      MenuItem P34 = MenuItem("SK rankinis vald.",2);
   MenuItem P4 = MenuItem("BOILERIS          ",1);
      MenuItem P41 = MenuItem("B ijungimo temp.",2);
      MenuItem P42 = MenuItem("B isjungimo temp.",2);
      MenuItem P43 = MenuItem("B rankinis vald.",2);
      MenuItem P44 = MenuItem("B termostatas   ",2);
   MenuItem P5 = MenuItem("PAMAISYMO VOZTUVAS",1);
      MenuItem P51 = MenuItem("Darbo Rezimas     ",2);
      MenuItem P52 = MenuItem("PV palaikoma temp.",2);
      MenuItem P53 = MenuItem("PV ijungimo temp. ",2);
      MenuItem P54 = MenuItem("PV isjungimo temp.",2);
      MenuItem P55 = MenuItem("PV palaikoma riba ",2);
      MenuItem P56 = MenuItem("PV pauze          ",2);
      MenuItem P57 = MenuItem("PV veiksmas       ",2);


/* --- Teraz pozycjonujemy  menu ( zgodnie z ustawieniem podanym powyżej) ------------
add - dodaje w pionie, addRight - dodaje w poziomie z prawej , addLeft dodaje z lewej
*/
void menuSetup()                       // funkcja klasy MenuBackend 
{
      menu.getRoot().add(P4);          // 
      P1.add(P11);
        P11.add(P12);P11.addLeft(P1);  //  
        P12.add(P13);P12.addLeft(P1);  // 
        P13.add(P14);P13.addLeft(P1);
        P14.add(P15);P14.addLeft(P1);
        P15.add(P16);P15.addLeft(P1);
        P16.add(P17);P16.addLeft(P1);
        P17.add(P18);P17.addLeft(P1);
        P18.add(P11);P18.addLeft(P1);
      menu.getRoot().add(P2);
      P1.addRight(P2);                 //
      
      P2.add(P21);                     // 
        P21.add(P22);P21.addLeft(P2);  // 
        P22.add(P23);P22.addLeft(P2);  // 
        P23.add(P21);P23.addLeft(P2);
      menu.getRoot().add(P3);
      P2.addRight(P3);                 //
      
      P3.add(P31);                     // 
        P31.add(P32);P31.addLeft(P3);  //  
        P32.add(P33);P32.addLeft(P3);
        P33.add(P34);P33.addLeft(P3);
        P34.add(P31);P34.addLeft(P3);    //        
      menu.getRoot().add(P5);
      P3.addRight(P4);
      
      P4.add(P41);
        P41.add(P42);P41.addLeft(P4);
        P42.add(P43);P42.addLeft(P4);
        P43.add(P44);P43.addLeft(P4);
        P44.add(P41);P44.addLeft(P4);
      menu.getRoot().add(P1);
      P4.addRight(P5);
      
      P5.add(P51);
        P51.add(P52);P51.addLeft(P5);
        P52.add(P53);P52.addLeft(P5);
        P53.add(P54);P53.addLeft(P5);
        P54.add(P55);P54.addLeft(P5);
        P55.add(P56);P55.addLeft(P5);
        P56.add(P57);P56.addLeft(P5);
        P57.add(P51);P57.addLeft(P5);
      menu.getRoot().add(P5);
      P5.addRight(P1);
}
// ----------- uff... nareszcie :-) -----------------------------------------------------------------------
void menuUseEvent(MenuUseEvent used)      // funkcja klasy MenuBackend - reakcja na wciśnięcie OK
                                          // tutaj właśnie oddajemy menu na rzecz akcji obsługi klawisza OK
{
   Serial.print("pasirinkta:  "); Serial.println(used.item.getName()); // do testów, potem niepotrzebne
   // --- ponizej kilka przykładów obsługi  opcji -----------
   // przykładowa reakcja na wcisnięcie klawisza OK w opcji Otworz :
/* __________________________ NUSTATYMAI Pasvietimas____________________________________ */
  if (used.item.getName() == "Pasvietimas")
  {
  lcd.setCursor(0,2);lcd.write(7);     // simbolis aukštyn/žemyn
  lcd.print("                  ");lcd.setCursor(1,2);lcd.print("Sviesumas"); // keičiamos reikšmės pavadinimas
  lcd.setCursor(13,2);lcd.print(lcd_pasvietimas);lcd.print("0% ");                        // dabartinė reikšmė
  int  veiksmas=-1;delay(1000);                                             // pagalbinis kintamasis, kontroliuojantis while ciklą
                                                                         // jei jums nereikia keisti, spauti OK po 1 sek. ir grįžti į meniu  
  while(veiksmas!=4)                   // Šis ciklas bus kartojamas, kol paspausite mygtuką OK
         {
           klaviaturos_pasikeitimas=-1; 
           veiksmas=Klaviaturos_skaitymas(Key_Pin);//delay(300);   // odczyt stanu klawiatury - funkcja Klaviaturos_skaitymas lub czytaj_2 lub czytaj_3
                                            // opis poniżej przy 3 różnych definicjach funkcji czytaj
           if(klaviaturos_pasikeitimas!=veiksmas)                    // ruszamy do pracy tylko wtedy gdy klaviaturos_pasikeitimasienił sie stan klawiatury
             {lcd.setCursor(13,2);
             if (veiksmas==1) {lcd_pasvietimas++; analogWrite(BackLight_Pin,lcd_pasvietimas*25);delay(300);}
               // jesli akcja=1 (czyli wciśnieto klawisz w górę to zwiększono temperaturę
               // ustawiono max próg i wyświetlono obecną temperaturę
             if(veiksmas==2)  {lcd_pasvietimas--;analogWrite(BackLight_Pin,lcd_pasvietimas*25);delay(300);}
            if ((lcd_pasvietimas == 255) || (lcd_pasvietimas == 0)) lcd_pasvietimas = 0;
            if (lcd_pasvietimas > 10) lcd_pasvietimas = 10;
            if (lcd_pasvietimas < 10) lcd.print(" ");
            lcd.print(lcd_pasvietimas);
            if (lcd_pasvietimas == 0) lcd.print("");
            lcd.print("0% ");

               // jesli akcja=2 (czyli wciśnieto klawisz w dół to mniejszono temperaturę
               // ustawiono min próg i wyświetlono obecną temperaturę
             if(veiksmas==4) // jeśli wciśnieto OK 
               {
                 lcd.setCursor(0,2);lcd.print(">Sviesumas OK");delay(2000); // rodome OK 2 sek.
                 lcd.setCursor(0,2);lcd.print("                    "); // czyścimy linię
                 menu.moveDown();
               //  lcd.setCursor(1,0);lcd.print(eilute1);           // odtwarzamy poprzedni stan na LCD
               }
             } 
         } klaviaturos_pasikeitimas=veiksmas;  // aktualizacja klaviaturos_pasikeitimasiennej klaviaturos_pasikeitimas, po to aby reagować tylko na klaviaturos_pasikeitimasiany stanu klawiatury
         // tu WAŻNY MOMENT - kończy się pętla while i zwracamy sterowanie do głównej pętli loop()
      } 
/* __________________________ NUSTATYMAI Irasymas____________________________________ */
     if (used.item.getName() == "Irasymas")   // dokładnie taki sam ciąg " Temperatura"
      {
                 WriteConfig();
                 lcd.setCursor(0,2);lcd.print(">Irasyta OK        ");delay(2000); // rodome OK 2 sek.
                 lcd.setCursor(0,2);lcd.print("                    "); // czyścimy linię
                 menu.moveDown();
                // lcd.setCursor(0,0);lcd.print("*");lcd.print(eilute1);           // odtwarzamy poprzedni stan na LCD
                // lcd.setCursor(19,0);lcd.print("*");

      }
/* __________________________________________________________________________ */

/* __________________________ NUSTATYMAI Irasymas____________________________________ */
     if (used.item.getName() == "Numatytos reiksmes")   // dokładnie taki sam ciąg " Temperatura"
      {
       k_skirtumas_OFF = 3; k_skirtumas_ON = 5;
       at_OFF_T = 89; at_ON_T = 90;
       b_OFF_T = 65; b_ON_T = 45;
       pv_palaikoma_T = 40;  // PV palaikoma temperatūra
       pv_ON_T = 35; pv_OFF_T = 28; // PV atidarymo temperatūra // PV atidarymo temperatūra
       pv_palaikoma_riba_T = 1.5; // Pamaišymo vožtuvo palaikomos temperatūros riba
       PV_pauzes_pertrauka = 200;
       PV_darinejimas = 60;
       WriteConfig();
                 lcd.setCursor(0,2);lcd.print("Pradines reiksmes OK");delay(2000); // rodome OK 2 sek.
                 lcd.setCursor(0,2);lcd.print("                    "); // Išvaloma eilutė
                 menu.moveDown();
                 //lcd.setCursor(0,0);lcd.print("*");lcd.print(eilute1);           // odtwarzamy poprzedni stan na LCD
                 //lcd.setCursor(19,0);lcd.print("*");

      }
/* __________________________________________________________________________ */

/* _____________________ AKUMULIACINES TALPOS SIURBLIO IJUNGIMAS ____________ */
     if (used.item.getName() == "AT ijungimo temp.")   // dokładnie taki sam ciąg " Temperatura"
     at_ON_T = MeniuFunkcija ("Nustatyta.temp=   ", at_ON_T, 99, 20, ">Temperatura OK");
 ///////////////////////////////////////////////////////////////////////////////////////////////////////
/* _____________________ AKUMULIACINES TALPOS SIURBLIO ISJUNGIMAS ____________ */
     if (used.item.getName() == "AT isjungimo temp.")   // dokładnie taki sam ciąg " Temperatura"
         at_OFF_T = MeniuFunkcija ("Nustatyta.temp=   ", at_OFF_T, 99, 20, ">Temperatura OK");
         /////////////////////////////////////////////////////////////////////////
/* _____________________ AKUMULIACINES TALPOS RANKINIS VALDYMAS ____________ */
     if (used.item.getName() == "AT rankinis vald.")   // dokładnie taki sam ciąg " Temperatura"
     {}
     /*
      {
        lcd.setCursor(0,2);lcd.print(">Isjungti");lcd.write(5);lcd.print(" Ijungti");lcd.write(1);AT_rankinis_ijungimas = false;
int  veiksmas=-1;delay(1000);         // klaviaturos_pasikeitimasienna pomocnicza, sterująca dla petli while
                                           // jesli nie puścisz klawisza OK w ciągu 1 sek. to powrót do menu    
        while(veiksmas!=4)                   // ta pętla trwa tak długo aż wciśniesz klawisz OK  
         {
           klaviaturos_pasikeitimas=-1; 
           veiksmas=Klaviaturos_skaitymas(Key_Pin); //delay(300);   // odczyt stanu klawiatury - funkcja Klaviaturos_skaitymas lub czytaj_2 lub czytaj_3
                                            // opis poniżej przy 3 różnych definicjach funkcji czytaj
           if(klaviaturos_pasikeitimas!=veiksmas)                    // ruszamy do pracy tylko wtedy gdy klaviaturos_pasikeitimasienił sie stan klawiatury
             {
             if (veiksmas==1) {AT_rankinis_ijungimas = true;  lcd.setCursor(0,2);lcd.print(">Ijungta           ");delay(200);}
             if (veiksmas==2) {AT_rankinis_ijungimas = false; lcd.setCursor(0,2);lcd.print(">Isjungta          ");delay(200);}
             if (veiksmas==4) // jeśli wciśnieto OK 
               {
                 lcd.setCursor(10,2);lcd.print("Irasyta OK");delay(2000); // rodome OK 2 sek.
                 lcd.setCursor(0,2);lcd.print("                    "); // czyścimy linię
                 AT_rankinio_ijungimo_laikas = millis();
#ifdef DEBUGakumuliacine
Serial.print("AT_rankinis_ijungimas- ");  Serial.println(AT_rankinis_ijungimas); delay(5000);
#endif
             }
             } 
         } klaviaturos_pasikeitimas=veiksmas;  // aktualizacja klaviaturos_pasikeitimasiennej klaviaturos_pasikeitimas, po to aby reagować tylko na klaviaturos_pasikeitimasiany stanu klawiatury
         
      }
 */     
/* _____________________ SAULES KOLEKTORIAUS SIURBLIO IJUNGIMAS ____________ */
     if (used.item.getName() == "Ijungimo t.skirtum")   // dokładnie taki sam ciąg " Temperatura"
              k_skirtumas_ON = MeniuFunkcija ("Nustatyta.temp=   ", k_skirtumas_ON, 25, 1, ">Temperatura OK");
     ////////////////////////////////////////////////////////////////////////////////////
/* _____________________ SAULES KOLEKTORIAUS SIURBLIO ISJUNGIMAS ____________ */
     if (used.item.getName() == "Isjungimo t.skirtu")   // dokładnie taki sam ciąg " Temperatura"
                  k_skirtumas_OFF = MeniuFunkcija ("Nustatyta.temp=   ", k_skirtumas_OFF, 24, 0, ">Temperatura OK");
                  //////////////////////////////////////////////////////////////////////////
/* _____________________ SAULES KOLEKTORIAUS APSAUGA NUO UŽŠALIMO ____________ */
     if (used.item.getName() == "Apsauga nuo salcio")   // 
      {
        lcd.setCursor(0,2);
        if (SK_apsauga_nuo_salcio == 1) lcd.println(">Ijungta           ");
        if (SK_apsauga_nuo_salcio == 2) lcd.println(">Isjungta          ");
 
int  veiksmas=-1;delay(1000);         // klaviaturos_pasikeitimasienna pomocnicza, sterujaca dla petli while
                                           // jesli nie puscisz klawisza OK w ciagu 1 sek. to powrót do menu    
        while(veiksmas!=4)                   // ta petla trwa tak dlugo az wcisniesz klawisz OK  
         {
           klaviaturos_pasikeitimas=-1; 
           veiksmas=Klaviaturos_skaitymas(Key_Pin); //delay(300);   // odczyt stanu klawiatury - funkcja Klaviaturos_skaitymas lub czytaj_2 lub czytaj_3
                                            // opis ponizej przy 3 róznych definicjach funkcji czytaj
           if(klaviaturos_pasikeitimas!=veiksmas)                    // ruszamy do pracy tylko wtedy gdy klaviaturos_pasikeitimasienil sie stan klawiatury
             {
             if (veiksmas==1) {SK_apsauga_nuo_salcio++; lcd.setCursor(0,2);if (SK_apsauga_nuo_salcio > 2) 
                                                                            {SK_apsauga_nuo_salcio = 2; 
                                                                             lcd.print(">Isjungta          ");
                                                                             delay(200);
                                                                            }
                               }
             if (veiksmas==2) {SK_apsauga_nuo_salcio--; lcd.setCursor(0,2);if (SK_apsauga_nuo_salcio < 1) 
                                                                            {SK_apsauga_nuo_salcio = 1; 
                                                                             lcd.print(">Ijungta           ");
                                                                             delay(200);
                                                                            }
                              }
             if (veiksmas==4) // jesli wcisnieto OK 
               {
                 lcd.setCursor(10,2);lcd.print("Irasyta OK");delay(2000); // rodome OK 2 sek.
                 lcd.setCursor(0,2);lcd.print("                    "); // czyscimy linie
                 menu.moveDown();
#ifdef DEBUGakumuliacine
Serial.print("Apsauga nuo salcio- ");  Serial.println(SK_apsauga_nuo_salcio); delay(5000);
#endif
             }
             } 
         } klaviaturos_pasikeitimas=veiksmas;  // aktualizacja klaviaturos_pasikeitimasiennej klaviaturos_pasikeitimas, po to aby reagowac tylko na klaviaturos_pasikeitimasiany stanu klawiatury
         
      }     

/* _____________________ BOILERIO SIURBLIO IJUNGIMAS ____________ */
     if (used.item.getName() == "B ijungimo temp.")   // dokładnie taki sam ciąg " Temperatura"
            b_ON_T = MeniuFunkcija ("Nustatyta.temp=   ", b_ON_T, 99, 20, ">Temperatura OK"); 
/////////////////////////////////////////////////////////////////////////////////////////////////////
/* _____________________ BOILERIO SIURBLIO ISJUNGIMAS ____________ */
     if (used.item.getName() == "B isjungimo temp.")   // dokładnie taki sam ciąg " Temperatura"
       b_OFF_T = MeniuFunkcija ("Nustatyta.temp=   ", b_OFF_T, 99, 25, ">Temperatura OK");     
     ////////////////////////////////////////////////////////////////////////////////////////////////
/* _____________________ BOILERIO RANKINIS VALDYMAS ____________ */
     if (used.item.getName() == "B rankinis vald.")   // dokładnie taki sam ciąg " Temperatura"
      {
        lcd.setCursor(0,2);lcd.print(">Isjungti");lcd.write(5);lcd.print(" Ijungti");lcd.write(1);B_rankinis_ijungimas = false;
int  veiksmas=-1;delay(1000);         // klaviaturos_pasikeitimasienna pomocnicza, sterująca dla petli while
                                           // jesli nie puścisz klawisza OK w ciągu 1 sek. to powrót do menu    
        while(veiksmas!=4)                   // ta pętla trwa tak długo aż wciśniesz klawisz OK  
         {
           klaviaturos_pasikeitimas=-1; 
           veiksmas=Klaviaturos_skaitymas(Key_Pin); //delay(300);   // odczyt stanu klawiatury - funkcja Klaviaturos_skaitymas lub czytaj_2 lub czytaj_3
                                            // opis poniżej przy 3 różnych definicjach funkcji czytaj
           if(klaviaturos_pasikeitimas!=veiksmas)                    // ruszamy do pracy tylko wtedy gdy klaviaturos_pasikeitimasienił sie stan klawiatury
             {
             if (veiksmas==1) {B_rankinis_ijungimas = false; lcd.setCursor(0,2);lcd.print(">Ijungta           ");delay(200);}
             if (veiksmas==2) {B_rankinis_ijungimas = true;  lcd.setCursor(0,2);lcd.print(">Isjungta          ");delay(200);}
             if (veiksmas==4) // jeśli wciśnieto OK 
               {
                 lcd.setCursor(10,2);lcd.print("Irasyta OK");delay(2000); // rodome OK 2 sek.
                 lcd.setCursor(0,2);lcd.print("                    "); // czyścimy linię
                 menu.moveDown();
                 B_rankinio_ijungimo_laikas = millis();
#ifdef DEBUGboileris
Serial.print("B_rankinis_ijungimas- ");  Serial.println(B_rankinis_ijungimas); delay(3000);
#endif
             }
             } 
         } klaviaturos_pasikeitimas=veiksmas;  // aktualizacja klaviaturos_pasikeitimasiennej klaviaturos_pasikeitimas, po to aby reagować tylko na klaviaturos_pasikeitimasiany stanu klawiatury
         
      } 
     ////////////////////////////////////////////////////////////////////////////////////////////////
/* _____________________ BOILERIO TERMOSTATAS ____________ */
     if (used.item.getName() == "B termostatas   ")   // dokładnie taki sam ciąg " Temperatura"
      {
        lcd.setCursor(0,2);lcd.print(">Isjungti");
        lcd.write(5);lcd.print(" Ijungti");lcd.write(1);
        B_termostat_status = false;    // termostato elektrinis tenas isjungtas
int  veiksmas=-1;delay(1000);         // klaviaturos_pasikeitimasienna pomocnicza, sterująca dla petli while
                                           // jesli nie puścisz klawisza OK w ciągu 1 sek. to powrót do menu    
        while(veiksmas!=4)                   // ta pętla trwa tak długo aż wciśniesz klawisz OK  
         {
           klaviaturos_pasikeitimas=-1; 
           veiksmas=Klaviaturos_skaitymas(Key_Pin); //delay(300);   // odczyt stanu klawiatury - funkcja Klaviaturos_skaitymas lub czytaj_2 lub czytaj_3
                                            // opis poniżej przy 3 różnych definicjach funkcji czytaj
           if(klaviaturos_pasikeitimas!=veiksmas)                    // ruszamy do pracy tylko wtedy gdy klaviaturos_pasikeitimasienił sie stan klawiatury
             {
             if (veiksmas==1) {B_termostat_ON = true; lcd.setCursor(0,2);lcd.print(">Ijungta           ");delay(200);} // termostato elektrinis tenas isjungtas
             if (veiksmas==2) {B_termostat_ON = false;  lcd.setCursor(0,2);lcd.print(">Isjungta          ");delay(200);} // termostato elektrinis tenas ijungtas
             if (veiksmas==4) // jeśli wciśnieto OK 
               {
                 lcd.setCursor(10,2);lcd.print("Irasyta OK");delay(2000); // rodome OK 2 sek.
                 lcd.setCursor(0,2);lcd.print("                    "); // czyścimy linię
                 menu.moveDown();
                 B_rankinio_ijungimo_laikas = millis();
#ifdef DEBUGboileris
Serial.print("B_rankinis_sildymas ");  
if (B_termostat_ON == true) Serial.println("I_J_U_N_G_T_A_S"); else Serial.println("I*S*J*U*N*G*T*A*S"); 
#endif
             }
             } 
         } klaviaturos_pasikeitimas=veiksmas;  // aktualizacja klaviaturos_pasikeitimasiennej klaviaturos_pasikeitimas, po to aby reagować tylko na klaviaturos_pasikeitimasiany stanu klawiatury
         
      } 
         
/* _____________________ PAMAIŠYMO VOŽTUVO PALAIKOMA TEMPERATURA ____________ */
///////
if (used.item.getName() == "Darbo Rezimas     ") 
 {       
        lcd.setCursor(0,0);lcd.write(7);     
        lcd.setCursor(1,1);lcd.print("Darbo Rezimas     "); 
        lcd.setCursor(8,2);
        if (PV_rezimas == 1) lcd.print("isjungta OFF");
        if (PV_rezimas == 2) lcd.print("ijungta   ON");
//        if (PV_rezimas == 3) lcd.print("isjungta    ");
        int  veiksmas=-1; delay(1000);         // 
                                           
        while(veiksmas!=4)                   // 
         {
           klaviaturos_pasikeitimas=-1; 
           veiksmas=Klaviaturos_skaitymas(Key_Pin); //delay(300);  
                                            
           if(klaviaturos_pasikeitimas!=veiksmas)           
             {
             if (veiksmas==1) {PV_rezimas++; if(PV_rezimas>2) 
                                               {PV_rezimas=2; 
                                                lcd.setCursor(8,2); 
                                                lcd.print("ijunta    ON");
                                                delay(200);
                                              }
                               }
             if(veiksmas==2)  {PV_rezimas--; if(PV_rezimas<1) 
                                               {PV_rezimas=1; 
                                                lcd.setCursor(8,2); 
                                                lcd.print("isjungta OFF");
                                               delay(200);
                                              }
                               }
             if(veiksmas==4) // 0
               {
                 lcd.setCursor(0,2); lcd.print(">Darbo Rezimas    OK"); delay(2000); // 0
                 lcd.setCursor(0,2); lcd.print("                    "); // 0
//                 lcd.setCursor(1,0);lcd.print(eilute1);           // 0
                 menu.moveDown();
               }
             } 
         } klaviaturos_pasikeitimas=veiksmas;
 }
//////
     if (used.item.getName() == "PV palaikoma temp.")   // dokladnie taki sam ciag " Temperatura"
  pv_palaikoma_T = MeniuFunkcija ("Nustatyta.temp=   ", pv_palaikoma_T, 70, 20, ">Temperatura OK");     
     //////////////////////////////////////////////////////////////////////////////
 /* _____________________ PAMAIŠYMO VOŽTUVO ĮJUNGIMO TEMPERATURA ____________ */
     if (used.item.getName() == "PV ijungimo temp. ")   // dokladnie taki sam ciag " Temperatura"
  pv_ON_T = MeniuFunkcija ("Nustatyta.temp=   ", pv_ON_T, 90, 20, ">Temperatura OK");     
     ////////////////////////////////////////////////////////////////////////////////////
/* _____________________ PAMAIŠYMO VOŽTUVO IŠJUNGIMO TEMPERATURA ____________ */
     if (used.item.getName() == "PV isjungimo temp.")   // dokladnie taki sam ciag " Temperatura"
pv_OFF_T =  MeniuFunkcija ("Nustatyta.temp=", pv_OFF_T, 90, 20, ">Temperatura OK"); 
     ///////////////////////////////////////////////////////////////////
/* _____________________ PAMAIŠYMO VOŽTUVO PALAIKOMOS TEMPERATUROS RIBA ____________ */
     if (used.item.getName() == "PV palaikoma riba ")   // dokladnie taki sam ciag " Temperatura"
      {
        lcd.setCursor(0,2);lcd.write(7);     // wyswietlamy nasz symbol strzalki góra-dól
        lcd.setCursor(1,2);lcd.print("Palaikoma riba=   "); // tekst dla uzytkownika
        lcd.setCursor(16,2);lcd.print(pv_palaikoma_riba_T);lcd.setCursor(19,2);lcd.print(" "); // wyswietlamy akt. temperature
        int  veiksmas=-1;delay(1000);         // klaviaturos_pasikeitimasienna pomocnicza, sterujaca dla petli while
                                           // jesli nie puscisz klawisza OK w ciagu 1 sek. to powrót do menu    
        while(veiksmas!=4)                   // ta petla trwa tak dlugo az wcisniesz klawisz OK  
         {
           klaviaturos_pasikeitimas=-1; 
           veiksmas=Klaviaturos_skaitymas(Key_Pin); //delay(300);   // odczyt stanu klawiatury - funkcja Klaviaturos_skaitymas lub czytaj_2 lub czytaj_3
                                            // opis ponizej przy 3 róznych definicjach funkcji czytaj
           if(klaviaturos_pasikeitimas!=veiksmas)                    // ruszamy do pracy tylko wtedy gdy klaviaturos_pasikeitimasienil sie stan klawiatury
             {
             if (veiksmas==1) {pv_palaikoma_riba_T=pv_palaikoma_riba_T+0.1;if(pv_palaikoma_riba_T>10)pv_palaikoma_riba_T=10;lcd.setCursor(16,2);
                                                 lcd.print(pv_palaikoma_riba_T);lcd.setCursor(19,2);lcd.print(" ");delay(200);}
             if(veiksmas==2)  {pv_palaikoma_riba_T=pv_palaikoma_riba_T-0.1;if(pv_palaikoma_riba_T<0.1)pv_palaikoma_riba_T=0.1;lcd.setCursor(16,2);
                                                 lcd.print(pv_palaikoma_riba_T);lcd.setCursor(19,2);lcd.print(" ");delay(200);}
             if(veiksmas==4) // jesli wcisnieto OK 
               {
                 lcd.setCursor(0,2);lcd.print(">Riba        OK");delay(2000); // rodome OK 2 sek.
                 lcd.setCursor(0,2);lcd.print("                    "); // czyscimy linie
                 menu.moveDown();
                 //lcd.setCursor(1,0);lcd.print(eilute1);           // odtwarzamy poprzedni stan na LCD
               }
             } 
         } klaviaturos_pasikeitimas=veiksmas;  // aktualizacja klaviaturos_pasikeitimasiennej klaviaturos_pasikeitimas, po to aby reagowac tylko na klaviaturos_pasikeitimasiany stanu klawiatury
         // tu WAZNY MOMENT - konczy sie petla while i zwracamy sterowanie do glównej petli loop()
      }
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* _____________________ PAMAIŠYMO VOŽTUVO PAUZĖ ____________ */
     if (used.item.getName() == "PV pauze          ")   // dokladnie taki sam ciag " Temperatura"
     PV_pauzes_pertrauka =  MeniuFunkcija ("Pauze         =   ", PV_pauzes_pertrauka, 999, 50, ">Pauze       OK"); 
     //////////////////////////////////////////////////////////////////////////////////////
/* _____________________ PAMAIŠYMO VOŽTUVO DARINEJIMAS ____________ */
     if (used.item.getName() == "PV veiksmas       ")   // dokladnie taki sam ciag " Temperatura"
//     PV_darinejimas =  MeniuFunkcija ("Veiksmas      =   ", PV_darinejimas, PV_pauzes_pertrauka-20, 30, ">Veiksmas    OK"); 
     PV_darinejimas =  MeniuFunkcija ("Veiksmas      =   ", PV_darinejimas, 999, 30, ">Veiksmas    OK"); 
          //////////////////////////////////////////////////////////////////////////////////////////
 // ...
}
// --- Reakcja na wciśnięcie klawisza -----------------------------------------------------------------
void menuChangeEvent(MenuChangeEvent changed)  // funkcja klasy MenuBackend 
{
  if(changed.to.getName()==menu.getRoot()){InMenu =false;
                                           Serial.println("Dabar esame MenuRoot");
                                           LCD_T_sablonas();
                                           Temperaturu_vaizdavimas();}
  /* tak naprawdę to tylko tutaj przydaje się ów shortkey i służy przede wszystkim do wzbogacenia menu
     o symbole strzałek w zależności co wybrano. Wszystko co tutaj się wyprawia jest pokazywane na LCD. 
  */
  int c=changed.to.getShortkey();                         // pobieramy shortkey (1,2,3, lub4)
  lcd.setCursor(0,0); 
  if(c==1)                                                // jeśli to menu głowne (shortkey=1) to:
    {InMenu =true;
    lcd.write(3);                                         // strzałka w lewo
    strcpy(eilute1,changed.to.getName());                  // tworzymy napis w pierwszej linii
    lcd.print(eilute1);                                    // wyświetlamy ją
    lcd.setCursor(19,0);lcd.write(4);                     // strzałka w prawo
    lcd.setCursor(0,1);lcd.write(5); lcd.print("                  ");   
    lcd.setCursor(19,1);lcd.write(5);                     // strzałka w dół
    lcd.setCursor(0,2);lcd.print("                    "); // išvaloma 3 eilutė, kai atsiduriama meniu nr.1
    }
    if(c==2)                                              // jeśli to podmenu dla dziecka - (shortkey=2) to:
    {InMenu =true;
    lcd.print("*");                                       // rysujemy gwiazdkę
    strcpy(eilute2,changed.to.getName());                  // tworzymy napis w pierwszej linii
    lcd.print(eilute1);                                    // wyświetlamy ją
    lcd.setCursor(19,0);lcd.print("*");                   // gwiazdka 
    lcd.setCursor(0,1);lcd.write(6); lcd.print("                  ");                     // druga eilute i strzałka powrotu (arrowBack)
    lcd.setCursor(1,1);lcd.print(changed.to.getName());                      // wyświetlamy nazwe "dziecka"
    lcd.setCursor(19,1);lcd.write(7);                     // strzałka góra-dół
    }
    
    if(c==3)                                              // jeśli dziecko  ma dziecko - (shortkey =3) to:
    {InMenu =true;
    lcd.print("*");                                       // gwiazdka
    strcpy(eilute2,changed.to.getName());                  // kopiujemy akt. nazwe opcji menu do klaviaturos_pasikeitimasiennej eilute2
    lcd.print(eilute1);                                    // i wyświetlamy pierwszą linię
    lcd.setCursor(19,0);lcd.print("*");                   // gwiazdka
    lcd.setCursor(0,1);lcd.write(6);                      // druga eilute i strzałka arrowBack
    lcd.print(changed.to.getName());                      // wyświetlamy wnuka w drugiej linii
    lcd.setCursor(19,1);lcd.write(4);                     // strzałka w prawo bo są wnuki
    }
    
    if(c==4)                                              // jeśli to wnuk  (shortkey =4) to:
    {InMenu =true;
    lcd.print("*");                                       // gwaizdka
    lcd.print(eilute2);                                    // w pierwszej linii wyświetlamy dziecko ( czyli rodzica wnuka) 
    lcd.setCursor(19,0);lcd.print("*");                   // gwaizdka
    lcd.setCursor(0,1);lcd.write(6);                      // druga eilute i strzałka arrowBack
    lcd.print(changed.to.getName());                      // wyświetlamy wnuka
    lcd.setCursor(19,1);lcd.write(7);                     // strzałka góra-dół 
    } 
}

// --- analoginė 5 mygtukų klaviatūros nuskaitymo versija, DFRobot --------------------------------------
volatile int Klaviaturos_skaitymas(int analog)
{
   int stan_Analog = analogRead(analog);delay(30);//Serial.println(stan_Analog); 
   if (stan_Analog > 1000) return -1; // riba
   if (stan_Analog < 50)   return 0;  // Į dešinę 
   if (stan_Analog < 200)  return 1;  // Viršun 
   if (stan_Analog < 400)  return 2;  // Žemyn‚ 
   if (stan_Analog < 600)  return 3;  // Į kairę  
   if (stan_Analog < 800)  return 4;  // OK 
   return -1;                         // Nepaspaustas
}
// ===================MenBackend pabaiga=========================================================================
//

/*
Include the HTML, STYLE and Script "Pages"
*/
#include "Page_Root.h"
#include "Page_Admin.h"
#include "Page_Script.js.h"
#include "Page_Style.css.h"
#include "Page_NTPsettings.h"
#include "Page_Information.h"
#include "Page_General.h"
#include "PAGE_NetworkConfiguration.h"
#include "Page_NotFound.h"
#include "Page_Kolektorius.h"
#include "Page_Boileris.h"
#include "Page_Akumuliacine.h"
#include "Page_Emoncms.h"
#include "Page_Index.h"
#include "Page_DS18B20.h"
#include "Page_PriskirtiDS18B20.h"
#include "Page_Pvoztuvas.h"

#define Diagnostika 1 // Naudojama tik testavimui
#define ACCESS_POINT_NAME  "SauleVire_PT"				
#define ACCESS_POINT_PASSWORD  "" 
#define AdminTimeOut 300  // Defines the Time in Seconds, when the Admin-Mode will be disabled
SimpleTimer timer;
const char* host = "Vire";
// ============================================================================================
//
/////////////////////////////////////////////////////////////////////////
void setup ( void ) {
  // Initialise the TFT screen
  tft.init();

  // Set the rotation before we calibrate
  tft.setRotation(3);

  // Calibrate the touch screen and retrieve the scaling factors
//  touch_calibrate();

  // Clear the screen
  tft.fillScreen(TFT_BLACK);
    //Display a simple splash screen
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_YELLOW);
  tft.setCursor(1, 1);
  tft.print(F("SauleVire.lt"));
  EEPROM.begin(512);

#ifdef Diagnostika
	Serial.begin(115200);
	Serial.println("Čia SauleVire.lt pradžia\n");
#endif
	if (!ReadConfig())
	{
		// DEFAULT CONFIG
		config.ssid = "SauleVire"; //belaidžio tinklo pavadinimas
		config.password = "SauleVire.lt"; //slaptažodis
		config.dhcp = true;
    config.IP[0] = 192;config.IP[1] = 168;config.IP[2] = 2;config.IP[3] = 111;
    config.DNS[0] = 1;config.DNS[1] = 1;config.DNS[2] = 1;config.DNS[3] = 1;
   	config.Netmask[0] = 255;config.Netmask[1] = 255;config.Netmask[2] = 255;config.Netmask[3] = 0;
		config.Gateway[0] = 192;config.Gateway[1] = 168;config.Gateway[2] = 2;config.Gateway[3] = 1;
		config.ntpServerName = "pool.ntp.org";
		config.Update_Time_Via_NTP_Every =  30;
		config.timezone = 20;
		config.daylight = true;
		config.DeviceName = "Vire.lt";
		config.AutoTurnOff = false;
		config.AutoTurnOn = false;
		config.TurnOffHour = 0;
		config.TurnOffMinute = 0;
		config.TurnOnHour = 0;
		config.TurnOnMinute = 0;
/* ********** kintamieji saulės kolektoriui ******************* */
    config.k_skirtumas = 4;
    config.k_uzsalimas = true; // 1-įjungta, 0- išjungta , SK apsauga nuo šalčio, pašildymas
    config.k_nuorinimas = false; //  SK siurblio rankiniam valdymui (nuorinimas)
    config.k_intervalas = 35; // Numatytas laikas saulės kolektoriaus temperatūros matavimui 10s.

    config.reiksme1 = "a";
    config.reiksme2 = "b";
    config.reiksme3 = "c";
    config.katalogas = "d";
    config.emoncmsOn = false;
/* ********** kintamieji Boileriui ******************* */
    config.Bo_ON_T = 45; // temperatūra boilerio siurbliui įjungti
    config.Bo_OFF_T = 65; // temperatūra boilerio siurbliui įšjungti
    config.Bo_Rankinis_ijungimas = false; // Žymė rankiniam AT siurblio valdymui
    config.Bo_Termostatas_ON = false; // Žymė rankiniam termostato įjungimui
    config.Bo_Termostato_busena = false; // Žymė termostato busenai
/* ********** kintamieji Akumuliacinei talpai ******************* */
    config.At_ON_T = 90; // temperatūra akumuliacines talpos siurbliui įjungti
    config.At_OFF_T = 89; // temperatūra akumuliacines talpos siurbliui įšjungti
    config.At_Rankinis_ijungimas = 0; // Žymė rankiniam AT siurblio valdymui
/* ********** Pamaišymo vožtuvo nustatymai ************************************ */   
    config.PV_ON_T = 40; // temperatūra radijatorių siurbliui įjungti
    config.PV_OFF_T = 35; // temperatūra radijatorių siurbliui įšjungti
    config.PV_palaikoma_T = 40; // Į radiatorius tiekiamo termofikato temperatūra 
    config.PV_tolerancijos_T = 1.2; // Į radiatorius tiekiamo termofikato tolerancijos temperatūra 
    config.PV_rankinis_ijungimas = false; // Žymė rankiniam radijatorių siurblio valdymui
/* ********** PID nustatymai ************************************ */    
    config.Kp = 25;
    config.Ki = 1.5;
    config.Kd = 4;
    config.WindowSize = 160;
		WriteConfig();
#ifdef Diagnostika    
		Serial.println("General config applied");
#endif   
	}
	
	
	if (AdminEnabled)
	{
		WiFi.mode(WIFI_AP_STA);
		WiFi.softAP( ACCESS_POINT_NAME , ACCESS_POINT_PASSWORD);
	}
	else
	{
		WiFi.mode(WIFI_STA);
	}

	ConfigureWifi();
	

	server.on ( "/", processIndex  );
	server.on ( "/admin/filldynamicdata", filldynamicdata );
	server.on ( "/favicon.ico",   []() { 
#ifdef Diagnostika	  
	  Serial.println("favicon.ico"); 
#endif   
	  server.send ( 200, "text/html", "" );   }  );
	server.on ( "/admin.html", []() {
#ifdef Diagnostika   
	  Serial.println("admin.html"); 
#endif 
	  server.send ( 200, "text/html", PAGE_AdminMainPage );   }  );
	server.on ( "/config.html", send_network_configuration_html );
  server.on ( "/info.html", []() { 
#ifdef Diagnostika   
  Serial.println("info.html"); 
#endif  
  server.send ( 200, "text/html", PAGE_Information );   }  );
	server.on ( "/ntp.html", send_NTP_configuration_html  );
	server.on ( "/general.html", send_general_html  );
  server.on ( "/example.html", []() {
#ifdef Diagnostika    
    Serial.println("example.html"); 
#endif
    server.send ( 200, "text/html", PAGE_EXAMPLE );  } );
  server.on ( "/kolektorius.html", send_KolektoriausKonfiguracija_html ); 
  server.on ( "/boileris.html", send_BoilerioKonfiguracija_html ); 
  server.on ( "/akumuliacine.html", send_AkumuliacinesKonfiguracija_html ); 
  server.on ( "/pvoztuvas.html", send_PVoztuvoKonfiguracija_html ); 
  server.on ( "/emoncms.html", send_Emoncms_html ); 
  server.on ( "/ds18b20.html", Page_DS18B20 ); 
  server.on ( "/priskirtiDS18b20.html", send_PriskirtiDS18B20_html );
  server.on ( "/naujinimas.html", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex);
  });
  server.on("/serverIndex", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });
  /*handling uploading firmware file */
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
  server.on ( "/style.css", []() { 
#ifdef Diagnostika
    Serial.println("style.css"); 
#endif
    server.send ( 200, "text/plain", PAGE_Style_css );  } );
	server.on ( "/microajax.js", []() { 
#ifdef Diagnostika
	  Serial.println("microajax.js"); 
#endif
	  server.send ( 200, "text/plain", PAGE_microajax_js );  } );
	server.on ( "/admin/values", send_network_configuration_values_html );
	server.on ( "/admin/connectionstate", send_connection_state_values_html );
	server.on ( "/admin/infovalues", send_information_values_html );
  server.on ( "/admin/ntpvalues", send_NTP_configuration_values_html );
  server.on ( "/admin/kolektoriusvalues", send_KolektoriausKonfiguracija_values_html );
  server.on ( "/admin/pvoztuvovalues", send_PVoztuvoKonfiguracija_values_html );
  server.on ( "/admin/BoilerisValues", send_BoilerioKonfiguracija_values_html );
  server.on ( "/admin/AkumuliacineValues", send_AkumuliacinesKonfiguracija_values_html );
  server.on ( "/admin/emoncmsvalues", send_Emoncms_values_html );
  server.on ( "/admin/priskirtids18b20values", send_PriskirtiDS18B20_values_html );
	server.on ( "/admin/generalvalues", send_general_configuration_values_html);
	server.on ( "/admin/devicename", send_devicename_value_html);

	server.onNotFound ( []() { 
#ifdef Diagnostika
	  Serial.println("Page Not Found"); 
#endif
	  server.send ( 200, "text/html", PAGE_NotFound );   
	  }  );
	server.begin();


	tkSecond.attach(1,Second_Tick);
	UDPNTPClient.begin(2390);  // Port for NTP receive

//  Setup DS18b20 temperature sensor

  SetupDS18B20();
Setpoint = Boileris + config.k_skirtumas;
  //tell the PID to range between 0 and the full window size
  myPID.SetOutputLimits(0, config.WindowSize);

  //turn the PID on
  myPID.SetMode(AUTOMATIC);
//  timer.setInterval(15000L, KolektoriusT);

  pinMode(RELAYPIN,OUTPUT);

}

 
void loop ( void ) {

	if (AdminEnabled) {
		if (AdminTimeOutCounter > AdminTimeOut) {
			 AdminEnabled = false;
			 Serial.println("\n\nAdmin Mode disabled!\n\n");
//     WiFi.reconnect();
       WiFi.mode(WIFI_STA);
       Serial.println(WiFi.localIP().toString());
       WiFi.begin(config.ssid, config.password);
      ConfigureWifi();
		             }	}
	if (config.Update_Time_Via_NTP_Every  > 0 ) {
		if (cNTP_Update > 5 && firstStart)
		{	NTPRefresh();
			cNTP_Update =0;
			firstStart = false;
		}
		else if ( cNTP_Update > (config.Update_Time_Via_NTP_Every * 60) )
		{	NTPRefresh();
			cNTP_Update =0;
		}
	}
	if(DateTime.minute != Minute_Old)
	{	 Minute_Old = DateTime.minute;
		 if (config.AutoTurnOn)
		 { if (DateTime.hour == config.TurnOnHour && DateTime.minute == config.TurnOnMinute)
			 { Serial.println("SwitchON"); Laikmatis = true;}
		 }
		 Minute_Old = DateTime.minute;
		 if (config.AutoTurnOff)
		 { if (DateTime.hour == config.TurnOffHour && DateTime.minute == config.TurnOffMinute)
			 {Serial.println("SwitchOff"); Laikmatis = false;}
		 }
	}
	server.handleClient();
///////////// *   START Your Code here    * //////////////////////
/****************************************************************/


// Taimeris nustato laiko intervalus temperatūrų matavimui
  unsigned long currentMillis = millis();
  unsigned long currentMillis1 = millis();

// tikrinama ar jau laikas matuoti temperatūrą
if (config.k_intervalas < 3) config.k_intervalas = 3;
  if ((unsigned long)(currentMillis - previousMillis) >= config.k_intervalas * 1000)
  { // įsimenamas paskutinio matavimo laikas
    previousMillis = currentMillis;

    TemteraturosMatavimas();
// Jei įjungtas nuorinimo režimas arba apsauga nuo užšalimo ir kolektoriaus temperatūra artėja prie 0, įjungiamas siurblys
    if (config.k_nuorinimas == 1 or ((Kolektorius < 0.68) & (config.k_uzsalimas == 1)))
          { digitalWrite(RELAYPIN, HIGH); RelayState = "Įjungtas";
                      if (millis() - previousMillis2 >= 20000)
  { // įsimenamas paskutinio matavimo laikas
    previousMillis2 = millis();
       Serial.print("\nSiurblio rele įjungta ON (Nuorinimas, užšalimas)\n");
  }
       } else {
//Jei laikas sutampa su laiku, kai kolektoriaus šiluma niekinė, siurblys išjungiamas
        if (DateTime.hour == config.TurnOffHour or DateTime.hour == config.TurnOffHour +1 )
          { digitalWrite(RELAYPIN, LOW); RelayState = "Išjungtas(laikas)";
            if (millis() - previousMillis2 >= 20000)
  { // įsimenamas paskutinio matavimo laikas
    previousMillis2 = millis();
       Serial.print("\nSiurblio rele įjungta OFF (nurodytas išjungimo laikas)\n");
  }     
       } else {Siurblys();}
       }
    
}
/* ****************************** emoncms ****************************** */
//ar aktyvuotas duomenų siuntimas į emoncms ir jau galima siųsti duomenis
    if ((config.emoncmsOn  == 1) & ((unsigned long)(currentMillis1 - previousMillis1) >= config.intervalasEmon * 1000)) 
    {
      // įsimenamas paskutinio matavimo laikas
      previousMillis1 = currentMillis1;
      emoncms();
//      postEmoncms();
    }



#ifdef Diagnostika
// tikrinama ar jau laikas matuoti temperatūrą
  if (millis() - previousMillis2 >= 20000)
  { // įsimenamas paskutinio matavimo laikas
    previousMillis2 = millis();
    Serial.print("\nemoncmsSrv - "); Serial.print(config.emoncmsSrv);
    Serial.print("\nemoncmsOn - "); Serial.print(config.emoncmsOn);
    Serial.print(", k_uzsalimas - "); Serial.print(config.k_uzsalimas);
    Serial.print("\nK- "); Serial.print(Kolektorius);
    Serial.print(", B- "); Serial.print(Boileris);
    Serial.print(", O- "); Serial.print(OrasL);
Serial.print("\nIP- "); Serial.print(WiFi.localIP());
//  tft.fillScreen(TFT_BLACK);
//  tft.setTextSize(2);
//  tft.setTextColor(TFT_YELLOW);
//  tft.setCursor(1, 1);
//  tft.println(F("SauleVire.lt"));
yield();tft.setCursor(1, 25); tft.print(F("IP- "));tft.setCursor(37, 25); tft.print(WiFi.localIP());
Serial.print("\nDNS- "); Serial.print(WiFi.dnsIP());
Serial.print("\nKaukė- "); Serial.print(WiFi.subnetMask());
Serial.print("\nVartai- "); Serial.print(WiFi.gatewayIP());    
Serial.printf("\nFreeMem: %d \nDabar- %d:%d:%d %d.%d.%d \n",ESP.getFreeHeap(), DateTime.year, DateTime.month, DateTime.day, DateTime.hour, DateTime.minute, DateTime.second);
//yield();tft.setCursor(1, 50); tft.print(DateTime.second);
tft.fillRect(1,50,60,18,TFT_BLACK);
yield();tft.drawNumber(DateTime.minute, 1, 50);tft.print(F(" : "));tft.drawNumber(DateTime.second, 33, 50);
 /* 
  if(DS18B20.getTempC(devAddr[config.Kid]) == DEVICE_DISCONNECTED_C) {    
    Serial.println("Klaida! Ds18B20 Kolektorius rodmenys neteisingi");
    return;                                }
  if(DS18B20.getTempC(devAddr[config.Bid]) == DEVICE_DISCONNECTED_C) {    
    Serial.println("Klaida! Ds18B20 Boileris rodmenys neteisingi");
    return;                                }
  if(DS18B20.getTempC(devAddr[config.OLid]) == DEVICE_DISCONNECTED_C) {    
    Serial.println("Klaida! Ds18B20 OrasL rodmenys neteisingi");
    return;                                }
  if(DS18B20.getTempC(devAddr[config.OKid]) == DEVICE_DISCONNECTED_C) {    
    Serial.println("Klaida! Ds18B20 OrasK rodmenys neteisingi");
    return;                                }
  if(DS18B20.getTempC(devAddr[config.AVid]) == DEVICE_DISCONNECTED_C) {    
    Serial.println("Klaida! Ds18B20 AkumuliacineV rodmenys neteisingi");
    return;                                }
  if(DS18B20.getTempC(devAddr[config.AAid]) == DEVICE_DISCONNECTED_C) {    
    Serial.println("Klaida! Ds18B20 AkumuliacineA rodmenys neteisingi");
    return;                                }
  if(DS18B20.getTempC(devAddr[config.PVid]) == DEVICE_DISCONNECTED_C) {    
    Serial.println("Klaida! Ds18B20 PVoztuvas rodmenys neteisingi");
    return;                                }
  if(DS18B20.getTempC(devAddr[config.KKKid]) == DEVICE_DISCONNECTED_C) {    
    Serial.println("Klaida! Ds18B20 Katilas rodmenys neteisingi");
    return;                                }
*/
  }
#endif
//  if (WiFi.mode(WIFI_STA))
//    machineIOs.SetLeds(noChange, noChange, (((millis() / 125) & 7) == 0) ? On : Off); // 1 Hz blink with 12.5% duty cycle
//  else
//    machineIOs.SetLeds(noChange, noChange, (((millis() / 125) & 7) != 0) ? On : Off); // 1 Hz blink with 87.5% duty cycle
//  machine.ModulateSound(((millis() / 63) & 7) == 0); // 2 Hz blink with 12.5% duty cycle (1.984... Hz)
/****************************************************************/

//  MDNS.update();
//  timer.run();
///////////// *    Your Code here END   * //////////////////////
	
	if (Refresh)  
	{	Refresh = false;
//		Serial.println("Refreshing...");
//		Serial.printf("FreeMem:%d %d:%d:%d %d.%d.%d \n",ESP.getFreeHeap() , DateTime.hour,DateTime.minute, DateTime.second, DateTime.year, DateTime.month, DateTime.day);
	}
}
//------------------------------------------------------------------------------------------
