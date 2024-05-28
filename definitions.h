
/* ************** LKLAVIATUROS KINTAMIEJI ************************************* */
#define Key_Pin 39 //Analogines klaviaturos pajungimo i6vadas A3
volatile int klaviaturos_pasikeitimas =-1;               // Tikrinti ar pasikeitė klaviatūros reikšmė
volatile int x=-1;                 // 
volatile int stan_Analog;      
char *eilute1;                      // pierwsza linia wyświetlanego tekstu na LCD
char *eilute2;                      // druga linia wyświetlanego tekstu na LCD
char *eilute3;                      // pierwsza linia wyświetlanego tekstu na LCD
char *eilute4;                      // druga linia wyświetlanego tekstu na LCD
char *eilute5;                      // druga linia wyświetlanego tekstu na LCD
boolean InMenu = false;
/* ********** LCD aprasymas ******************* */
#define BackLight_Pin 2 //LCD pasvietimo isvadas (standart LCD KeeyPad use pin 10)
byte lcd_pasvietimas = 10; // lcd_pasvietimas * 25 = MAX LCD apsviestumas
boolean Backlighting = true; // zyme ekrano pasvietimui

unsigned long Ekrano_pasvietimo_ijungimo_laikas;
unsigned long  Ekrano_pasvietimo_pertrauka = 600000;
unsigned long Ekrano_rodmenu_atnaujinimo_laikas = 0;
#define Ekrano_rodmenu_atnaujinimo_pertrauka 5000

float KI,SK,PV,AV,AA,KT,BV,BA,LT; 
/* ********** kintamieji saulės kolektoriui ******************* */
byte k_skirtumas_ON = 5;
byte k_skirtumas_OFF = 3;
//boolean k_uzsalimas = false;
boolean SK_rankinis_ijungimas = false; // Žymė rankiniam SK siurblio valdymui (nuorinimas)
byte SK_apsauga_nuo_salcio = 1; // 1-įjungta, 2- išjungta ,Žymė SK apsaugai nuo šalčio
/* ********** kintamieji Akumuliacinei talpai ******************* */
byte at_ON_T = 90; // temperatūra akumuliacines talpos siurbliui įjungti
byte at_OFF_T = 89; // temperatūra akumuliacines talpos siurbliui įšjungti
byte AT_rankinis_ijungimas = 0; // Žymė rankiniam AT siurblio valdymui
/* ********************************************************************** */
/* ********** kintamieji Boileriui ******************* */
byte b_ON_T = 45; // temperatūra boilerio siurbliui įjungti
byte b_OFF_T = 65; // temperatūra boilerio siurbliui įšjungti
boolean B_rankinis_ijungimas = false; // Žymė rankiniam AT siurblio valdymui
boolean B_termostat_ON = false; // Žymė rankiniam termostato įjungimui
boolean B_termostat_status = false; // Žymė termostato busenai
/* ************************************************************************ */
/* ********** kintamieji Pamaišymo vožtuvui ******************* */
byte pv_palaikoma_T = 40; // PV palaikoma temperatūra 
float pv_palaikoma_riba_T = 1.5; // PV palaikomos temperatūros riba 
byte pv_ON_T = 35; // PV įjungimo temperatūra 
byte pv_OFF_T = 28; // PV išjungimo temperatūra 
byte PV_rankinis_ijungimas = 0; // Žymė rankiniam PV valdymui
float pv_klaida;
//boolean PV_atidarytas = false;
//boolean PV_uzdarytas = true;
//boolean PV_atidarinejamas = false;
//boolean PV_uzdarinejamas = false;
//boolean PV_pusiausvyra = false;
//boolean PV_stop = true;
byte PV_rezimas = 2; // 1- rankinis, 2- automatinis
/* ********************************************************************** */
unsigned long B_rankinio_ijungimo_laikas =0;
unsigned long B_rankinio_ijungimo_trukme = 300000;
