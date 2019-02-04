#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <HX711.h>

/////////////// Scale
HX711 scale(A1, A0);
long raw;
long offset = 0;
double Scale = 12295;
double init_weight;

/////////////// Motor
//Rotor
int enB = 3;
int in3 = 4;
int in4 = 5;

// Cart Motor
int enA = 25;
int in1 = 27;
int in2 = 29;
int direction = 1;



//////////// Keypad
const byte ROWS = 4; 
const byte COLS = 4; 


char hexaKeys[ROWS][COLS] = {
  {'1', '4', '7', '*'},
  {'2', '5', '8', '0'},
  {'3', '6', '9', '#'},
  {'A', 'B', 'C', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6}; 
byte colPins[COLS] = {13, 12, 11, 10}; 

////////////// Lcd
LiquidCrystal_I2C lcd(0x27,16,2); 
char start[] = "Select mass of ";
char start2[] = "salt: (1, 5, 7)";


Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void setup(){
  Serial.begin(9600);
  lcd.init(); //initialize the lcd
  lcd.backlight(); //open the bac 6klight   

  pinMode(enB, OUTPUT);// init motor pins
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  pinMode(enA, OUTPUT);// init motor pins
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  pinMode(22, OUTPUT); // Dump motor
  pinMode(23, OUTPUT);
  scale.set_gain(128);
  init_scale();
  Serial.println(offset);
}
  
void loop(){
  digitalWrite(10, LOW);
  askOption();
  char customKey = getOption();
  delay(1000);
  Serial.println(customKey);
  if (customKey == 53) {
    fiveG();
  } else if(customKey == 55) {
    sevenG();
  } else if(customKey == 49) {
    oneG();
  } else if(customKey == 'C') {
    refreshSalt();
  }

  lcd.clear();
  delay(1000);
  
  
}

///////////// Functions /////////////////////////

void askOption() {
  lcd.print(start);
  lcd.setCursor(0,1);
  lcd.print(start2);
}

int getOption() {
  int flag = 1;
  int customKey = 0;
  while(flag){
    customKey = customKeypad.getKey();
    if (customKey != 0) {
      return customKey;  
    }
  }
}

void fiveG(){
  lcd.clear();
  lcd.setCursor(0,0);
  char fiveGs[16] = "5g, A: Confirm";
  char fiveGs2[16] = "B: Cancel";
  lcd.print(fiveGs);
  lcd.setCursor(0,1);
  lcd.print(fiveGs2);
  char option = getOption();
  if(option == 'A') {
    countdown();
    startMotor();
  } 
}

void sevenG(){
  lcd.clear();
  lcd.setCursor(0,0);
  char sevenGs[16] = "7g, A: Confirm";
  char sevenGs2[16] = "B: Cancel";
  lcd.print(sevenGs);
  lcd.setCursor(0,1);
  lcd.print(sevenGs2);
  char option = getOption();
  if(option == 'A') {
    countdown();
    startMotor();
  } 
}

void countdown() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(3);
  delay(1000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(2);
  delay(1000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(1);
  delay(1000);
  lcd.clear();
}

void oneG(){
  lcd.clear();
  lcd.setCursor(0,0);
  char oneGs[16] = "1g, A: Confirm";
  char oneGs2[16] = "B: Cancel";
  lcd.print(oneGs);
  lcd.setCursor(0,1);
  lcd.print(oneGs2);
  char option = getOption();
  if(option == 'A') {
    countdown();
    dispenseSalt(1);
  } 
  dumpSalt();
}


void startMotor() {
  if (direction == 1) {
    digitalWrite(in4, LOW);
    digitalWrite(in3, HIGH);
    analogWrite(enB, 200); 
    delay(100);
  } else {
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    analogWrite(enB, 200);
    delay(100);
  }   
  direction = !direction;
  digitalWrite(in4, LOW);
  digitalWrite(in3, LOW);  
}

void init_scale(){
  lcd.print("Init...");
  for(int i = 0; i < 8; i++) {
    raw = scale.read_average(10);
    Serial.println(raw-offset);
    //delay(300);
  }
  raw = scale.read_average(100);
  offset = raw;
  Serial.println("INIT DONE");
  Serial.println(offset);
  lcd.clear();
  lcd.print("Init done.");
  delay(1000);
  lcd.clear();
}

void dispenseSalt(int grams) {
  double mass = 0;
  while(mass < grams) {
    raw = scale.read_average(10);
    mass = (raw - offset)/Scale;
    Serial.println(mass);
    startMotor();
  }
  lcd.clear();
  lcd.print(mass);
  delay(1000);
  lcd.clear();  
}

void dumpSalt() {
  digitalWrite(23, HIGH); 
  delay(300);
  digitalWrite(23, LOW);
  
  moveCart();

  digitalWrite(22, HIGH); // PURPLE
  delay(300);
  digitalWrite(22, LOW);
}

void moveCart() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  analogWrite(enA, 130);

  delay(1100);
    
  digitalWrite(in2, LOW);
  digitalWrite(in1, HIGH);
  analogWrite(enA, 130);
  delay(1100);

  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
}

void refreshSalt() {
  countdown();
  for( int i = 0; i < 10; i++){  
    digitalWrite(in4, LOW);
    digitalWrite(in3, HIGH);
    analogWrite(enB, 200); 
    delay(500);
    digitalWrite(in4, HIGH);
    digitalWrite(in3, LOW);
    analogWrite(enB, 200); 
    delay(500);
    digitalWrite(in4, LOW);
    digitalWrite(in3, LOW);
  }
}
