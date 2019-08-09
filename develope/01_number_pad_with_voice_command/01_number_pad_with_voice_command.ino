/*
 ==================================================
 * 声纹保险箱 V0.1 黄昊&肖煜瀚@MG产品设计营 2019.08.07  *
 ==================================================

【说明】
---------------------------------------


【声控模块 - LD3320】
---------------
D2   -  IRQ
D4   -  NSS
D9   -  RST
D11  -  MOSI
D12  -  MISO
D13  -  SCK
GND  -  GND & WR
3.3V -  VCC

【按键模块 - 3*4】
---------------
D3  -  1
D5  -  2
D6  -  3
D7  -  4
D10 -  5
D8  -  6
A3  -  7

【其他】
---------------
A0 - 舵机SIG
A1 - led指示灯
A2 - 蜂鸣器
A4 - 模式切换拨动开关
A5 - 干簧管

*/

#include <ld3320.h>
#include <Servo.h>
#include <Keypad.h> 
#include <Password.h>

int mode = 0;
int doorSens = A5;
Password password = Password( "2580" ); 
const byte ROWS = 4; // Four rows
const byte COLS = 3; // columns
// Define the Keymap
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
// Connect keypad ROW0, ROW1, ROW2 and ROW3 to these Arduino pins.
byte rowPins[ROWS] = { 3, 5, 6, 7 };// Connect keypad COL0, COL1 and COL2 to these Arduino pins.
byte colPins[COLS] = { 10, 8, A3};
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

VoiceRecognition Voice;                         //声明一个语音识别对象
Servo lock;
#define Led A1                                 //定义LED控制引脚
#define buzzer A2 
void setup() 
{
    pinMode(Led,OUTPUT);                        //初始化LED引脚为输出模式
    digitalWrite(Led,0);
    pinMode(A4,INPUT_PULLUP);
    pinMode(doorSens, INPUT_PULLUP);
    Serial.begin(9600);                        //配置9600
    Serial.println("Uart start!");  
    Serial.write(254);
    Serial.write(0x01);
    lock.attach(A0);
    lock.write(90);
    
    Voice.init();                               //初始化VoiceRecognition模块   
    Voice.addCommand("kai men",0);             //添加指令，参数（指令内容，指令标签（可重复））
    Voice.start();//开始识别
}

void loop() {
  mode = digitalRead(A4);
  Serial.print("mode: ");
  Serial.println(mode);
  
  if(mode){
    keypad.addEventListener(keypadEvent);
    keypad.getKey();
  }else{
    
    switch(Voice.read())                        
    {
      case 0:                                    
        openDoor();
        break;               
      default:
        digitalWrite(Led,LOW); 
        break;
    }
  }
}

void keypadEvent(KeypadEvent eKey) {
  switch (keypad.getState()) {
    case PRESSED:

      Serial.print("Enter:");
      Serial.println(eKey);
      playsound();

      Serial.write(254);

      switch (eKey) {
        case '#': checkPassword(); delay(1); break;

        case '*': password.reset(); delay(1); break;

        default: password.append(eKey); delay(1);
      }
  }
}

void checkPassword() {
  if (password.evaluate()) { //if password is right open
    Serial.println("Accepted");
    //Serial.write(254); delay(10);
    openDoor();
    
  } else { 
    Serial.println("Denied"); //if passwords wrong keep locked
    Serial.write(254); delay(10);
    playfail();
    password.reset();
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
