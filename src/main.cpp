#include <Arduino.h>
// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tues", "Wed", "Thur", "Fri", "Sat"};
//PINS
//OUTPUT TO OTHER BOARD
const int dataPinSEG = 7;
const int latchPinSEG = 8;
const int clockPinSEG = 9;
// OUTPUT TO LEDS
const int dataPinLED_1 = 4;
const int latchPinLED_1 = 5;
const int clockPinLED_1 = 6;
// INPUT TO NANO
const int buttonPin = 3;
const int PIR_Pin = 2;

// ANALOG PINS
const int dipPin = 3;
const int photoResPin= 2;
const int varResPin = 6;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 7 SEGMENT NUMBERS FOR MOST SIGNIFANCT FIRST
byte one =    B00000110;
byte two =    B11101100;
byte three =  B01101110;
byte four =   B00110110;
byte five =   B01111010;
byte six =    B11111010;
byte seven =  B00001110;
byte eight =  B11111110;
byte nine =   B00111110;
byte zero =   B11011110;

byte letterA = B10111110;
byte letterB = B11110010;
byte letterC = B11011000;
byte letterD = B11100110;
byte letterE = B11111000;
byte letterF = B10111000;
byte letterG = B01111110;
byte letterH = B10110110;
byte letterI= B10010000;
byte letterJ = B01000110;
byte letterL = B11010000;

byte letterN = B10100010;
byte letterO= B11011110;
byte letterP = B10111100;
byte letterQ = B00111110;
byte letterR = B10100000;
byte letterS = B01111010;
byte letterT = B11110000;
byte letterU= B11010110;
byte letterY = B01110110;
byte letterZ = B11101100;

byte letterX= B00000000;
byte letterDash= B00100000;

//start screen stuff
byte ani_L1 =  B01000000;
byte ani_L2 = B11000000;
byte ani_L3 = B11010000;
byte ani_L4 = B11011000;
byte ani_L5 = B10011000;
byte ani_L6 = B01011000;
byte ani_L7 = B11001000;
byte ani_L8 = B11010000;
byte ani_L10 = B10011000;
byte ani_L11 = B00011000;
byte ani_L12 = B00001000;
byte ani_L13 = B00000000;

byte ani_R1 = B00001000;
byte ani_R2 =B00001100;
byte ani_R3 =B00001110;
byte ani_R4 =B01001110;
byte ani_R5 =B01000110;
byte ani_R6 =B01001010;
byte ani_R7 =B01001100;
byte ani_R8 =B00001110;
byte ani_R10 =B01000110;
byte ani_R11 =B01000010;
byte ani_R12 =B01000000;
byte ani_R13 =B00000000;

//  LED ARRAGEMENTS AND COULOURS
byte greenOne = B10000101;
byte greenTwo = B00100100;

byte yellowOne = B01010010;
byte yellowTwo = B10010010;

byte redOne = B00101000;
byte redTwo = B01001001;

byte greenYellowOne =B11010111;
byte greenYellowTwo = B10110110;
byte greenRedOne = B10101101;
byte greenRedTwo = B01101101;

byte onBit = B11111111;
byte offBit = B00000000;

byte inputBit = B00000000;

// TIME BETWEEN PRINT++++++++++++++++++++++++++++++++++++++++++++++++++++++
int time_5bit = 32;
int time_6bit = 64;
int time_7bit = 128;
int time_8bit = 256;
int time_9bit = 512;
int time_10bit = 1024;

// Variables
word number_one;
word number_two;
word number_three;

int stateOne = 0;
int stateTwo = 0;
int stateThree = 0;
int stateCounter = 0;
int modeCounter = 0;
int bitCounter = 0;

bool mode_1 = false;
bool mode_2 = false;
bool mode_3 = false;
bool mode_4 = false;
bool mode_5 = false;

int counter_1 = 0;
int counter_2 = 0;
int counter_3 = 0;
// ############################################################################
int dipRead;
int dipValue;
int dipMap;
int lightSense;

byte bitState;
int hertz;

