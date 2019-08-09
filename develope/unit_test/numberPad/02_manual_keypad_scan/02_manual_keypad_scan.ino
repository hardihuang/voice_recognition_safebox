const byte ROWS = 4; //FIVE  rows
const byte COLS = 4; //three columns
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'},
};
byte rowPins[ROWS] = {3, 5, 6, 7 };   //connect to the row pinouts of the keypad
byte colPins[COLS] = {10, 8, A3, A5};    //connect to the column pinouts of the keypad
int debounce[4][4]; 
int key = 0;              //What key is pressed
byte input[127];            //The input buffer
byte buffPos = 0;           //Where we are in the buffer
byte numberCall[11] = {'_', '_', '_', '_', '_', '_', '_', '_', '_', '_', '_',};
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

  delay(2000);
}

void loop() {
  getKeys();
  if(key!=0){
    Serial.println(key);
  }
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
          debounce[column][row] = 2000;
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
