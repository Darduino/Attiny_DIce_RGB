/* 
--------------------------------------------------------------------------------
Charliplexing 7 LEDs RGB common Cathode
dice description : 
  1      5
  2  4   6
  3      7
--------------------------------------------------------------------------------
Row 1 (R1): Arduino Pin 07 or ATTINY Pin 1
Row 2 (R2): Arduino Pin 08 or ATTINY Pin 2
Row 3 (R3): Arduino Pin 09 or ATTINY Pin 3
Row 4 (R4): Arduino Pin 10 or ATTINY Pin 4
Row 5 (R4): Arduino Pin 11 or ATTINY Pin 5
Row 6 (R4): Arduino Pin 12 or ATTINY Pin 6
Row 7 (R4): Arduino Pin 13 or ATTINY Pin 7
Button : Arduino Pin 02 or ATTINY Pin 4

See pin_switch var Arduino vs Attiny
---------------------------------------------------------------------------------
*/

//#define ATTINY_MODE 1    // Comment for ARDUINO
//#define TEST_MODE 1      // Comment for production version

//#define ATTINYfd_1   1     //Attiny à 1 mhz
//#define ATTINYfd_8   1     //Attiny à 8 mhz
//#define ATTINYfd_20  1    //Attiny à 20 mhz

//-------------Library-----------------------------------------------------------
#include <EEPROM.h>         // save data for random
#include <Entropy.h>        // GNU random
#include <avr/wdt.h>        // watchdog interrupt


//-------------Const Variables----------------------------------------------------
#if !defined (ATTINY_MODE) //ARDUINO
  const int temps_affiche =   500; //display refresh rate = 500 for arduino 150 for ATTINY
  const int temps_une_led =   400; // display repet light per led = 30 for arduino and 5 for ATTINY
  const int pin_switch =      6;   // match pin between ATTINY and arduino 
#endif
#if defined(ATTINYfd_1)
  const int temps_affiche =   150; //display refresh rate = 500 for arduino 150 for ATTINY
  const int temps_une_led =   24;  // display repet light per led = 30 for arduino and 5 for ATTINY
  const int pin_switch =      0;   // match pin between ATTINY and arduino
#endif
#if defined(ATTINYfd_8)
  const int temps_affiche =   300; //display refresh rate = 500 for arduino 150 for ATTINY
  const int temps_une_led =   100; // display repet light per led = 30 for arduino and 5 for ATTINY
  const int pin_switch =      0;   // match pin between ATTINY and arduino
#endif
//-------------Const Variables----------------------------------------------------
const int Pin1 = 1;           // Define const for PIN1
const int Pin2 = 2;           // Define const for PIN2
const int Pin3 = 3;           // Define const for PIN3
const int Pin4 = 4;           // Define const for PIN4
const int Pin5 = 5;           // Define const for PIN5
const int Pin6 = 6;           // Define const for PIN6
const int Pin7 = 7;           // Define const for PIN6

const int PinButton = 2;      // Define const for Button PIN

const int redPin    = 1;      // Define const for color RED
const int greenPin  = 2;      // Define const for color GREEN
const int bluePin   = 3;      // Define const for color BLUE

const int coef      =30;      // Define time dipslay

int MAXPlayer       = 6;
volatile int NBPlayer =2;
volatile unsigned long button_time = 0;  
volatile unsigned long last_button_time = 0; 
volatile byte increment;
int time_fade =150;



//-------------TABCOLOR Mapping pin led with color fading------------------------
int tabcolor[6][3]={
  {redPin,bluePin,greenPin}, //redtoyellow
  {greenPin,bluePin,redPin}, //yellowtogreen
  {greenPin,redPin,bluePin}, //greentocyan
  {bluePin,redPin,greenPin}, //cyantoblue
  {bluePin,greenPin,redPin}, //bluetomagenta
  {redPin,greenPin,bluePin}, //magentatored
 };


// red 1, green 2, blue 3, white 4, magenta 5, yellow 6, cyan 7
int colorplayer[8] ={1,2,3,4,5,6,7};