//TIME Variables
int thisYear ;
int thisMonth ;
int thisDay ;
int thisHour ;
int thisMinute ;
int thisSecond ;


int monthTen;
int monthUnit;

int dayTen;
int dayUnit;

int hourTen;
int hourUnit;

int minuteTen;
int minuteUnit;

int secondsTen;
int secondsUnit;

word MTen;
word MUnt;
word dTen;
word dUnt;
word hTen;
word hUnt;
word mTen;
word mUnt;
word sTen;
word sUnt;


// ############################################################################
//    SUB ROUTINES
// ############################################################################
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

void LED_ONE(byte insertByte_0, byte insertByte_1)
{
  digitalWrite(latchPinLED_1, LOW);
  shiftOut(dataPinLED_1, clockPinLED_1, LSBFIRST, insertByte_0);
  shiftOut(dataPinLED_1, clockPinLED_1, LSBFIRST, insertByte_1);
  digitalWrite(latchPinLED_1, HIGH);
}
void LED_greenON()
{
  LED_ONE(greenOne, greenTwo);
}
void LED_yellowON()
{
  LED_ONE(yellowOne, yellowTwo);
}
void LED_redON()
{
  LED_ONE(redOne, redTwo);
}
void LED_greenYellowON()
{
  LED_ONE(greenYellowOne, greenYellowTwo);
}
void LED_greenRedON() // turn both green and red leds on
{
  LED_ONE(greenRedOne, greenRedTwo);
}
void LED_clear() // turn all LEDS off
{
  LED_ONE(offBit, offBit);
}
void printInputNumber(byte insertOne, byte insertTwo, byte insertThree) // varibles are switched around this so unit, ten, hundred
{
  digitalWrite(latchPinSEG, LOW);
  shiftOut(dataPinSEG, clockPinSEG, LSBFIRST, insertOne);
  shiftOut(dataPinSEG, clockPinSEG, LSBFIRST, insertTwo);
  shiftOut(dataPinSEG, clockPinSEG, LSBFIRST, insertThree);
  digitalWrite(latchPinSEG, HIGH);
}
void printInputWord(byte insertOne, byte insertTwo, byte insertThree) //places the varibles on which they are written prints three letters to 7 segment
{
  digitalWrite(latchPinSEG, LOW);
  shiftOut(dataPinSEG, clockPinSEG, LSBFIRST, insertThree);
  shiftOut(dataPinSEG, clockPinSEG, LSBFIRST, insertTwo);
  shiftOut(dataPinSEG, clockPinSEG, LSBFIRST, insertOne);
  digitalWrite(latchPinSEG, HIGH);
}

