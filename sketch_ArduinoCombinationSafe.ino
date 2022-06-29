/* Arduino 'Combination Safe' concept

   Components:
                - Arduino Uno
                - Green LED (x 4)
                - 10KOhm potentiometer (x 4)
                - 1kOhm resistor (x 4)
                - 220Ohm resistor
                - LCD I2C (20x4)
                - Passive Buzzer
                - Push button tactile switch
                - Some jumper wires

   Libraries:
                - LiquidCrystal_I2C library

   Created on 29 June 2022 by c010rblind3ngineer
*/

#include <LiquidCrystal_I2C.h>

int pot1, pot2, pot3, pot4;
const int buzzer = 9;
const int buttonPin = 2;

////////////////////////////////////////////////////////////////////////////////
// LEDs are for visual indication of the correct input password,              //
// for future project you could put numeric indicators on each potentiometer. //
////////////////////////////////////////////////////////////////////////////////
const int D1_LED = 7;
const int D2_LED = 6;
const int D3_LED = 5;
const int D4_LED = 4;

unsigned long firstDigitTime;
unsigned long lastDigitTime;

int timeLimit = 8000; // time limit for User to input combination
int i = 0;
int t = 0;
int load = 0;

int line;
int count;

int password[5] = {1, 2, 3, 4};  // set your own password here...

byte loadingBar[] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
};

LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup() {
  Serial.begin(9600);
  pinMode(D1_LED, OUTPUT);
  pinMode(D2_LED, OUTPUT);
  pinMode(D3_LED, OUTPUT);
  pinMode(D4_LED, OUTPUT);
  pinMode(buttonPin, INPUT);

  lcd.init();
  lcd.createChar(0, loadingBar);
}

void loop() {
  //////////////////////////////////////////////////////////////
  // There are 3 sets of 'Combination' functions:             //
  // Combination_original() ---->> pot#1, pot#2, pot#3, pot#4 //
  // Combination_one()      ---->> pot#1, pot#3, pot#2, pot#4 //
  // Combination_two()      ---->> pot#4, pot#3, pot#2, pot#1 //
  //////////////////////////////////////////////////////////////
  for (count = 0; count < 3;) {
    if (count == 0) {
      while (load < 1) {
        safeInit();
        load = 1;
      }
      Combination_original();
    }
    if (count == 1) {
      while (load < 1) {
        lcd.clear();
        lcd.print("Press button once...");
        while (digitalRead(buttonPin) != HIGH) {}
        if (digitalRead(buttonPin) == HIGH) {
          i = 0;
          t = 0;
        }
        delay(100);
        safeInit();
        load = 1;
      }
      Combination_one();
    }
    if (count == 2) {
      while (load < 1) {
        lcd.clear();
        lcd.print("Press button once...");
        while (digitalRead(buttonPin) != HIGH) {}
        if (digitalRead(buttonPin) == HIGH) {
          i = 0;
          t = 0;
        }
        delay(100);
        safeInit();
        load = 1;
      }
      Combination_two();
    }
  }
  // if User fail to input combination within time limit for ALL tries, shutdown system...
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("      Goodbye.      ");
  delay(1000);
  tone(buzzer, 2000);
  delay(100);
  noTone(buzzer);
  delay(50);
  tone(buzzer, 2000);
  delay(100);
  noTone(buzzer);
  delay(50);
  lcd.clear();
  lcd.noBacklight();
}

// Initialization & Loading screen
void safeInit() {
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("      Arduino       ");
  lcd.setCursor(0, 1);
  lcd.print("  Combination Safe  ");
  delay(5000);
  lcd.clear();
  lcd.print("      Loading...    ");
  delay(1000);
  lcd.setCursor(0, 1);
  for (line = 0; line < 20; line++) {
    lcd.write(0);
    delay(200);
  }
  delay(250);
  tone(buzzer, 2000);
  delay(100);
  noTone(buzzer);
  delay(50);
  tone(buzzer, 2000);
  delay(100);
  noTone(buzzer);
  delay(50);
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print(" Enter combination: ");
}