// Mapping pin with leds
int tabledRGB[7][4]={
//Red, GND, Green, Blue  
{Pin1, Pin2, Pin3, Pin4}, // LED 1
{Pin2, Pin3, Pin4, Pin1}, // LED 2
{Pin3, Pin4, Pin1, Pin2}, // LED 3
{Pin4, Pin1, Pin2, Pin3}, // LED 4
{Pin6, Pin5, Pin7, Pin4}, // LED 5
{Pin7, Pin6, Pin5, Pin4}, // LED 6
{Pin6, Pin7, Pin4, Pin5}, // LED 7
};


// dice number To led matrix
byte dede[8][7] ={
 //3 4 5 6 7 8 9
  {0,0,0,0,0,0,0}, // 0 nothing
  {0,0,0,1,0,0,0}, // 1
  {0,0,1,0,1,0,0}, // 2
  {1,0,0,1,0,0,1}, // 3
  {1,0,1,0,1,0,1}, // 4
  {1,0,1,1,1,0,1}, // 5
  {1,1,1,0,1,1,1}, // 6
  {1,1,1,1,1,1,1}, // 7 all
}; 



void software_Reboot()
{
  wdt_enable(WDTO_15MS);
  while(1)
  {
  }
}

void displaynbplayer()
{
  button_time = millis();

if (button_time - last_button_time > 3000)  afficheledRGB(0,temps_affiche*coef,1);
  //check to see if increment() was called in the last 250 milliseconds
  if (button_time - last_button_time > 250)
  {
      last_button_time = button_time;
      if (NBPlayer == MAXPlayer ) increment= 0;
      if (NBPlayer == 1) increment = 1;
      if (increment==1) NBPlayer ++;
      else NBPlayer --;
       EEPROM.write(10, NBPlayer);
       EEPROM.write(12,increment);

      software_Reboot();
  }
}

void setup() 
{
    uint8_t random_byte;
    unsigned int id = EEPROM.read(0);
    
    NBPlayer=EEPROM.read(10);
    increment=EEPROM.read(12);

   Entropy.initialize();

  if (Entropy.available()) {
    randomSeed(id);
    id = Entropy.random();
  }
    EEPROM.write(0, id);
  
#if !defined (ATTINY_MODE)    
    Serial.begin (115200);
    Serial.println ("setup");
    Serial.println ("id : ");
    Serial.print (id+1);
    Serial.println("");
#endif 

  pinMode(PinButton, INPUT);
  
  
  attachInterrupt(0, displaynbplayer, CHANGE);
  

}

void loop()
{
  int i,j;
  
  affichedeRGB(NBPlayer, 1000,1,0);
 #if !defined (TEST_MODE)
  Lancedes(NBPlayer);
#endif
#if defined (TEST_MODE)
    afficheledRGB(0,temps_affiche*coef,1);
    afficheledRGB(1,temps_affiche*coef,2);
    afficheledRGB(2,temps_affiche*coef,3);
    afficheledRGB(3,temps_affiche*coef,4);
    afficheledRGB(4,temps_affiche*coef,5);
    afficheledRGB(5,temps_affiche*coef,6);
    afficheledRGB(6,temps_affiche*coef,7);
    affichedeRGB(1, temps_affiche,1,1);
    affichedeRGB(2, temps_affiche,2,1);
    affichedeRGB(3, temps_affiche,3,1);
    affichedeRGB(4, temps_affiche,4,1);
    affichedeRGB(5, temps_affiche,5,1);
    affichedeRGB(6, temps_affiche,6,1);

#endif
  
  for (j=0;j<7;j++) for (i=0;i<6;i++) changeColor(i,j);

}