void inputBitSeg(byte insertOne, byte insertTwo, byte insertThree) //places the varibles on which they are written prints three letters to 7 segment
{
  digitalWrite(latchPinSEG, LOW);
  shiftOut(dataPinSEG, clockPinSEG, LSBFIRST, insertThree);
  shiftOut(dataPinSEG, clockPinSEG, LSBFIRST, insertTwo);
  shiftOut(dataPinSEG, clockPinSEG, LSBFIRST, insertOne);
  digitalWrite(latchPinSEG, HIGH);
}
void countNumber() // count using stateOne, Two and Three, gets to 10 and restsarts.
{
  stateOne++;
  if (stateOne == 10)
  {
    stateOne = 0;
    stateTwo++;
  }
  if (stateTwo == 10)
  {
    stateTwo = 0;
    stateThree++;
  }
  if (stateThree == 10)
  {
    stateThree = 0;
  }
}
void sortNumber() //takes the variable stateOne, stateTwo and stateThree and returns the number_one, number_two, number_three variables
{
  switch (stateOne) {
    case 0:
    number_one = zero;
    break;
    case 1:
    number_one = one;
    break;
    case 2:
    number_one = two;
    break;
    case 3:
    number_one = three;
    break;
    case 4:
    number_one = four;
    break;
    case 5:
    number_one = five;
    break;
    case 6:
    number_one = six;
    break;
    case 7:
    number_one = seven;
    break;
    case 8:
    number_one = eight;
    break;
    case 9:
    number_one = nine;
    break;
  }
  switch (stateTwo) {
    case 0:
    number_two = zero;
    break;
    case 1:
    number_two = one;
    break;
    case 2:
    number_two = two;
    break;
    case 3:
    number_two = three;
    break;
    case 4:
    number_two = four;
    break;
    case 5:
    number_two = five;
    break;
    case 6:
    number_two = six;
    break;
    case 7:
    number_two = seven;
    break;
    case 8:
    number_two = eight;
    break;
    case 9:
    number_two = nine;
    break;
  }
  switch (stateThree) {
    case 0:
    number_three = zero;
    break;
    case 1:
    number_three = one;
    break;
    case 2:
    number_three = two;
    break;
    case 3:
    number_three = three;
    break;
    case 4:
    number_three = four;
    break;
    case 5:
    number_three = five;
    break;
    case 6:
    number_three = six;
    break;
    case 7:
    number_three = seven;
    break;
    case 8:
    number_three = eight;
    break;
    case 9:
    number_three = nine;
    break;
  }
}
void printNumber() //prints to the 7 segment using the number_one, two and three varibles.
{
  digitalWrite(latchPinSEG, LOW);
  shiftOut(dataPinSEG, clockPinSEG, LSBFIRST, number_one);
  shiftOut(dataPinSEG, clockPinSEG, LSBFIRST, number_two);
  shiftOut(dataPinSEG, clockPinSEG, LSBFIRST, number_three);
  digitalWrite(latchPinSEG, HIGH);
}
void serialPrintTime()  //print the current time
{
  DateTime now = rtc.now();
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
}

void dipSwitchReading() //take a reading from the 4 pin dip switch
{
  dipRead = analogRead(dipPin);
  dipMap = map(dipRead,  0, 1024, 0, 100);

  if(dipRead < 10)
  {
    dipValue = 0;
  }
  else if (dipRead > 230 && dipRead < 250)
  {
    dipValue = 1;
  }
  else if (dipRead > 500 && dipRead < 520)
  {
    dipValue = 2;
  }
  else if (dipRead > 770 && dipRead < 790)
  {
    dipValue = 3;
  }
  else if (dipRead > 1010)
  {
    dipValue = 4;
  }
  Serial.print("ANG - ");
  Serial.print(dipRead);
  Serial.print("\t IFS-");
  Serial.print(dipValue);
  Serial.print("\t MAP-");
  Serial.println(dipMap);
}
void hoursTillXmas() //print the hours until christmas
{

  DateTime now = rtc.now();
  long unixTimeNow = now.unixtime();
  long unixTimeXmas = 1545696000;
  long unixTimeDelta = unixTimeXmas - unixTimeNow;
  long timeHours = unixTimeDelta / 3600L;

  // Serial.print(unixTimeXmas);
  // Serial.print(" - ");
  // Serial.print(unixTimeNow);
  // Serial.print(" = ");
  // Serial.print(unixTimeDelta);
  // Serial.print(" -> hours:");
  // Serial.println(timeHours);

  int hoursLeft = int(timeHours);

  int hundreds = hoursLeft/ 100;

  // Serial.print("Hundreds:");
  // Serial.println(hundreds);

  int tens = timeHours - (hundreds*100);

  tens = tens / 10;

  // Serial.print("Tens:");
  // Serial.println(tens);

  int units = timeHours - ((hundreds * 100) + (tens * 10 )); //get the units of the time

  // Serial.print("Units:");
  // Serial.println(units);

  stateOne = units;
  stateTwo = tens;
  stateThree = hundreds;

  sortNumber();
  printNumber();
}
void sendFirstThree(byte insertLetter_0, byte insertLetter_1, byte insertLetter_2 )
{
  printInputWord(letterX, letterX, letterX);

  printInputWord(letterX, letterX, insertLetter_0);

  printInputWord(letterX, insertLetter_0, insertLetter_1);

  printInputWord(insertLetter_0, insertLetter_1, insertLetter_2);
}
void sendWordThree(byte insertLetter_3, byte insertLetter_4, byte insertLetter_5 )
{
  printInputWord(insertLetter_3, insertLetter_4, insertLetter_5);

}
void sendLastThree(byte insertLetter_6, byte insertLetter_7, byte insertLetter_8 )
{
  printInputWord(insertLetter_6, insertLetter_7, insertLetter_8);

  printInputWord(insertLetter_7, insertLetter_8, letterX);

  printInputWord(insertLetter_8, letterX, letterX);

  printInputWord(letterX, letterX, letterX);
}

