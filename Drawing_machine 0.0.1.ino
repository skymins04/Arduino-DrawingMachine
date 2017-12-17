/*
  소스 제작기간: 2017. 12. 3 ~ 12. 15

   10201 강민수 (BetaMan)
   skymin0417@gmail.com

   steppeer's pin

                 IN1    IN2    IN3    IN4
  
  drawMotorA      2      3      4      5
  
  drawMotorB      6      7      8      9
  
  turnMotorC      10     11     12     13
  
  ==============================================================
  
  rotary encoder (Serial.end()를 사용해야 엔코더를 사용할 수 있음)
  
  SW   A0
  
  CLK  D0
  
  DT   D1
*/

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//==main source======================================================================================================================================
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include <StepperMulti.h>
#include <LiquidCrystal_I2C.h>
#include <Encoder.h>

// in4, in2, in3, in1
int stepsPerRavolution = 2080;
StepperMulti DrawMotorA(stepsPerRavolution, 5, 3, 4, 2);
StepperMulti DrawMotorB(stepsPerRavolution, 9, 7, 8, 6);
StepperMulti TurnMotorC(stepsPerRavolution, 13, 11, 12, 10);

LiquidCrystal_I2C LCD(0x27, 16, 2);

Encoder ENC(0,1);

// stepper speed
int DrawMotorA_Speed = 10;
int DrawMotorB_Speed = 10;
int TurnMotorC_Speed = 10;

// 0 = left direction, 1 = right direction
boolean DrawMotorA_Direction = 0;
boolean DrawMotorB_Direction = 0;
boolean TurnMotorC_Direction = 0;

long oldPosition = -999;
long newPosition = 0;
const int EncPower = 5;
int screenScroll = 0;
int select = 0;

int RTM = 0;

int cursorPosition = -1;

uint8_t copyright1[8] = {0x03, 0x04, 0x09, 0x0a, 0x0a, 0x09, 0x04, 0x03}; // copyright 특수 문자 HEX data
uint8_t copyright2[8] = {0x18, 0x04, 0x12, 0x02, 0x02, 0x12, 0x04, 0x18};
uint8_t cursorICon[8] = {0x02, 0x06, 0x0E, 0x1E, 0x1E, 0x0E, 0x06, 0x02}; // 커서 아이콘 HEX data
uint8_t starICon[8] = {0x00, 0x04, 0x04, 0x1f, 0x0e, 0x1b, 0x11, 0x00}; // 별 아이콘

char title[2][16] = {" DrawingMachine ", "================"};
char StartDrawing[2][16] = {"Start Drawing?  ", "1.Yes    2.No   "};
char info[2][16] = {"Version: 0.0.1v", " BetaMan"};

