#include <Servo.h>

#define mode 0
#define doorSens A5
#define Led A1
#define buzzer A2

Servo lock;
const byte ROWS = 4; //FIVE  rows
const byte COLS = 4; //three columns
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'},
};
byte rowPins[ROWS] = {3, 5, 6, 7 };   //connect to the row pinouts of the keypad
byte colPins[COLS] = {10, 8, A3, A7};    //connect to the column pinouts of the keypad
int debounce[4][4]; 
int key = 0;              //What key is pressed
byte realPassword[4] = {'1', '2', '3', '4'};
byte password[4] = {' ', ' ', ' ', ' '};
int pos=0;
unsigned long timer = 0;

void setup() {
  Serial.begin(9600);
  pinMode(colPins[0], 1);
  pinMode(colPins[1], 1);
  pinMode(colPins[2], 1);
  pinMode(colPins[3], 1);
  for (int x = 0 ;  x < 4 ; x++) {
    pinMode(rowPins[x], 0);        //Set as input
    digitalWrite(rowPins[x], 1);   //Set internal pullup
  }
  
  pinMode(Led,OUTPUT);                        //初始化LED引脚为输出模式
  digitalWrite(Led,0);
  pinMode(A4,INPUT_PULLUP);
  pinMode(doorSens, INPUT_PULLUP);
  lock.attach(A0);
  lock.write(90);
  delay(2000);
  Serial.println("ready!");
}

void loop() {
  Serial.println(digitalRead(doorSens));
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