void sendTime(word insertT_0, word insertT_1, word insertT_2 )
{
  digitalWrite(latchPinSEG, LOW);
  shiftOut(dataPinSEG, clockPinSEG, LSBFIRST, insertT_2);
  shiftOut(dataPinSEG, clockPinSEG, LSBFIRST, insertT_1);
  shiftOut(dataPinSEG, clockPinSEG, LSBFIRST, insertT_0);
  digitalWrite(latchPinSEG, HIGH);
}

void getCurrentTime()
{
  DateTime now = rtc.now();

  thisYear = (now.year());
  thisMonth = (now.month());
  thisDay = (now.day());
  thisHour = (now.hour());
  thisMinute =(now.minute());
  thisSecond = (now.second());

  Serial.print(thisYear);
  Serial.print('/');
  Serial.print(thisMonth);
  Serial.print('/');
  Serial.print(thisDay);
  Serial.print(' ');
  Serial.print(thisHour);
  Serial.print(':');
  Serial.print(thisMinute);
  Serial.print(':');
  Serial.print(thisSecond);
  Serial.println();

}
void printCurretTime()
{
  getCurrentTime();

  if (thisMonth >= 9)  //month into tens and units less than 9
  {
    monthTen = thisMonth / 10;
    monthUnit = thisMonth - (monthTen * 10);
  }
  if (thisMonth <= 10) //month into tens and units greater than 0
  {
    monthTen = 0;
    monthUnit = thisMonth;
  }

  if (thisDay >= 9)  //day into tens and units less than 9
  {
    dayTen = thisDay / 10;
    dayUnit = thisDay - (dayTen * 10);
  }
  if (thisDay <= 10) //day into tens and units greater than 0
  {
    dayTen = 0;
    dayUnit = thisDay;
  }

  if (thisHour >= 9)  //day into tens and units less than 9
  {
    hourTen = thisHour / 10;
    hourUnit = thisHour - (thisHour * 10);
  }
  if (thisHour <= 10)//day into tens and units greater than 0
  {
    hourTen = 0;
    hourUnit = thisHour;
  }

  if (thisMinute < 10)
  {
    minuteTen = 0;
    minuteUnit = thisMinute;
  }
  if (thisMinute > 9)
  {
    minuteTen = thisMinute / 10;
    minuteUnit = thisMinute - (minuteTen * 10);
  }

  if (thisSecond < 10)
  {
    secondsTen = 0;
    secondsUnit = thisSecond;
  }
  if (thisSecond > 9)
  {
    secondsTen = thisMinute / 10;
    secondsUnit = thisSecond - (secondsTen * 10);
  }

  // =------------------------------------------------------------------

  switch (monthTen)
  {
    case 0:
    MTen = zero;
    break;
    case 1:
    MTen = one;
    break;
    case 2:
    MTen = two;
    break;
    case 3:
    MTen = three;
    break;
    case 4:
    MTen = four;
    break;
    case 5:
    MTen = five;
    break;
    case 6:
    MTen = six;
    break;
    case 7:
    MTen = seven;
    break;
    case 8:
    MTen = eight;
    break;
    case 9:
    MTen = nine;
    break;
  }
  switch (monthUnit)
  {
    case 0:
    MUnt = zero;
    break;
    case 1:
    MUnt = one;
    break;
    case 2:
    MUnt = two;
    break;
    case 3:
    MUnt = three;
    break;
    case 4:
    MUnt = four;
    break;
    case 5:
    MUnt = five;
    break;
    case 6:
    MUnt = six;
    break;
    case 7:
    MUnt = seven;
    break;
    case 8:
    MUnt = eight;
    break;
    case 9:
    MUnt = nine;
    break;
  }

  switch (dayTen)
  {
    case 0:
    dTen = zero;
    break;
    case 1:
    dTen = one;
    break;
    case 2:
    dTen = two;
    break;
    case 3:
    dTen = three;
    break;
    case 4:
    dTen = four;
    break;
    case 5:
    dTen = five;
    break;
    case 6:
    dTen = six;
    break;
    case 7:
    dTen = seven;
    break;
    case 8:
    dTen = eight;
    break;
    case 9:
    dTen = nine;
    break;
  }
  switch (dayUnit)
  {
    case 0:
    dUnt = zero;
    break;
    case 1:
    dUnt = one;
    break;
    case 2:
    dUnt = two;
    break;
    case 3:
    dUnt = three;
    break;
    case 4:
    dUnt = four;
    break;
    case 5:
    dUnt = five;
    break;
    case 6:
    dUnt = six;
    break;
    case 7:
    dUnt = seven;
    break;
    case 8:
    dUnt = eight;
    break;
    case 9:
    dUnt = nine;
    break;
  }

  switch (hourTen)
  {
    case 0:
    hTen = zero;
    break;
    case 1:
    hTen = one;
    break;
    case 2:
    hTen = two;
    break;
    case 3:
    hTen = three;
    break;
    case 4:
    hTen = four;
    break;
    case 5:
    hTen = five;
    break;
    case 6:
    hTen = six;
    break;
    case 7:
    hTen = seven;
    break;
    case 8:
    hTen = eight;
    break;
    case 9:
    hTen = nine;
    break;
  }
  switch (hourUnit)
  {
    case 0:
    hUnt = zero;
    break;
    case 1:
    hUnt = one;
    break;
    case 2:
    hUnt = two;
    break;
    case 3:
    hUnt = three;
    break;
    case 4:
    hUnt = four;
    break;
    case 5:
    hUnt = five;
    break;
    case 6:
    hUnt = six;
    break;
    case 7:
    hUnt = seven;
    break;
    case 8:
    hUnt = eight;
    break;
    case 9:
    hUnt = nine;
    break;
  }

  switch (minuteTen)
  {
    case 0:
    mTen = zero;
    break;
    case 1:
    mTen = one;
    break;
    case 2:
    mTen = two;
    break;
    case 3:
    mTen = three;
    break;
    case 4:
    mTen = four;
    break;
    case 5:
    mTen = five;
    break;
    case 6:
    mTen = six;
    break;
    case 7:
    mTen = seven;
    break;
    case 8:
    mTen = eight;
    break;
    case 9:
    mTen = nine;
    break;
  }
  switch (minuteUnit)
  {
    case 0:
    mUnt = zero;
    break;
    case 1:
    mUnt = one;
    break;
    case 2:
    mUnt = two;
    break;
    case 3:
    mUnt = three;
    break;
    case 4:
    mUnt = four;
    break;
    case 5:
    mUnt = five;
    break;
    case 6:
    mUnt = six;
    break;
    case 7:
    mUnt = seven;
    break;
    case 8:
    mUnt = eight;
    break;
    case 9:
    mUnt = nine;
    break;
  }

  switch (secondsTen)
  {
    case 0:
    sTen = zero;
    break;
    case 1:
    sTen = one;
    break;
    case 2:
    sTen = two;
    break;
    case 3:
    sTen = three;
    break;
    case 4:
    sTen = four;
    break;
    case 5:
    sTen = five;
    break;
    case 6:
    sTen = six;
    break;
    case 7:
    sTen = seven;
    break;
    case 8:
    sTen = eight;
    break;
    case 9:
    sTen = nine;
    break;
  }
  switch (secondsUnit)
  {
    case 0:
    sUnt = zero;
    break;
    case 1:
    sUnt = one;
    break;
    case 2:
    sUnt = two;
    break;
    case 3:
    sUnt = three;
    break;
    case 4:
    sUnt = four;
    break;
    case 5:
    sUnt = five;
    break;
    case 6:
    sUnt = six;
    break;
    case 7:
    sUnt = seven;
    break;
    case 8:
    sUnt = eight;
    break;
    case 9:
    sUnt = nine;
    break;
  }




  int prints = 256;
  byte dash = letterDash;
  byte non = letterX;


  sendTime(non, non, non);
  delay(prints);
  LED_greenON();
  sendTime(non, non, hTen);
  delay(prints);
  LED_yellowON();
  sendTime(non, hTen, hUnt);
  delay(prints);
  LED_redON();
  sendTime(hTen, hUnt, dash);
  delay(prints);
    LED_greenON();
  sendTime(hUnt, dash, mTen);
  delay(prints);
    LED_yellowON();
  sendTime(dash, mTen, mUnt);
  delay(prints);
    LED_redON();
  sendTime(mTen, mUnt, non);
  delay(prints);
    LED_greenON();
  sendTime(mUnt, non, non);
  delay(prints);
      LED_yellowON();
  sendTime(non, non, non);
  delay(prints);
  LED_redON();
}


