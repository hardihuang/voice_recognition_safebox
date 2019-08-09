#include <ld3320.h>
#include <Servo.h>
#include <Keypad.h> 
#include <Password.h>

VoiceRecognition Voice;                         //声明一个语音识别对象
Servo lock;
#define Led A1                                 //定义LED控制引脚
#define buzzer A2 
void setup() 
{
    pinMode(Led,OUTPUT);                        //初始化LED引脚为输出模式
    digitalWrite(Led,0);
    Serial.begin(9600);                        //配置9600
    Serial.println("Uart start!");  
    lock.attach(A0);
    lock.write(90);
    
    Voice.init();                               //初始化VoiceRecognition模块   
    Voice.addCommand("zhi ma kai men",0);             //添加指令，参数（指令内容，指令标签（可重复））
    Voice.start();//开始识别

}
void loop() {

  switch(Voice.read())                          //判断识别
  {
    case 0:                                     //若是指令“kai deng”
      openDoor();
      break;               
    default:
      digitalWrite(Led,LOW); 
      break;
  }
}

void openDoor(){
  digitalWrite(Led,HIGH); 
  playUnlock();
  delay(200);
  lock.write(0);
  Serial.println("门已开！");
  delay(3000);
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