void afficheledRGB (int numled, int time, int coul)
{
  int i,j;
  int maxi = 10000;
  int time2;
  
     int but=0;
 

  // close all pins
  pinMode (pin_switch +Pin1, INPUT);
  pinMode (pin_switch +Pin2, INPUT);
  pinMode (pin_switch +Pin3, INPUT);
  pinMode (pin_switch +Pin4, INPUT);
  pinMode (pin_switch +Pin5, INPUT);
  pinMode (pin_switch +Pin6, INPUT);
  pinMode (pin_switch +Pin7, INPUT);
  
  time2=time;
  if (coul ==4) time2= time/2;
  if (coul==5 || coul==6 || coul ==7) time2=time/1,6;
  
  // Pin GND
  pinMode(pin_switch +tabledRGB [numled][1],OUTPUT);
  digitalWrite(pin_switch +tabledRGB [numled][1], LOW);


    // PinMode for color
    switch (coul) {
    case 1 : // rouge
      pinMode(pin_switch +tabledRGB [numled][0], OUTPUT);
      pinMode (pin_switch +tabledRGB [numled][2], INPUT);
      pinMode (pin_switch +tabledRGB [numled][3], INPUT);
      break;
    case 2 : // vert
      pinMode(pin_switch +tabledRGB [numled][0], INPUT);
      pinMode (pin_switch +tabledRGB [numled][2], OUTPUT);
      pinMode (pin_switch +tabledRGB [numled][3], INPUT);
      break;
    case 3 : // bleu
      pinMode(pin_switch +tabledRGB [numled][0], INPUT);
      pinMode (pin_switch +tabledRGB [numled][2], INPUT);
      pinMode (pin_switch +tabledRGB [numled][3], OUTPUT);
      break;
    case 4 : // white
      pinMode(pin_switch +tabledRGB [numled][0], OUTPUT);
      pinMode (pin_switch +tabledRGB [numled][2], OUTPUT);
      pinMode (pin_switch +tabledRGB [numled][3], OUTPUT);
      break;
     break;
    case 5 : // magenta
      pinMode(pin_switch +tabledRGB [numled][0], OUTPUT);
      pinMode (pin_switch +tabledRGB [numled][2], INPUT);
      pinMode (pin_switch +tabledRGB [numled][3], OUTPUT);
      break;
    case 6 : // yellow
      pinMode(pin_switch +tabledRGB [numled][0], OUTPUT);
      pinMode (pin_switch +tabledRGB [numled][2], OUTPUT);
      pinMode (pin_switch +tabledRGB [numled][3], INPUT);
      break;
     case 7 : // cyan
      pinMode(pin_switch +tabledRGB [numled][0], INPUT);
      pinMode (pin_switch +tabledRGB [numled][2], OUTPUT);
      pinMode (pin_switch +tabledRGB [numled][3], OUTPUT);
      break;
      default : 
      break;
  };  

  
  
  for (i =0; i < time2 ; i++) {
    switch (coul) {
    case 0 : // rien
      break;
    case 1 : // rouge
      digitalWrite (pin_switch +tabledRGB [numled][0], HIGH);
      break;
    case 2 : // vert
      digitalWrite (pin_switch +tabledRGB [numled][2], HIGH);
      break;
    case 3 : // bleu
      digitalWrite (pin_switch +tabledRGB [numled][3], HIGH);
      break;
    case 4 : // white
      digitalWrite (pin_switch +tabledRGB [numled][0], HIGH);
      digitalWrite (pin_switch +tabledRGB [numled][2], HIGH);
      digitalWrite (pin_switch +tabledRGB [numled][3], HIGH);
      break;
    case 5 : // magenta
      digitalWrite (pin_switch +tabledRGB [numled][0], HIGH);
      digitalWrite (pin_switch +tabledRGB [numled][3], HIGH);
      break;
    case 6 : // yellow
      digitalWrite (pin_switch +tabledRGB [numled][0], HIGH);
      digitalWrite (pin_switch +tabledRGB [numled][2], HIGH);
      break;
    case 7 : // cyan
      digitalWrite (pin_switch +tabledRGB [numled][2], HIGH);
      digitalWrite (pin_switch +tabledRGB [numled][3], HIGH);
      break;
    default : 
      break;
  };  
   };
  } 