void startRoutine()
{
  Serial.println("");
  Serial.println("");
  Serial.println("");
  Serial.println("");
  Serial.println("################################");
  Serial.println("################################");
  Serial.println("RTC_CLK \t | CHECK |");
  //Check if the clock lost power if it did it will be reset.
  if (! rtc.begin())
  {
    Serial.println("RTC_CLK | NOT FOUND |");
    while (1);
  }
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  delay(time_8bit);
  Serial.println("RTC_CLK \t| READY |");
  delay(time_8bit);
  Serial.print(">>STARTING");
  for (int x = 0; x<6;x++)
  {
    Serial.print(" .");
    delay(time_6bit);
  }
  Serial.println("\t|");
  Serial.println("################################");
  Serial.println("--------------------------------");
  Serial.println("\t      XMAS      ");
  Serial.println("\t      TREE      ");
  Serial.println("--------------------------------");
  Serial.println("################################");
  Serial.println("\t       v2       ");
  Serial.println("--------------------------------");
  delay(time_7bit);
  //Print Out Starting Information.
  Serial.println  ("\t      TIME      ");
  Serial.print  ("\t");
  serialPrintTime();
  Serial.println("--------------------------------");
  delay(time_7bit);

  Serial.print(">>SHFT_REG_7SEG ");


  printInputWord(offBit, letterH, offBit);
  delay(time_5bit);
  printInputWord(ani_L1, letterH,ani_R1 );
  delay(time_5bit);
  printInputWord(ani_L2, letterH, ani_R2);
  delay(time_5bit);
  printInputWord(ani_L3, letterH, ani_R3);
  delay(time_5bit);
  printInputWord(ani_L4, letterH, ani_R4);
  delay(time_5bit);
  printInputWord(ani_L5, letterH, ani_R5);
  delay(time_5bit);
  printInputWord(ani_L6, letterH, ani_R6);
  delay(time_5bit);
  printInputWord(ani_L7, letterH, ani_R7);
  delay(time_5bit);
  printInputWord(ani_L8, letterH, ani_R8);
  delay(time_5bit);
  printInputWord(ani_L4, letterH, ani_R4);
  delay(time_5bit);
  printInputWord(ani_L5, letterH, ani_R5);
  delay(time_5bit);
  printInputWord(ani_L6, letterH, ani_R6);
  delay(time_5bit);
  printInputWord(ani_L7, letterH, ani_R7);
  delay(time_5bit);
  printInputWord(ani_L8, letterH, ani_R8);
  delay(time_5bit);
  printInputWord(ani_L4, letterH, ani_R4);
  delay(time_5bit);
  printInputWord(ani_L5, letterH, ani_R5);
  delay(time_5bit);
  printInputWord(ani_L6, letterH, ani_R6);
  delay(time_5bit);
  printInputWord(ani_L7, letterH, ani_R7);
  delay(time_5bit);
  printInputWord(ani_L8, letterH, ani_R8);
  delay(time_5bit);

  printInputWord(ani_L4, letterH, ani_R4);
  delay(time_8bit);

  printInputWord(ani_L4, letterH, ani_R4);
  delay(time_7bit);
  printInputWord(offBit, letterH, offBit);
  delay(time_5bit);

  printInputWord(ani_L4, letterH, ani_R4);
  delay(time_7bit);
  printInputWord(offBit, letterH, offBit);
  delay(time_5bit);

  printInputWord(ani_L4, letterH, ani_R4);
  delay(time_8bit);

  printInputWord(ani_L5, letterH, ani_R5);
  delay(time_7bit);
  printInputWord(ani_L6, letterH, ani_R6);
  delay(time_7bit);
  printInputWord(ani_L7, letterH, ani_R7);
  delay(time_7bit);
  printInputWord(ani_L8, one, ani_R8);
  delay(time_7bit);
  printInputWord(ani_L4, one, ani_R4);
  delay(time_7bit);
  printInputWord(ani_L10, one, ani_R10);
  delay(time_7bit);
  printInputWord(ani_L11, two, ani_R11);
  delay(time_7bit);
  printInputWord(ani_L12, two, ani_R12);
  delay(time_7bit);
  printInputWord(ani_L13, two, ani_R13);
  delay(time_7bit);
  printInputWord(letterR, three, letterY);
  delay(time_5bit);
  printInputWord(letterR, three, letterY);
  delay(time_5bit);
  printInputWord(letterR, three, letterY);
  delay(time_5bit);
  Serial.println("| READY |");
  delay(time_8bit);

  Serial.print(">>SHFT_REG_LED1 ");
  LED_greenON();
  delay(time_6bit);
  LED_clear();
  delay(time_5bit);

  LED_yellowON();
  delay(time_6bit);
  LED_clear();
  delay(time_5bit);
  LED_redON();
  delay(time_6bit);
  LED_clear();
  delay(time_5bit);
  LED_greenON();
  delay(time_6bit);
  LED_clear();
  delay(time_5bit);
  LED_yellowON();
  delay(time_6bit);
  LED_clear();
  delay(time_5bit);
  LED_redON();
  delay(time_6bit);
  LED_clear();
  delay(time_5bit);
  LED_greenON();
  delay(time_6bit);
  LED_clear();
  delay(time_5bit);
  LED_yellowON();
  delay(time_6bit);
  LED_clear();
  delay(time_5bit);
  LED_redON();
  delay(time_6bit);
  LED_clear();
  delay(time_5bit);
  Serial.println("| READY |");
  Serial.println("--------------------------------");
}


