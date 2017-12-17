int FirmwareVer[3] = {0, 0, 2};
  
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#include <Stepper.h>
Stepper AngleA(2080, 5, 3, 4, 2);
Stepper AngleB(2080, 9, 7, 8, 6);
Stepper AngleC(2080, 13, 11, 12, 10);

#include <StepperMulti.h>

int stepPerRevolution[4] = {400, 400, 400, NULL};

int MAngle[4] = {0, 0, 0, NULL};

int MDir[4] = {0, 0, 0, NULL};

int MActive[3] = {1, 1, NULL};

uint8_t copyright1[8] = {0x03, 0x04, 0x09, 0x0a, 0x0a, 0x09, 0x04, 0x03}; // copyright 특수 문자 HEX data
uint8_t copyright2[8] = {0x18, 0x04, 0x12, 0x02, 0x02, 0x12, 0x04, 0x18};
uint8_t cursorICon[8] = {0x02, 0x06, 0x0E, 0x1E, 0x1E, 0x0E, 0x06, 0x02}; // 커서 아이콘 HEX data
uint8_t starICon[8] = {0x00, 0x04, 0x04, 0x1f, 0x0e, 0x1b, 0x11, 0x00}; // 별 아이콘

//==LCD Menus==========================================================================================================================================================
char startScreen[][16] = {" DrawingMachine ", "================"};

char MainMenu[][16] = {"1.StartDrawing", "2.Option", "3.info"};

char StartDrawingMenu1[][16] = {"Start Drawing?", "1.Yes    2.No"};
char StartDrawingMenu2[][16] = {"Drawing...", "Press OK to stop"};

char OptionMenu[][16] = {"1.set Speed", "2.set Angle", "3.set direction", "4.set active" , "5.exit"};
char OptionMenuSpeed[][16] = {"DrawA: ", "DrawB: ", "TurnC: ", "exit"};
char OptionMenuAngle[][16] = {"DrawA: ", "DrawB: ", "TurnC: ", "exit"};
char OptionMenuDir[][16] = {"DrawA: ", "DrawB: ", "TurnC: ", "exit"};
char OptionMenuActive[][16] = {"DrawA: ", "DrawB: ", "exit"};

char infoMenu[][16] = {"Version: ", "BetaMan"};
//=====================================================================================================================================================================

#include <Encoder.h>
Encoder Enc(0, 1);
int newEnc = 0;
int oldEnc = -999;
int EncPower = 5;
int cursorPosition = 0;
int scrollPosition = 0;
int returnMain = 0;

void setup() {
  AngleA.setSpeed(10);
  AngleB.setSpeed(10);
  AngleC.setSpeed(10);
  
  lcd.begin();
  lcd.backlight();
  lcd.createChar(0, copyright1);
  lcd.createChar(1, copyright2);
  lcd.createChar(2, cursorICon);
  lcd.createChar(3, starICon);
  lcd.home();

  StartScreenFn(); // 그래픽 요소
  lcd.setCursor(0, 0); lcd.write(3);
  lcd.setCursor(15, 0); lcd.write(3);
  delay(1000);
  lcd.clear();
  delay(1000);
  lcd.setCursor(0, 0); lcd.print(MainMenu[0]);
  lcd.setCursor(0, 1); lcd.print(MainMenu[1]);
  lcd.setCursor(15, 0); lcd.write(2);

  oldEnc = Enc.read();
}

void loop() {
  newEnc = Enc.read();
  if (newEnc > oldEnc + EncPower || newEnc < oldEnc - EncPower) {
MainLabel:
    MainMenuFn();
  }
  if (readOK() == 1) { // main menu select
    while (readOK() == 1) {}

    if (cursorPosition == 0 && scrollPosition == 0) { // 1.start drawing
      StartDrawingMenuFn();
      goto MainLabel;
    }
    else if ((cursorPosition == 1 && scrollPosition == 0) || (cursorPosition == 0 && scrollPosition == 1)) { // 2.Option
      OptionMenuFn();
      goto MainLabel;
    }
    else if (cursorPosition == 1 && scrollPosition == 1) { // 3.info
      infoMenuFn();
      goto MainLabel;
    }
  }
}