// ***** set up leds corresponding the number *****
void affichedeRGB(int n ,int temps,int color, int effect)
{    int i,j,tempsvide,z,k =0;   
     int tp_led , tp_boucle;

     if (n >6 || n <1) affiche_erreur (2);
     
     tp_led = temps_une_led / n;
     tp_boucle= temps;
 
 

  //boucle de repetition nombre de temps
  //par defaut 1
  if (effect==1) {
    tempsvide =150;
      for (j=0;j<tp_boucle;j++)
      {
          for (z=0; z< tempsvide; z++)
          {
          for(i=0;i<7;i++) 
          {  
            if (dede[n][i]==1) 
            {
                afficheledRGB(i,tp_led,color);
              }
            }
           }
          j=j+tempsvide;
          if (tempsvide > 0){
            afficheledRGB(i,tp_led,0);
            delay(tempsvide);
            j=j+tempsvide;
          }        
      }
    } 
    else {
      if (tp_boucle ==0) tp_boucle=1;
      for (j=0;j<tp_boucle;j++)
      {
          for(i=0;i<7;i++) 
          {  
            if (dede[n][i]==1) 
            {
                afficheledRGB(i,tp_led,color);
              }
          }
       }
    } 
}


void affiche_erreur(int erreur)
{
  int i,j;
  
    switch (erreur) {
      case 1 : // erreur affiche led
        for (i=0;i<10;i++) for (j=1;j<8;j++) afficheledRGB(j,temps_affiche,1);
        break;
      case 2 : // erreur affichede
        for (i=0;i<10;i++) for (j=1;j<4;j++) afficheledRGB(j,temps_affiche,1);
        break;
     default : 
        break;
    }
}
  

void Lancedes(int nbplay) 
{
  
 int n,j,i = 0; 
 int loto1;                // résultat random du nombre de tour de dès avant résultat
 int maxi =30;             // maxi du random du nombre de tour de dès avant résultat
 int mini = 7;             // mini du random du nombre de tour de dès avant résultat
 int delay1;               // temps d'affichage entre deux tours avant final
 int resultat = 10;        // résultat intermediaire
 int ancienresultat = 10;  // empeche deux fois le meme nombre
 

  loto1 = Entropy.random(mini,maxi);

 for (n=0; n < loto1;n++)
  {
     resultat=Entropy.random(1,7);      
     delay1=(temps_affiche)/(loto1-n);
     if (i>=nbplay) i=0;
     affichedeRGB(resultat,delay1,colorplayer[i],0);
     i++;
   }

#if !defined (ATTINY_MODE)
    Serial.print ("resultat final :");
    Serial.print(resultat);
    Serial.print(" nb resul inter : ");
    Serial.println(loto1);
#endif

i=i-1;
for (j = 0 ; j<nbplay; j++)
{
 affichedeRGB(resultat,temps_affiche*2, colorplayer[i],1);
 if (i>=nbplay-1) i=0;
 else i++;
 resultat=Entropy.random(1,7);      

 }
}

// Mood Light
void changeColor(byte num, byte numled)
{
  byte on, off,i,sens;
  
    // close all pins
  pinMode (pin_switch +Pin1, INPUT);
  pinMode (pin_switch +Pin2, INPUT);
  pinMode (pin_switch +Pin3, INPUT);
  pinMode (pin_switch +Pin4, INPUT);
  pinMode (pin_switch +Pin5, INPUT);
  pinMode (pin_switch +Pin6, INPUT);
  pinMode (pin_switch +Pin7, INPUT);

    // Pin GND
  pinMode(pin_switch +tabledRGB [numled][1],OUTPUT);
  digitalWrite(pin_switch +tabledRGB [numled][1], LOW);

  pinMode(pin_switch +tabledRGB [numled][0],OUTPUT);
  pinMode(pin_switch +tabledRGB [numled][2],OUTPUT);
  pinMode(pin_switch +tabledRGB [numled][3],OUTPUT);

  digitalWrite(pin_switch +tabledRGB [numled][tabcolor[num][0]], HIGH);
  digitalWrite(pin_switch +tabledRGB [numled][tabcolor[num][2]], LOW);
if ( num & 1) sens =1;
else sens=0;

  // fade up green
  for(i=1; i<time_fade; i++) {
 
    if ( sens==0) { 
      on  = i;
      off = time_fade-on;
    } 
    else 
    {
      on  = time_fade -i;
      off = i;      
    };
    
    for( byte a=0; a<time_fade; a++ ) {
      digitalWrite(pin_switch +tabledRGB [numled][tabcolor[num][2]], HIGH);
      delayMicroseconds(on);
      digitalWrite(pin_switch +tabledRGB [numled][tabcolor[num][2]], LOW);
      delayMicroseconds(off);
    }
  }
}