///////////////////////////Combination # Original/////////////////////////////
// Turn potentiometers in this sequence:                                    //
// First  - pot #1                                                          //
// Second - pot #2                                                          //
// Third  - pot #3                                                          //
// Fourth - pot #4                                                          //
// *NOTE*: You can set your own combination sequence by changing the        //
//         arrangement of pot1_digact - pot4_digact in the 'if'             //
//         statements.                                                      //
//////////////////////////////////////////////////////////////////////////////
void Combination_original() {
  while (i < 4) {
    if (pot1_digact(password[0]) == 1) {       // pot #1
      digitalWrite(D1_LED, HIGH);       // turn ON 1st LED
      // capture time taken for the user to input the FIRST digit
      while (t < 1) {
        firstDigitTime = millis();
        Serial.print("First Digit Time: ");
        Serial.println(firstDigitTime);
        t = 1;
      }
      i = 1;
      if (pot2_digact(password[1]) == 1) {     // pot #2
        digitalWrite(D2_LED, HIGH);      // turn ON 2nd LED
        i = 2;
        if (pot3_digact(password[2]) == 1) {   // pot #3
          digitalWrite(D3_LED, HIGH);   // turn ON 3rd LED
          i = 3;
          if (pot4_digact(password[3]) == 1) { // pot #4
            digitalWrite(D4_LED, HIGH); // turn ON 4th LED
            t = 0;      // re-enable 'timer'
            // capture time taken for the user to input the LAST digit
            while (t < 1) {
              lastDigitTime = millis();
              Serial.print("Last Digit Time: ");
              Serial.println(lastDigitTime);
              t = 1;
            }
            // Compare start and end timing if User enter password within the time limit
            if (lastDigitTime - firstDigitTime < timeLimit) {
              //ACCESS GRANTED HERE//
              accessGranted();
            }
            else {
              //ACCCESS DENIED HERE//
              accessDenied();
              count++;
              load = 0;
            }
            delay(1000);
            digitalWrite(D1_LED, LOW);
            digitalWrite(D3_LED, LOW);
            digitalWrite(D2_LED, LOW);
            digitalWrite(D4_LED, LOW);
            i = 4;
          }
          else {
            digitalWrite(D4_LED, LOW);
          }
        }
        else {
          digitalWrite(D3_LED, LOW);
        }
      }
      else {
        digitalWrite(D2_LED, LOW);
      }
    }
    else {
      digitalWrite(D1_LED, LOW);
    }
  }
}

///////////////////////////Combination # ONE//////////////////////////////////
// Turn potentiometers in this sequence:                                    //
// First  - pot #1                                                          //
// Second - pot #3                                                          //
// Third  - pot #2                                                          //
// Fourth - pot #4                                                          //
// *NOTE*: You can set your own combination sequence by changing the        //
//         arrangement of pot1_digact - pot4_digact in the 'if'             //
//         statements.                                                      //
//////////////////////////////////////////////////////////////////////////////
void Combination_one() {
  while (i < 4) {
    if (pot1_digact(password[0]) == 1) {       // pot #1
      digitalWrite(D1_LED, HIGH);       // turn ON 1st LED
      // capture time taken for the user to input the FIRST digit
      while (t < 1) {
        firstDigitTime = millis();
        Serial.print("First Digit Time: ");
        Serial.println(firstDigitTime);
        t = 1;
      }
      i = 1;
      if (pot3_digact(password[1]) == 1) {     // pot #3
        digitalWrite(D2_LED, HIGH);     // turn ON 2nd LED
        i = 2;
        if (pot2_digact(password[2]) == 1) {   // pot #2
          digitalWrite(D3_LED, HIGH);   // turn ON 3rd LED
          i = 3;
          if (pot4_digact(password[3]) == 1) { // pot #4
            digitalWrite(D4_LED, HIGH); // turn ON 4th LED
            t = 0;      // re-enable 'timer'
            // capture time taken for the user to input the LAST digit
            while (t < 1) {
              lastDigitTime = millis();
              Serial.print("Last Digit Time: ");
              Serial.println(lastDigitTime);
              t = 1;
            }
            // Compare start and end timing if User enter password within the time limit
            if (lastDigitTime - firstDigitTime < timeLimit) {
              //ACCESS GRANTED HERE//
              accessGranted();
            }
            else {
              //ACCESS DENIED HERE//
              accessDenied();
              count++;
              load = 0;
            }
            delay(1000);
            digitalWrite(D1_LED, LOW);
            digitalWrite(D3_LED, LOW);
            digitalWrite(D2_LED, LOW);
            digitalWrite(D4_LED, LOW);
            i = 4;
          }
          else {
            digitalWrite(D4_LED, LOW);
          }
        }
        else {
          digitalWrite(D3_LED, LOW);
        }
      }
      else {
        digitalWrite(D2_LED, LOW);
      }
    }
    else {
      digitalWrite(D1_LED, LOW);
    }
  }
}