//==Functions===========================================================================================================================================================

int readOK() { // rotary encoder swich read
  int save = 0;
  int check = 100;
  for (int i = 0; i < check; i++) {
    if (analogRead(0) == 0) save += 1;
  }
  if (save == check) return 1;
  else return 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void StartScreenFn() { // print start screen
  lcd.setCursor(0, 0);
  lcd.print(startScreen[0]);
  lcd.setCursor(0, 1);
  lcd.print(startScreen[1]);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void MainMenuFn() { // main menu process
  lcd.setCursor(15, 0); lcd.print(' '); // clear cursor
  lcd.setCursor(15, 1); lcd.print(' ');

  if (newEnc > oldEnc) {
    if (cursorPosition == 1 && scrollPosition == 0) { // 커서가 아래, 스크롤이 0일때 아래로 스크롤하면
      lcd.clear();
      lcd.setCursor(0, 0); lcd.print(MainMenu[1]);
      lcd.setCursor(0, 1); lcd.print(MainMenu[2]);
      scrollPosition += 1;
    }
    if (cursorPosition == 0) cursorPosition = 1;
  }
  else if (newEnc < oldEnc) {
    if (cursorPosition == 0 && scrollPosition == 1) { // 커서가 위, 스크롤이 1일때 위로 스크롤하면
      lcd.clear();
      lcd.setCursor(0, 0); lcd.print(MainMenu[0]);
      lcd.setCursor(0, 1); lcd.print(MainMenu[1]);
      scrollPosition -= 1;
    }
    if (cursorPosition == 1) cursorPosition = 0;
  }
  if (returnMain == 1) {
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print(MainMenu[0]);
    lcd.setCursor(0, 1); lcd.print(MainMenu[1]);
    scrollPosition = 0;
    cursorPosition = 0;
    returnMain = 0;
  }
  lcd.setCursor(15, cursorPosition); lcd.write(2);
  oldEnc = newEnc;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void StartDrawingMenuFn() {
  cursorPosition = 0;
  lcd.clear(); lcd.home();
  lcd.print(StartDrawingMenu1[0]);
  lcd.setCursor(0, 1); lcd.print(StartDrawingMenu1[1]);
  lcd.setCursor(6, 1); lcd.write(2);
  oldEnc = Enc.read();

  while (readOK() == 0) {
    newEnc = Enc.read();
    if (newEnc > oldEnc + EncPower || newEnc < oldEnc - EncPower) {
      if (cursorPosition == 0 && newEnc > oldEnc) {
        lcd.setCursor(6, 1); lcd.print(' ');
        lcd.setCursor(14, 1); lcd.write(2);
        cursorPosition = 1;
        oldEnc = newEnc;
      }
      else if (cursorPosition == 1 && newEnc < oldEnc) {
        lcd.setCursor(6, 1); lcd.write(2);
        lcd.setCursor(14, 1); lcd.print(' ');
        cursorPosition = 0;
        oldEnc = newEnc;
      }
    }
  }
  while (readOK() == 1) {}

  if (cursorPosition == 1) returnMain = 1;
  else if (cursorPosition == 0) {
    
    StepperMulti DrawA(stepPerRevolution[0], 5, 3, 4, 2);
    StepperMulti DrawB(stepPerRevolution[1], 9, 7, 8, 6);
    StepperMulti TurnC(stepPerRevolution[2], 13, 11, 12, 10);
    
    lcd.clear(); lcd.home();
    lcd.print(StartDrawingMenu2[0]);
    lcd.setCursor(0, 1); lcd.print(StartDrawingMenu2[1]);

    DrawA.setSpeed(MActive[0] ==  0 ? 0 : 10);
    DrawB.setSpeed(MActive[1] ==  0 ? 0 : 10);
    TurnC.setSpeed(10);
    int count = 0;

    while (1) {
      if (readOK() == 1) break;
      DrawA.setStep(MDir[0] == 0 ? stepPerRevolution[0] : -stepPerRevolution[0]);
      DrawB.setStep(MDir[1] == 0 ? stepPerRevolution[1] : -stepPerRevolution[1]);
      TurnC.setStep(MDir[2] == 0 ? stepPerRevolution[2] : -stepPerRevolution[2]);
      DrawA.moveStep();
      DrawB.moveStep();
      TurnC.moveStep();
      count += 1;
      if(count == 200) {
        for(int i = 2; i < 14; i++) {
          digitalWrite(i, LOW);  
        } 
        count = 0;
      }
    }
    lcd.clear(); lcd.home();
    lcd.print(MainMenu[0]);
    lcd.setCursor(0, 1); lcd.print(MainMenu[1]);
    lcd.setCursor(15, 0); lcd.write(2);
    while (readOK() == 1) {}
    returnMain = 1;
  }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void OptionMenuFn() {
OptionLabel:
  scrollPosition = 0;
  cursorPosition = 0;

  lcd.clear(); lcd.home();
  lcd.print(OptionMenu[0]);
  lcd.setCursor(0, 1); lcd.print(OptionMenu[1]);

  lcd.setCursor(15, 0); lcd.write(2);
  oldEnc = Enc.read();
  int i = 0, j = 1;

  while (readOK() == 0) {
    newEnc = Enc.read();
    if (newEnc > oldEnc + EncPower || newEnc < oldEnc - EncPower) {
      if (newEnc > oldEnc) {
        if (cursorPosition == 1 && scrollPosition != 3) {
          lcd.clear();
          ++i; ++j;
          lcd.setCursor(0, 0); lcd.print(OptionMenu[i]);
          lcd.setCursor(0, 1); lcd.print(OptionMenu[j]);
          scrollPosition += 1;
        }
        if (cursorPosition == 0) cursorPosition = 1;
      }
      else if (newEnc < oldEnc) {
        if (cursorPosition == 0 && scrollPosition != 0) {
          lcd.clear();
          --i; --j;
          lcd.setCursor(0, 0); lcd.print(OptionMenu[i]);
          lcd.setCursor(0, 1); lcd.print(OptionMenu[j]);
          scrollPosition -= 1;
        }
        if (cursorPosition == 1) cursorPosition = 0;
      }
      lcd.setCursor(15, 0); lcd.print(' ');
      lcd.setCursor(15, 1); lcd.print(' ');
      lcd.setCursor(15, cursorPosition); lcd.write(2);
      oldEnc = newEnc;
    }
  }
  while (readOK() == 1) {}

  if (cursorPosition == 0 && scrollPosition == 0) { // speed
setSpeedLabel:
    cursorPosition = 0;
    scrollPosition = 0;
    i = 0; j = 1;

    lcd.clear(); lcd.home();
    lcd.print(OptionMenuSpeed[0]); lcd.print(stepPerRevolution[0]);
    lcd.setCursor(0, 1); lcd.print(OptionMenuSpeed[1]); lcd.print(stepPerRevolution[1]);

    lcd.setCursor(15, 0); lcd.write(2);
    oldEnc = Enc.read();

    while (readOK() == 0) {
      newEnc = Enc.read();
      if (newEnc > oldEnc + EncPower || newEnc < oldEnc - EncPower) {
        if (newEnc > oldEnc) {
          if (cursorPosition == 1 && scrollPosition != 2) {
            lcd.clear();
            ++i; ++j;
            lcd.setCursor(0, 0); lcd.print(OptionMenuSpeed[i]); lcd.print(stepPerRevolution[i]);
            lcd.setCursor(0, 1); lcd.print(OptionMenuSpeed[j]); lcd.print(stepPerRevolution[j]);
            if (j == 3) {
              lcd.setCursor(4, 1); lcd.print("      ");
            }
            scrollPosition += 1;
          }
          if (cursorPosition == 0) cursorPosition = 1;
        }
        else if (newEnc < oldEnc) {
          if (cursorPosition == 0 && scrollPosition != 0) {
            lcd.clear();
            --i; --j;
            lcd.setCursor(0, 0); lcd.print(OptionMenuSpeed[i]); lcd.print(stepPerRevolution[i]);
            lcd.setCursor(0, 1); lcd.print(OptionMenuSpeed[j]); lcd.print(stepPerRevolution[j]);
            scrollPosition -= 1;
          }
          if (cursorPosition == 1) cursorPosition = 0;
        }
        lcd.setCursor(15, 0); lcd.print(' ');
        lcd.setCursor(15, 1); lcd.print(' ');
        lcd.setCursor(15, cursorPosition); lcd.write(2);
        oldEnc = newEnc;
      }
    }
    while (readOK() == 1) {}

    if (cursorPosition == 0 && scrollPosition == 0) {
      oldEnc = Enc.read();
      while (readOK() == 0) {
        newEnc = Enc.read();
        if (newEnc > oldEnc + 2) {
          stepPerRevolution[0] += 10;
          if (stepPerRevolution[0] > 600) stepPerRevolution[0] -= 10;
          lcd.setCursor(7, cursorPosition); lcd.print("        ");
          lcd.setCursor(7, cursorPosition); lcd.print(stepPerRevolution[0]);
          oldEnc = newEnc;
        }
        else if (newEnc < oldEnc - 2) {
          stepPerRevolution[0] -= 10;
          if (stepPerRevolution[0] < 0) stepPerRevolution[0] += 10;
          lcd.setCursor(7, cursorPosition); lcd.print("        ");
          lcd.setCursor(7, cursorPosition); lcd.print(stepPerRevolution[0]);
          oldEnc = newEnc;
        }
      }
      while (readOK() == 1) {}
      goto setSpeedLabel ;
    }
    else if (cursorPosition == 1 && scrollPosition == 0 || cursorPosition == 0 && scrollPosition == 1) {
     oldEnc = Enc.read();
      while (readOK() == 0) {
        newEnc = Enc.read();
        if (newEnc > oldEnc + 2) {
          stepPerRevolution[1] += 10;
          if (stepPerRevolution[1] > 600) stepPerRevolution[1] -= 10;
          lcd.setCursor(7, cursorPosition); lcd.print("        ");
          lcd.setCursor(7, cursorPosition); lcd.print(stepPerRevolution[1]);
          oldEnc = newEnc;
        }
        else if (newEnc < oldEnc - 2) {
          stepPerRevolution[1] -= 10;
          if (stepPerRevolution[1] < 0) stepPerRevolution[1] += 10;
          lcd.setCursor(7, cursorPosition); lcd.print("        ");
          lcd.setCursor(7, cursorPosition); lcd.print(stepPerRevolution[1]);
          oldEnc = newEnc;
        }
      }
      while (readOK() == 1) {}
      goto setSpeedLabel;
    }
    else if (cursorPosition == 1 && scrollPosition == 1 || cursorPosition == 0 && scrollPosition == 2) {
      oldEnc = Enc.read();
      while (readOK() == 0) { 
        newEnc = Enc.read();
        if (newEnc > oldEnc + 2) {
          stepPerRevolution[2] += 10;
          if (stepPerRevolution[2] > 600) stepPerRevolution[2] -= 10;
          lcd.setCursor(7, cursorPosition); lcd.print("        ");
          lcd.setCursor(7, cursorPosition); lcd.print(stepPerRevolution[2]);
          oldEnc = newEnc;
        }
        else if (newEnc < oldEnc - 2) {
          stepPerRevolution[2] -= 10;
          if (stepPerRevolution[2] < 0) stepPerRevolution[2] += 10;
          lcd.setCursor(7, cursorPosition); lcd.print("        ");
          lcd.setCursor(7, cursorPosition); lcd.print(stepPerRevolution[2]);
          oldEnc = newEnc;
        }
      }
      while (readOK() == 1) {}
      goto setSpeedLabel;
    }
    else if (cursorPosition == 1 && scrollPosition == 2) {
      goto OptionLabel;
    }
  }
  else if (cursorPosition == 1 && scrollPosition == 0 || cursorPosition == 0 && scrollPosition == 1) { // Angle
setAngleLabel:
    cursorPosition = 0;
    scrollPosition = 0;
    i = 0; j = 1;

    for(int k = 0; k < 3; k++) {
      MAngle[k] = 0;  
    }

    lcd.clear(); lcd.home();
    lcd.print(OptionMenuAngle[0]); lcd.print(MAngle[0]);
    lcd.setCursor(0, 1); lcd.print(OptionMenuAngle[1]); lcd.print(MAngle[1]);

    lcd.setCursor(15, 0); lcd.write(2);
    oldEnc = Enc.read();

    while (readOK() == 0) {
      newEnc = Enc.read();
      if (newEnc > oldEnc + EncPower || newEnc < oldEnc - EncPower) {
        if (newEnc > oldEnc) {
          if (cursorPosition == 1 && scrollPosition != 2) {
            lcd.clear();
            ++i; ++j;
            lcd.setCursor(0, 0); lcd.print(OptionMenuAngle[i]); lcd.print(MAngle[i]);
            lcd.setCursor(0, 1); lcd.print(OptionMenuAngle[j]); lcd.print(MAngle[j]);
            if (j == 3) {
              lcd.setCursor(4, 1); lcd.print("  ");
            }
            scrollPosition += 1;
          }
          if (cursorPosition == 0) cursorPosition = 1;
        }
        else if (newEnc < oldEnc) {
          if (cursorPosition == 0 && scrollPosition != 0) {
            lcd.clear();
            --i; --j;
            lcd.setCursor(0, 0); lcd.print(OptionMenuAngle[i]); lcd.print(MAngle[i]);
            lcd.setCursor(0, 1); lcd.print(OptionMenuAngle[j]); lcd.print(MAngle[j]);
            if (j == 3) {
              lcd.setCursor(4, 1); lcd.print("  ");
            }
            scrollPosition -= 1;
          }
          if (cursorPosition == 1) cursorPosition = 0;
        }
        lcd.setCursor(15, 0); lcd.print(' ');
        lcd.setCursor(15, 1); lcd.print(' ');
        lcd.setCursor(15, cursorPosition); lcd.write(2);
        oldEnc = newEnc;
      }
    }
    while (readOK() == 1) {}

    if (cursorPosition == 0 && scrollPosition == 0) {
      oldEnc = Enc.read();
      while (readOK() == 0) {
        newEnc = Enc.read();
        if (newEnc > oldEnc + 1) {
          ++MAngle[0];
          if(MAngle[0] > 360) --MAngle[0];
          lcd.setCursor(7, cursorPosition); lcd.print("        ");
          lcd.setCursor(7, cursorPosition); lcd.print(MAngle[0]);
          oldEnc = newEnc;
        }
        else if (newEnc < oldEnc - 1) {
          --MAngle[0];
          if(MAngle[0] < -360) ++MAngle[0];
          lcd.setCursor(7, cursorPosition); lcd.print("        ");
          lcd.setCursor(7, cursorPosition); lcd.print(MAngle[0]);
          oldEnc = newEnc;
        }
      }
      while (readOK() == 1) {}
      AngleA.step(MAngle[0] * -5.5);
      for(int pin = 2; pin < 6; pin++) {
        digitalWrite(pin, LOW);  
      }
      goto setAngleLabel;
    }
    else if (cursorPosition == 1 && scrollPosition == 0 || cursorPosition == 0 && scrollPosition == 1) {
      oldEnc = Enc.read();
      while (readOK() == 0) {
        newEnc = Enc.read();
        if (newEnc > oldEnc + 2) {
          ++MAngle[1];
          if(MAngle[1] > 360) --MAngle[1];
          lcd.setCursor(7, cursorPosition); lcd.print("        ");
          lcd.setCursor(7, cursorPosition); lcd.print(MAngle[1]);
          oldEnc = newEnc;
        }
        else if (newEnc < oldEnc - 2) {
          --MAngle[1];
          if(MAngle[1] < -360) ++MAngle[1];
          lcd.setCursor(7, cursorPosition); lcd.print("        ");
          lcd.setCursor(7, cursorPosition); lcd.print(MAngle[1]);
          oldEnc = newEnc;
        }
      }
      while (readOK() == 1) {}
      AngleB.step(MAngle[1] * -6);
      for(int pin = 6; pin < 10; pin++) {
        digitalWrite(pin, LOW);  
      }
      goto setAngleLabel;
    }
    else if (cursorPosition == 1 && scrollPosition == 1 || cursorPosition == 0 && scrollPosition == 2) {
      oldEnc = Enc.read();
      while (readOK() == 0) {
        newEnc = Enc.read();
        if (newEnc > oldEnc + -5.5) {
          ++MAngle[2];
          lcd.setCursor(7, cursorPosition); lcd.print("        ");
          lcd.setCursor(7, cursorPosition); lcd.print(MAngle[2]);
          oldEnc = newEnc;
        }
        else if (newEnc < oldEnc - 2) {
          --MAngle[2];
          lcd.setCursor(7, cursorPosition); lcd.print("        ");
          lcd.setCursor(7, cursorPosition); lcd.print(MAngle[2]);
          oldEnc = newEnc;
        }
      }
      while (readOK() == 1) {}
      AngleC.step(MAngle[2] * -5.5);
      for(int pin = 10; pin < 14; pin++) {
        digitalWrite(pin, LOW);  
      }
      goto setAngleLabel;
    }
    else if (cursorPosition == 1 && scrollPosition == 2) {
      goto OptionLabel;
    }
  }
  else if (cursorPosition == 1 && scrollPosition == 1 || cursorPosition == 0 && scrollPosition == 2) { // direction
setDirectionLabel:
    cursorPosition = 0;
    scrollPosition = 0;
    i = 0; j = 1;

    lcd.clear(); lcd.home();
    lcd.print(OptionMenuDir[0]); if (MDir[0] == 0) lcd.print("Left"); else lcd.print("Right");
    lcd.setCursor(0, 1); lcd.print(OptionMenuDir[1]); if (MDir[1] == 0) lcd.print("Left"); else lcd.print("Right");

    lcd.setCursor(15, 0); lcd.write(2);
    oldEnc = Enc.read();

    while (readOK() == 0) {
      newEnc = Enc.read();
      if (newEnc > oldEnc + EncPower || newEnc < oldEnc - EncPower) {
        if (newEnc > oldEnc) {
          if (cursorPosition == 1 && scrollPosition != 2) {
            lcd.clear();
            ++i; ++j;
            lcd.setCursor(0, 0); lcd.print(OptionMenuDir[i]); if (MDir[i] == 0) lcd.print("Left"); else lcd.print("Right");
            lcd.setCursor(0, 1); lcd.print(OptionMenuDir[j]); if (MDir[j] == 0) lcd.print("Left"); else lcd.print("Right");
            if (j == 3) {
              lcd.setCursor(4, 1); lcd.print("       ");
            }
            scrollPosition += 1;
          }
          if (cursorPosition == 0) cursorPosition = 1;
        }
        else if (newEnc < oldEnc) {
          if (cursorPosition == 0 && scrollPosition != 0) {
            lcd.clear();
            --i; --j;
            lcd.setCursor(0, 0); lcd.print(OptionMenuDir[i]); if (MDir[i] == 0) lcd.print("Left"); else lcd.print("Right");
            lcd.setCursor(0, 1); lcd.print(OptionMenuDir[j]); if (MDir[j] == 0) lcd.print("Left"); else lcd.print("Right");
            if (j == 3) {
              lcd.setCursor(4, 1); lcd.print("       ");
            }
            scrollPosition -= 1;
          }
          if (cursorPosition == 1) cursorPosition = 0;
        }
        lcd.setCursor(15, 0); lcd.print(' ');
        lcd.setCursor(15, 1); lcd.print(' ');
        lcd.setCursor(15, cursorPosition); lcd.write(2);
        oldEnc = newEnc;
      }
    }
    while (readOK() == 1) {}

    if (cursorPosition == 0 && scrollPosition == 0) {
      oldEnc = Enc.read();
      while (readOK() == 0) {
        newEnc = Enc.read();
        if (newEnc > oldEnc + 2) {
          if (MDir[0] == 0) MDir[0] = 1;
          lcd.setCursor(7, cursorPosition); lcd.print("        ");
          lcd.setCursor(7, cursorPosition); if (MDir[0] == 0) lcd.print("Left"); else lcd.print("Right");
          oldEnc = newEnc;
        }
        else if (newEnc < oldEnc - 2) {
          if (MDir[0] == 1) MDir[0] = 0;
          lcd.setCursor(7, cursorPosition); lcd.print("        ");
          lcd.setCursor(7, cursorPosition); if (MDir[0] == 0) lcd.print("Left"); else lcd.print("Right");
          oldEnc = newEnc;
        }
      }
      while (readOK() == 1) {}
      goto setDirectionLabel;
    }
    else if (cursorPosition == 1 && scrollPosition == 0 || cursorPosition == 0 && scrollPosition == 1) {
      oldEnc = Enc.read();
      while (readOK() == 0) {
        newEnc = Enc.read();
        if (newEnc > oldEnc + 2) {
          if (MDir[1] == 0) MDir[1] = 1;
          lcd.setCursor(7, cursorPosition); lcd.print("        ");
          lcd.setCursor(7, cursorPosition); if (MDir[1] == 0) lcd.print("Left"); else lcd.print("Right");
          oldEnc = newEnc;
        }
        else if (newEnc < oldEnc - 2) {
          if (MDir[1] == 1) MDir[1] = 0;
          lcd.setCursor(7, cursorPosition); lcd.print("        ");
          lcd.setCursor(7, cursorPosition); if (MDir[1] == 0) lcd.print("Left"); else lcd.print("Right");
          oldEnc = newEnc;
        }
      }
      while (readOK() == 1) {}
      goto setDirectionLabel;
    }
    else if (cursorPosition == 1 && scrollPosition == 1 || cursorPosition == 0 && scrollPosition == 2) {
      oldEnc = Enc.read();
      while (readOK() == 0) {
        newEnc = Enc.read();
        if (newEnc > oldEnc + 2) {
          if (MDir[2] == 0) MDir[2] = 1;
          lcd.setCursor(7, cursorPosition); lcd.print("        ");
          lcd.setCursor(7, cursorPosition); if (MDir[2] == 0) lcd.print("Left"); else lcd.print("Right");
          oldEnc = newEnc;
        }
        else if (newEnc < oldEnc - 2) {
          if (MDir[2] == 1) MDir[2] = 0;
          lcd.setCursor(7, cursorPosition); lcd.print("        ");
          lcd.setCursor(7, cursorPosition); if (MDir[2] == 0) lcd.print("Left"); else lcd.print("Right");
          oldEnc = newEnc;
        }
      }
      while (readOK() == 1) {}
      goto setDirectionLabel;
    }
    else if (cursorPosition == 1 && scrollPosition == 2) {
      goto OptionLabel;
    }
  }
  else if (cursorPosition == 1 && scrollPosition == 2 || cursorPosition == 0 && scrollPosition == 3) { // active
setActiveLabel:
    cursorPosition = 0;
    scrollPosition = 0;
    i = 0; j = 1;

    lcd.clear(); lcd.home();
    lcd.print(OptionMenuActive[0]); if (MActive[0] == 0) lcd.print("Off"); else lcd.print("On");
    lcd.setCursor(0, 1); lcd.print(OptionMenuActive[1]); if (MActive[1] == 0) lcd.print("Off"); else lcd.print("On");

    lcd.setCursor(15, 0); lcd.write(2);
    oldEnc = Enc.read();

    while (readOK() == 0) {
      newEnc = Enc.read();
      if (newEnc > oldEnc + EncPower || newEnc < oldEnc - EncPower) {
        if (newEnc > oldEnc) {
          if (cursorPosition == 1 && scrollPosition != 1) {
            lcd.clear();
            ++i; ++j;
            lcd.setCursor(0, 0); lcd.print(OptionMenuActive[i]); if (MActive[i] == 0) lcd.print("Off"); else lcd.print("On");
            lcd.setCursor(0, 1); lcd.print(OptionMenuActive[j]); if (MActive[j] == 0) lcd.print("Off"); else lcd.print("On");
            if (j == 2) {
              lcd.setCursor(4, 1); lcd.print("       ");
            }
            scrollPosition += 1;
          }
          if (cursorPosition == 0) cursorPosition = 1;
        }
        else if (newEnc < oldEnc) {
          if (cursorPosition == 0 && scrollPosition != 0) {
            lcd.clear();
            --i; --j;
            lcd.setCursor(0, 0); lcd.print(OptionMenuActive[i]); if (MActive[i] == 0) lcd.print("Off"); else lcd.print("On");
            lcd.setCursor(0, 1); lcd.print(OptionMenuActive[j]); if (MActive[j] == 0) lcd.print("Off"); else lcd.print("On");
            if (j == 2) {
              lcd.setCursor(4, 1); lcd.print("       ");
            }
            scrollPosition -= 1;
          }
          if (cursorPosition == 1) cursorPosition = 0;
        }
        lcd.setCursor(15, 0); lcd.print(' ');
        lcd.setCursor(15, 1); lcd.print(' ');
        lcd.setCursor(15, cursorPosition); lcd.write(2);
        oldEnc = newEnc;
      }
    }
    while (readOK() == 1) {}

    if (cursorPosition == 0 && scrollPosition == 0) {
      oldEnc = Enc.read();
      while (readOK() == 0) {
        newEnc = Enc.read();
        if (newEnc > oldEnc + 2) {
          if (MActive[0] == 0) MActive[0] = 1;
          lcd.setCursor(7, cursorPosition); lcd.print("        ");
          lcd.setCursor(7, cursorPosition); if (MActive[0] == 0) lcd.print("Off"); else lcd.print("On");
          oldEnc = newEnc;
        }
        else if (newEnc < oldEnc - 2) {
          if (MActive[0] == 1) MActive[0] = 0;
          lcd.setCursor(7, cursorPosition); lcd.print("        ");
          lcd.setCursor(7, cursorPosition); if (MActive[0] == 0) lcd.print("Off"); else lcd.print("On");
          oldEnc = newEnc;
        }
      }
      while (readOK() == 1) {}
      goto setActiveLabel;
    }
    else if (cursorPosition == 1 && scrollPosition == 0 || cursorPosition == 0 && scrollPosition == 1) {
      oldEnc = Enc.read();
      while (readOK() == 0) {
        newEnc = Enc.read();
        if (newEnc > oldEnc + 2) {
          if (MActive[1] == 0) MActive[1] = 1;
          lcd.setCursor(7, cursorPosition); lcd.print("        ");
          lcd.setCursor(7, cursorPosition); if (MActive[1] == 0) lcd.print("Off"); else lcd.print("On");
          oldEnc = newEnc;
        }
        else if (newEnc < oldEnc - 2) {
          if (MActive[1] == 1) MActive[1] = 0;
          lcd.setCursor(7, cursorPosition); lcd.print("        ");
          lcd.setCursor(7, cursorPosition); if (MActive[1] == 0) lcd.print("Off"); else lcd.print("On");
          oldEnc = newEnc;
        }
      }
      while (readOK() == 1) {}
      goto setActiveLabel;
    }
    else if (cursorPosition == 1 && scrollPosition == 1) {
      goto OptionLabel;
    }
  }
  else if (cursorPosition == 1 && scrollPosition == 3) { // exit
    returnMain = 1;
  }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void infoMenuFn() {
  lcd.clear(); lcd.home();
  lcd.print(infoMenu[0]); lcd.print(String(FirmwareVer[0]) + '.' + String(FirmwareVer[1]) + '.' + String(FirmwareVer[2]) + 'v');
  lcd.setCursor(0, 1); lcd.write(0); lcd.write(1);
  lcd.setCursor(2, 1); lcd.print(infoMenu[1]);

  while (readOK() == 0) {}
  while (readOK() == 1) {}

  returnMain = 1;
}