char mainMenu[3][16] = {"1.StartDrawing", "2.Option", "3.info"};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void setup() {
  Serial.end();

  LCD.begin();
  LCD.backlight();

  LCD.createChar(0, copyright1);
  LCD.createChar(1, copyright2);
  LCD.createChar(2, cursorICon);
  LCD.createChar(3, starICon);
  
  LCD.clear();
  LCD.home();

  DrawMotorA.setSpeed(DrawMotorA_Speed);
  DrawMotorB.setSpeed(DrawMotorB_Speed);
  TurnMotorC.setSpeed(TurnMotorC_Speed);

  PrintLCD(title);  
  LCD.setCursor(0,0); LCD.write(3);
  LCD.setCursor(15,0); LCD.write(3);
  
  delay(2000);
  LCD.clear();
  delay(1000);
  
  LCD.clear();
  LCD.setCursor(0,0);
  LCD.print(mainMenu[0]); 
  LCD.setCursor(0,1);
  LCD.print(mainMenu[1]);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void loop() {
  newPosition = ENC.read();
  if(newPosition > oldPosition + EncPower || newPosition < oldPosition - EncPower) {
MainMenu:
    LCD_MainMenu(); 
  }
  if(readOK() == 1){
    while(readOK() == 1) {}
    if(screenScroll == 0 && cursorPosition == 0) { // start drawing
      StartDrawingMenu();
      goto MainMenu; 
    }
    else if((screenScroll == 0 && cursorPosition == 1) || (screenScroll == 1 && cursorPosition == 0)) { // option
      LCD.clear();
      LCD.print("Test 2");
      delay(1000);
    }
    else if(screenScroll == 1 && cursorPosition == 1) { // info
      InfoMenu();
      goto MainMenu;
    }
  }  
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void PrintLCD(char a[][16]) {
  for(int i = 0; i < 2; i++){
    LCD.setCursor(0,i);
    for(int j = 0; j < 16; j++){
      LCD.setCursor(j,i);
      LCD.print(a[i][j]);  
    }  
  }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void LCD_MainMenu() {
  LCD.setCursor(15, 0);
  LCD.print(' ');
  LCD.setCursor(15, 1);
  LCD.print(' ');
  if(newPosition > oldPosition) {
    if(cursorPosition == 1 && screenScroll == 0) {
      LCD.clear();
      LCD.setCursor(0,0);
      LCD.print(mainMenu[1]); 
      LCD.setCursor(0,1);
      LCD.print(mainMenu[2]);
      screenScroll = 1;
    }
    if(cursorPosition != 1) cursorPosition += 1; // down
  } else if(newPosition < oldPosition) {
    if(cursorPosition == 0 && screenScroll == 1) {
      LCD.clear();
      LCD.setCursor(0,0);
      LCD.print(mainMenu[0]); 
      LCD.setCursor(0,1);
      LCD.print(mainMenu[1]);
      screenScroll = 0;
    }
    if(cursorPosition != 0) cursorPosition -= 1; // up
  }
  if(RTM == 1) {
    if(screenScroll == 0) {
      LCD.clear();
      LCD.setCursor(0,0);
      LCD.print(mainMenu[0]); 
      LCD.setCursor(0,1);
      LCD.print(mainMenu[1]);
    } 
    else if(screenScroll == 1) {
      LCD.clear();
      LCD.setCursor(0,0);
      LCD.print(mainMenu[1]); 
      LCD.setCursor(0,1);
      LCD.print(mainMenu[2]);
    } 
    RTM = 0;
  }
  LCD.setCursor(15, cursorPosition);
  LCD.write(2);
  oldPosition = newPosition;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void StartDrawingMenu() {
  PrintLCD(StartDrawing);
  LCD.setCursor(7,1); LCD.write(2);
  select = 0;
  int temp = newPosition;

  while(readOK() == 0) {
    newPosition = ENC.read();
    if(newPosition > oldPosition) {
      if(select == 0) {
        LCD.setCursor(7,1); LCD.print(' ');
        LCD.setCursor(14,1); LCD.write(2);
        select = 1;
      }  
    } 
    else if(newPosition < oldPosition) {
      if(select == 1) {
        LCD.setCursor(14,1); LCD.print(' ');
        LCD.setCursor(7,1); LCD.write(2);
        select = 0;
      }
    } 
  }
  if(select == 0) {
        
  }
  while(readOK() == 1){}
  newPosition = temp;
  RTM = 1; 
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void InfoMenu() {
  LCD.clear();
  LCD.print(info[0]);
  LCD.setCursor(0,1);
  LCD.write(0); LCD.write(1);
  LCD.print(info[1]);
  
  while(readOK() == 1) {}
  while(readOK() == 0) {}
  while(readOK() == 1) {}  
  
  RTM = 1;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int readOK() {
  int a[50] = {0, };
  int b = 0;
  for(int i = 0; i < 50; i++){
    if(analogRead(0) == 0) a[i] = 1;
    delay(0.01);
  }  
  for(int i = 0; i < 50; i++){
    b += a[i];
  }
  if(b == 50) return 1;
  if(b != 50) return 0;
}