///////////////////////////Combination # TWO//////////////////////////////////
// Turn potentiometers in this sequence:                                    //
// First  - pot #4                                                          //
// Second - pot #3                                                          //
// Third  - pot #2                                                          //
// Fourth - pot #1                                                          //
// *NOTE*: You can set your own combination sequence by changing the        //
//         arrangement of pot1_digact - pot4_digact in the 'if'             //
//         statements.                                                      //
//////////////////////////////////////////////////////////////////////////////
void Combination_two() {
  while (i < 4) {
    if (pot4_digact(password[0]) == 1) {       // pot #4
      digitalWrite(D1_LED, HIGH);       // turn ON 1st LED
      // capture time taken for the user to input the FIRST digit
      while (t < 1) {
        firstDigitTime = millis();
        Serial.print("First Digit Time: ");
        Serial.println(firstDigitTime);
        t = 1;
      }
      i = 1;
      if (pot3_digact(password[1]) == 1) {     // pot #3
        digitalWrite(D2_LED, HIGH);     // turn ON 2nd LED
        i = 2;
        if (pot2_digact(password[2]) == 1) {   // pot #2
          digitalWrite(D3_LED, HIGH);   // turn ON 3rd LED
          i = 3;
          if (pot1_digact(password[3]) == 1) { // pot #1
            digitalWrite(D4_LED, HIGH); // turn ON 4th LED
            t = 0;      // re-enable 'timer'
            // capture time taken for the user to input the LAST digit
            while (t < 1) {
              lastDigitTime = millis();
              Serial.print("Last Digit Time: ");
              Serial.println(lastDigitTime);
              t = 1;
            }
            // Compare start and end timing if User enter password within the time limit
            if (lastDigitTime - firstDigitTime < timeLimit) {
              //ACCESS GRANTED HERE//
              accessGranted();
            }
            else {
              //ACCESS DENIED HERE//
              accessDenied();
              count++;
            }
            delay(1000);
            digitalWrite(D1_LED, LOW);
            digitalWrite(D3_LED, LOW);
            digitalWrite(D2_LED, LOW);
            digitalWrite(D4_LED, LOW);
            i = 4;
          }
          else {
            digitalWrite(D4_LED, LOW);
          }
        }
        else {
          digitalWrite(D3_LED, LOW);
        }
      }
      else {
        digitalWrite(D2_LED, LOW);
      }
    }
    else {
      digitalWrite(D1_LED, LOW);
    }
  }
}