// ############################################################################
void setup()
{

  Serial.begin(9600);

  //Pins for SHIFT REGISTERS
  pinMode(latchPinSEG, OUTPUT);
  pinMode(dataPinSEG, OUTPUT);
  pinMode(clockPinSEG, OUTPUT);

  pinMode(latchPinLED_1, OUTPUT);
  pinMode(dataPinLED_1, OUTPUT);
  pinMode(clockPinLED_1, OUTPUT);


  pinMode(buttonPin, INPUT);
  pinMode(PIR_Pin, INPUT);

  startRoutine();

}
// #############################################################################
void loop()
{

  getCurrentTime();

  if(thisMinute % 5 == 0 && thisSecond < 20)
  {
    for (int x = 0; x<5;x++)
    {
      getCurrentTime();
      printCurretTime();
    }
    mode_2 = 0;
  }

  if(thisMinute != 36)
  {
    hoursTillXmas();
  }

  if(digitalRead(PIR_Pin) == HIGH && mode_2 == 0)
  {
    mode_2 = 1;
    
  }



  // if (thisSecond % 10 == 0)
  // {
  //   LED_greenON();
  //   delay(time_9bit);
  //   delay(1000);
  // }
  // if (thisSecond % 10 != 0)
  // {
  //   LED_clear();
  //   delay(time_9bit);
  // }
  // unsigned long finishTime = micros();
  // unsigned long deltaTime = finishTime - startTime;
  // Serial.println("---------------------");
  // Serial.println(deltaTime);
  // float secondsTime = float(deltaTime) / 1000000.00;
  // Serial.println(secondsTime,8);
  // Serial.println("---------------------");

}
