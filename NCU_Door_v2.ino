#include <b64.h>
#include <HttpClient.h>
#include <Bridge.h>
#include <BridgeClient.h>
#include <BridgeServer.h>
//#include <BridgeSSLClient.h>
#include <BridgeUdp.h>
//#include <Console.h>
//#include <FileIO.h>
//#include <HttpClient.h>
//#include <Mailbox.h>
//#include <Process.h>
//#include <YunClient.h>
//#include <YunServer.h>
#include <Keypad.h>
#include <Keypad_I2C.h>
#include <LiquidCrystal_I2C.h>
#include <stdio.h>
#include <stdlib.h>

//#define LCDI2CAddr 0x27
#define LCDI2CAddr 0x0f
#define KBI2CAddr 0x38

LiquidCrystal_I2C lcd (LCDI2CAddr, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char keys[ROWS][COLS] = {
  {'F', 'E', 'D', 'C'},
  {'B', '3', '6', '9'},
  {'A', '2', '5', '8'},
  {'0', '1', '4', '7'}
};

byte rowPins[ROWS] = {4, 5, 6, 7}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {0, 1, 2, 3}; //connect to the column pinouts of the keypad

Keypad_I2C kpd (makeKeymap(keys), rowPins, colPins, ROWS, COLS, KBI2CAddr, PCF8574);

char server[16] = "140.115.3.188";
char path[70] = "/door/v1/authorization_tokens/";

/*
byte mac[] = {
  0x9E , 0x65 , 0xF9 , 0x0B , 0x2A , 0xD3
};

byte mac2[] = { 0x9C , 0x65 , 0xF9 , 0x1B , 0x22 , 0xA0 };
byte byteIP[4] = {192, 168, 0, 101};
*/
int code_len = 5;
char code[6];

char title[20] = "Door.v2 I216";
char code_str[20] = "Code: ";


void printStr (char* s) {
  for ( int i = 0; s[i] != '\0' && s[i] != '\x0a'; ++ i ) lcd.print(s[i]);
}

char wait_key(){
  char c = '\x00';
  while( !c ) c = kpd.getKey();
  return c;  
}

void input(){
  for( int i = 0 ; i < code_len ; i++ ){
    code[i] = wait_key();
    if( code[i] == 'C' ){
      if( !i ){
        --i;
        continue;  
      }
      --i;
      lcd.setCursor( strlen( code_str ) + i , 1);
      lcd.print( '\x20' );
      --i;
      continue;
    }
    lcd.setCursor( strlen( code_str ) + i , 1);
    lcd.print( code[i] );
  }  
  code[5] = '\0';
}

void Title(){
  lcd.clear();
  lcd.setCursor (0, 0);
  printStr( title ); 
}

void line( char* s ){
  lcd.setCursor (0, 1);
  printStr( s );
}

void triggerDoor(){
  //line("T");
  digitalWrite(6, HIGH);
  delay(200);       
  digitalWrite(6, LOW);
}

void http(){
  
  line( code_str );
  input();
  
  char request[70];
  char response[16];
  snprintf( request , sizeof( request ) , "%s%s" , path , code );
  
  BridgeClient c;
  HttpClient client(c);
  client.get( server , request );

  Title();
  int StatusCode = client.responseStatusCode();
  snprintf( response , sizeof(response) , "      $%d" , StatusCode );
  line( response );
  if( StatusCode == 204 ) triggerDoor();
  delay(1000);
  Title();
}


void setup() {
  //Wire.begin();
  //Console.begin();
  //while (!Console);
  //Console.println("\nInit!!!");
  Serial.begin(9600);
  kpd.begin ( makeKeymap(keys) );
  lcd.begin (16, 2);
  lcd.backlight ();
  Title();
  line("Bridging...");
  Bridge.begin();
  //Ethernet.begin( mac2 );
  Title();
  //if (Ethernet.begin( mac2 ) == 0) line("FUCK");
  //Console.println(Ethernet.localIP());
  //lcd.backlight ();
  pinMode(6, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(3, OUTPUT);
  //digitalWrite(5, HIGH);
}

void pwn(){
  if( wait_key() == '3' )
    if( wait_key() == '9' )
      if( wait_key() == '7' )
        if( wait_key() == '7' ){
          //line( "Black pwn :(" );
          delay( 100 );
          line( "Good job :) " );
          delay( 100 );
          triggerDoor();
          //delay( 300 );
          delay( 770 );
          Title();
        }
}

void Reset(){
  setup();
}


void loop() {

  char key;
  key = kpd.getKey();
  if(key){
    switch( key ){
      case 'A':
        digitalWrite(4, HIGH);
        delay(70);       
        digitalWrite(4, LOW);
        break;
      case 'B':
        Reset();
        break;
      case 'F':
        Title();
        http();
        break;
      case 'C':
        pwn();
        break;
        
    }
  }

}