////////////////////////////////////////////////////////////////////////////
// This function turns the 1st potentiometer into a rotating dial (1 - 9) //
////////////////////////////////////////////////////////////////////////////
int pot1_digact(int val) {
  pot1 = analogRead(A0);
  // for 1st digit of the password
  switch (val) {
    case 1:
      if ((val == 1) && pot1 >= 100 && pot1 <= 199) {
        return 1;
      }
      else {
        return 0;
      }
      break;
    case 2:
      if ((val == 2) && pot1 >= 200 && pot1 <= 399) {
        return 1;
      }
      else {
        return 0;
      }
      break;
    case 3:
      if ((val == 3) && pot1 >= 300 && pot1 <= 399) {
        return 1;
      }
      else {
        return 0;
      }
      break;
    case 4:
      if ((val == 4) && pot1 >= 400 && pot1 <= 499) {
        return 1;
      }
      else {
        return 0;
      }
      break;
    case 5:
      if ((val == 5) && pot1 >= 500 && pot1 <= 599) {
        return 1;
      }
      else {
        return 0;
      }
      break;
    case 6:
      if ((val == 6) && pot1 >= 600 && pot1 <= 699) {
        return 1;
      }
      else {
        return 0;
      }
      break;
    case 7:
      if ((val == 7) && pot1 >= 700 && pot1 <= 799) {
        return 1;
      }
      else {
        return 0;
      }
      break;
    case 8:
      if ((val == 8) && pot1 >= 800 && pot1 <= 899) {
        return 1;
      }
      else {
        return 0;
      }
      break;
    case 9:
      if ((val == 9) && pot1 >= 900) {
        return 1;
      }
      else {
        return 0;
      }
      break;
  }
}

////////////////////////////////////////////////////////////////////////////
// This function turns the 2nd potentiometer into a rotating dial (1 - 9) //
////////////////////////////////////////////////////////////////////////////
int pot2_digact(int val) {
  pot2 = analogRead(A1);
  // for 2nd digit of the password
  switch (val) {
    case 1:
      if ((val == 1) && pot2 >= 100 && pot2 <= 199) {
        return 1;
      }
      else {
        return 0;
      }
      break;
    case 2:
      if ((val == 2) && pot2 >= 200 && pot2 <= 399) {
        return 1;
      }
      else {
        return 0;
      }
      break;
    case 3:
      if ((val == 3) && pot2 >= 300 && pot2 <= 399) {
        return 1;
      }
      else {
        return 0;
      }
      break;
    case 4:
      if ((val == 4) && pot2 >= 400 && pot2 <= 499) {
        return 1;
      }
      else {
        return 0;
      }
      break;
    case 5:
      if ((val == 5) && pot2 >= 500 && pot2 <= 599) {
        return 1;
      }
      else {
        return 0;
      }
      break;
    case 6:
      if ((val == 6) && pot2 >= 600 && pot2 <= 699) {
        return 1;
      }
      else {
        return 0;
      }
      break;
    case 7:
      if ((val == 7) && pot2 >= 700 && pot2 <= 799) {
        return 1;
      }
      else {
        return 0;
      }
      break;
    case 8:
      if ((val == 8) && pot2 >= 800 && pot2 <= 899) {
        return 1;
      }
      else {
        return 0;
      }
      break;
    case 9:
      if ((val == 9) && pot2 >= 900) {
        return 1;
      }
      else {
        return 0;
      }
      break;
  }
}

////////////////////////////////////////////////////////////////////////////
// This function turns the 3rd potentiometer into a rotating dial (1 - 9) //
////////////////////////////////////////////////////////////////////////////
int pot3_digact(int val) {
  pot3 = analogRead(A2);
  // for 3rd digit of the password
  switch (val) {
    case 1:
      if ((val == 1) && pot3 >= 100 && pot3 <= 199) {
        return 1;
      }
      else {
        return 0;
      }
      break;
    case 2:
      if ((val == 2) && pot3 >= 200 && pot3 <= 399) {
        return 1;
      }
      else {
        return 0;
      }
      break;
    case 3:
      if ((val == 3) && pot3 >= 300 && pot3 <= 399) {
        return 1;
      }
      else {
        return 0;
      }
      break;
    case 4:
      if ((val == 4) && pot3 >= 400 && pot3 <= 499) {
        return 1;
      }
      else {
        return 0;
      }
      break;
    case 5:
      if ((val == 5) && pot3 >= 500 && pot3 <= 599) {
        return 1;
      }
      else {
        return 0;
      }
      break;
    case 6:
      if ((val == 6) && pot3 >= 600 && pot3 <= 699) {
        return 1;
      }
      else {
        return 0;
      }
      break;
    case 7:
      if ((val == 7) && pot3 >= 700 && pot3 <= 799) {
        return 1;
      }
      else {
        return 0;
      }
      break;
    case 8:
      if ((val == 8) && pot3 >= 800 && pot3 <= 899) {
        return 1;
      }
      else {
        return 0;
      }
      break;
    case 9:
      if ((val == 9) && pot3 >= 900) {
        return 1;
      }
      else {
        return 0;
      }
      break;
  }
}

