/*
 =================================================================
 * Voice Recognition safebox V1.0（声纹保险箱）                 *
 * By Hardi(黄昊)&肖煜瀚 @MG_summer_camp 产品设计营 2019.08.09  *
 =================================================================

【ReadMe 说明】
---------------------------------------
	The safebox has two method to open the door, 
	we can either use our secret phrase to unlock it, 
	or we can input our password on the keyboard to unlock the device

	the password is four digit, and using "#" to clear the input, and "*" to comfirm the input,
	and the door will be open if the password is correct.

	by pushing down and hold the voice enable button, we can use our secret phrase to open the door as well.


	保险箱有两种开门的方法
	我们要么用我们的秘密短语来解锁它,
	或者我们可以在键盘上输入密码来解锁设备

	密码为四位数字,并使用“#”来清除输入,并用“*”来确认输入,
	如果密码正确,门将打开。

	通过按下并按住语音启用按钮,我们可以使用我们的秘密短语来打开门。


【Voice Recognition module (声控模块) - LD3320】
---------------
D2   -  IRQ
D4   -  NSS
D9   -  RST
D11  -  MOSI
D12  -  MISO
D13  -  SCK
GND  -  GND & WR
3.3V -  VCC

【matrix keypad (按键模块) - 3*4】
---------------
D3  -  1
D5  -  2
D6  -  3
D7  -  4
D10 -  5
D8  -  6
A3  -  7

【others (其他)】
---------------
A0 - Servo 舵机SIG
A1 - Led 指示灯
A2 - buzzer 蜂鸣器
A4 - Voice regognition enable button 语音识别开启按钮
A5 - Reed Switch 干簧管

*/

#include <Servo.h>
#include <ld3320.h>

#define doorSens A5
#define Led A1
#define buzzer A2

Servo lock;
int mode = 0;
const byte ROWS = 4; //FIVE  rows
const byte COLS = 4; //three columns
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'},
};
byte rowPins[ROWS] = {3, 5, 6, 7 }; 
byte colPins[COLS] = {10, 8, A3, A7};
int debounce[4][4]; 
int key = 0;              
byte realPassword[4] = {'1', '2', '3', '4'}; // four digit password
byte password[4] = {' ', ' ', ' ', ' '};
int pos=0;
unsigned long timer = 0;
VoiceRecognition Voice; 


void setup() {
  Serial.begin(9600);
  pinMode(A4, INPUT_PULLUP);
  pinMode(colPins[0], 1);
  pinMode(colPins[1], 1);
  pinMode(colPins[2], 1);
  pinMode(colPins[3], 1);
  for (int x = 0 ;  x < 4 ; x++) {
    pinMode(rowPins[x], 0);        //Set as input
    digitalWrite(rowPins[x], 1);   //Set internal pullup
  }
  
  pinMode(Led,OUTPUT);                    
  digitalWrite(Led,0);
  pinMode(A4,INPUT_PULLUP);
  pinMode(doorSens, INPUT_PULLUP);
  lock.attach(A0);
  lock.write(90);
  delay(2000);
  Serial.println("ready!");

  Voice.init();                             
  Voice.addCommand("zhi ma kai men",0);//secret phrase
  Voice.start();
}

void loop() {
  mode = !digitalRead(A4);
  //Serial.println(mode);
  if(mode){//btn pressed! voice activate
    delay(600);
    switch(Voice.read()){
      case 0:                                    
        openDoor();
        break;               
      default:
        digitalWrite(Led,LOW); 
        break;
    }
  }else{//keypad mode
    getKeys();
    if(key!=0){
      //Serial.println(key);
      if(key == 35){// # for clear
        playsound();
        for(int i=0;i<4;i++){
          password[i]  = ' ';
        }
        pos=0;
      }else if(key == 42){// *for enter
        checkPassword();
      }else{
        if(pos<4){
          password[pos]=key;
          pos++;  
          playsound();
        }
      }
      
    }

    for(int i=0;i<4;i++){
      Serial.print(password[i]);
      Serial.print(" ");
    }
    Serial.println();
    
    key=0;
  }
  delay(100);
  
}

void getKeys() {
  for (int column = 0 ; column < 4 ; column++ ) {
    digitalWrite(colPins[0], 1);      //Set all high
    digitalWrite(colPins[1], 1);
    digitalWrite(colPins[2], 1);
    digitalWrite(colPins[3], 1);
    digitalWrite(colPins[column], 0);   //Then set active column LOW
    for (int row = 0 ; row < 4 ; row++) {
      if (digitalRead(rowPins[row]) == 0) { //Is that switch pressed to active low?
        if (debounce[column][row] == 0) { //Debounce cleared?
          key = hexaKeys[row][column];
          debounce[column][row] = 5;
        }
        else {
          debounce[column][row] -= 1;   //If button pressed, count down debounce for repeat
        }
      }
      else {
        debounce[column][row] = 0;      //If button released, zero out debounce
      }   
    }
  }
}

void checkPassword(){
  bool isPass = 1;
  for(int i=0;i<4;i++)  {  
    if(realPassword[i]!=password[i]){
      isPass = 0;
    }
  }
  for(int i=0;i<4;i++){
    password[i]  = ' ';
  }
  pos=0; 
  if(isPass){
    Serial.println("success!");
    openDoor();
  }else{
    Serial.println("wrong password");
    key='#';
    playfail(); 
  }
}

void openDoor(){
  digitalWrite(Led,HIGH); 
  playUnlock();
  delay(200);
  lock.write(0);
  Serial.println("门已开！");
  delay(1000);
  while(digitalRead(doorSens)){
    Serial.println("waiting for the action");  
  }
  closeDoor();
}

void closeDoor(){
  delay(1000);
  digitalWrite(Led, LOW);
  playLocking();
  Serial.println("门已锁！");
  lock.write(90);
  for(int i=0;i<4;i++){
    password[i]  = ' ';
  }
  pos=0;
}

void playsound() {
  tone(buzzer, 131);
  delay(200);
  noTone(buzzer);
}

void playUnlock() {
  tone(buzzer, 200);
  delay(400);
  tone(buzzer, 250);
  delay(400);
  tone(buzzer, 300);
  delay(400);
  noTone(buzzer);
}

void playLocking(){
  tone(buzzer, 300);
  delay(400);
  tone(buzzer, 250);
  delay(400);
  tone(buzzer, 200);
  delay(400);
  noTone(buzzer);
}

void playfail() {
  tone(buzzer, 100);
  delay(200);
  noTone(buzzer);
  delay(50);
  tone(buzzer, 100);
  delay(200);
  noTone(buzzer);
}
