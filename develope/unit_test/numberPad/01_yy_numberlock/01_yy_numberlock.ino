#include <Servo.h>
#include <Keypad.h> 
#include <Password.h>

Password password = Password( "1234" ); 
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

Servo lock;
#define Led A1                                 //定义LED控制引脚
#define buzzer A2 
void setup() 
{
    pinMode(Led,OUTPUT);                        //初始化LED引脚为输出模式
    digitalWrite(Led,0);
    Serial.begin(9600);                        //配置9600
    Serial.write(254);
    Serial.write(0x01);
    lock.attach(A0);
    lock.write(90);
    
    keypad.addEventListener(keypadEvent); //add an event listener for this keypad

}
void loop() {
  keypad.getKey();
}

void keypadEvent(KeypadEvent eKey) {
  switch (keypad.getState()) {
    case PRESSED:

      Serial.print("Enter:");
      Serial.println(eKey);
      playsound();

      Serial.write(254);

      switch (eKey) {
        case '*': checkPassword(); delay(1); break;

        case '#': password.reset(); delay(1); break;

        default: password.append(eKey); delay(1);
      }
  }
}

void checkPassword() {
  if (password.evaluate()) { //if password is right open
    Serial.println("Accepted");
    Serial.write(254); delay(10);
    openDoor();
    password.reset();
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