////////////////////////////////////////////////////////////////////////////
// This function turns the 4th potentiometer into a rotating dial (1 - 9) //
////////////////////////////////////////////////////////////////////////////
int pot4_digact(int val) {
  pot4 = analogRead(A3);
  // for 4th digit of the password
  switch (val) {
    case 1:
      if ((val == 1) && pot4 >= 100 && pot4 <= 199) {
        return 1;
      }
      else {
        return 0;
      }
      break;
    case 2:
      if ((val == 2) && pot4 >= 200 && pot4 <= 399) {
        return 1;
      }
      else {
        return 0;
      }
      break;
    case 3:
      if ((val == 3) && pot4 >= 300 && pot4 <= 399) {
        return 1;
      }
      else {
        return 0;
      }
      break;
    case 4:
      if ((val == 4) && pot4 >= 400 && pot4 <= 499) {
        return 1;
      }
      else {
        return 0;
      }
      break;
    case 5:
      if ((val == 5) && pot4 >= 500 && pot4 <= 599) {
        return 1;
      }
      else {
        return 0;
      }
      break;
    case 6:
      if ((val == 6) && pot4 >= 600 && pot4 <= 699) {
        return 1;
      }
      else {
        return 0;
      }
      break;
    case 7:
      if ((val == 7) && pot4 >= 700 && pot4 <= 799) {
        return 1;
      }
      else {
        return 0;
      }
      break;
    case 8:
      if ((val == 8) && pot4 >= 800 && pot4 <= 899) {
        return 1;
      }
      else {
        return 0;
      }
      break;
    case 9:
      if ((val == 9) && pot4 >= 900) {
        return 1;
      }
      else {
        return 0;
      }
      break;
  }
}

void accessGranted() {
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("   Access Granted   ");
  digitalWrite(D1_LED, LOW);
  digitalWrite(D3_LED, LOW);
  digitalWrite(D2_LED, LOW);
  digitalWrite(D4_LED, LOW);
  tone(buzzer, 2000);
  digitalWrite(D1_LED, HIGH);
  digitalWrite(D3_LED, HIGH);
  digitalWrite(D2_LED, HIGH);
  digitalWrite(D4_LED, HIGH);
  delay(100);
  digitalWrite(D1_LED, LOW);
  digitalWrite(D3_LED, LOW);
  digitalWrite(D2_LED, LOW);
  digitalWrite(D4_LED, LOW);
  noTone(buzzer);
  delay(100);
  digitalWrite(D1_LED, HIGH);
  digitalWrite(D3_LED, HIGH);
  digitalWrite(D2_LED, HIGH);
  digitalWrite(D4_LED, HIGH);
  tone(buzzer, 2000);
  delay(100);
  noTone(buzzer);
  delay(100);
}

void accessDenied() {
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("   Access Denied    ");
  tone(buzzer, 1000);
  delay(250);
  noTone(buzzer);
  delay(100);
  tone(buzzer, 1000);
  delay(250);
  noTone(buzzer);
  delay(100);
  tone(buzzer, 1000);
  delay(250);
  noTone(buzzer);
  delay(100);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("    Please reset    ");
  lcd.setCursor(0, 1);
  lcd.print("ALL FOUR knobs to 0");
  lcd.setCursor(0, 2);
  for (line = 0; line < 20; line++) {
    lcd.write(0);
    delay(1000);
  }
}
