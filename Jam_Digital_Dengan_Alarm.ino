// KELOMPOK 1
// 1.Hilman Mutaqin
// 2.M Wipaldi Nurpadilah
// 3.Devin Rival anugrah saputra 

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"
#include <EEPROM.h>

// sensor suara
#define SPEAKER_PIN 7
#define buzzer 7
#define LED_PIN 13 

RTC_DS1307 rtc; // sensor jam
LiquidCrystal_I2C lcd(0x27, 16, 2); 

// variable push button
int btnMode = 8; // tombol warna biru
int btnAdd = 9; // tombol warna hijau 
int btnMin = 10; // tombol warna merah 

int tmpHour = 0;
int tmpMinute = 0;

int menu = 0;

int storageHour = 0;
int storageMinute = 1;

const int debounce = 35;

int song[] = {500, 1000, 1500, 2000, 1500, 1000, 500};
int durasi[] = {80, 180, 280, 380, 500, 1000, 1500};

// variable nama hari dalam seminggu
char daysOfTheWeek[7][12] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};

void setup () {
  Serial.begin(9600);
  pinMode(buzzer, OUTPUT);
  pinMode(LED_PIN, OUTPUT); 
  pinMode(btnMode, INPUT);
  pinMode(btnAdd, INPUT);
  pinMode(btnMin, INPUT);

  lcd.begin (16,2);  
  lcd.backlight();  

  if (!rtc.begin()) {
    lcd.print("Couldn't find RTC");
  } if (!rtc.isrunning()) {
    lcd.print("RTC is NOT running!");
  }
}

void loop () {
  switch(menu) {
    case 0:
      displayTime();
      break;
    case 1:
      displayChangeHour();
      break;
    case 2:
      displayChangeMinute();
      break;
    case 3:
      saveSetup();
      break;
  }

  if (EEPROM.read(storageHour) != 255 && EEPROM.read(storageMinute) != 255) {
    alarm(tmpHour, tmpMinute);
  }

  delay(50);
}

void displayTime() {
  DateTime now = rtc.now();

  // menampilkan informasi tanggal pada baris pertama di LCD
  lcd.setCursor(0, 0);
  lcd.print(daysOfTheWeek[now.dayOfTheWeek()]);
  lcd.print("  ");
  lcd.print(now.day());
  lcd.print('/');
  lcd.print(now.month());
  lcd.print('/');
  lcd.print(now.year());

  // menampilkan informasi jam:menit:detik pada baris kedua di LCD
  lcd.setCursor(0, 1);
  lcd.print("TIME");
  lcd.print(" ");
  lcd.print(now.hour());
  lcd.print(':');
  lcd.print(now.minute());
  lcd.print(':');
  lcd.print(now.second());

  // mengambil jam dan menit dari sensor RTC
  tmpHour = now.hour();
  tmpMinute = now.minute();

  if (EEPROM.read(storageHour) != 255) {
    tmpHour = EEPROM.read(storageHour) <= 23 ? EEPROM.read(storageHour) : 0;
  } if (EEPROM.read(storageMinute) != 255) {
    tmpMinute = EEPROM.read(storageMinute) <= 59 ? EEPROM.read(storageMinute) : 0;
  }

 
  if(digitalRead(btnMode)) {
    menu = 1;
    delay(debounce);
  }
}

void displayChangeHour() {
  lcd.clear();
  DateTime now = rtc.now();

  lcd.setCursor(0,0);
  lcd.print("SET HOUR:");

  lcd.setCursor(0, 1);
  lcd.print(tmpHour);

  
  while(!digitalRead(btnMode)) {
    if(digitalRead(btnAdd) == HIGH) {
      if(tmpHour >= 23) {
        tmpHour = 0;
      } else {
        tmpHour = tmpHour + 1;
      }
      delay(debounce);
    } if(digitalRead(btnMin) == HIGH) { 
      if(tmpHour == 0) {
        tmpHour = 23;
      } else {
        tmpHour = tmpHour - 1;
      }
      delay(debounce);
    }
    DateTime now = rtc.now();

    lcd.setCursor(0, 0);
    lcd.print("SET HOUR:");

    lcd.setCursor(0, 1);
    lcd.print(tmpHour);
    lcd.print(" ");
  }

 
  menu = 2;
  delay(debounce);
}

void displayChangeMinute() {
  lcd.clear();
  DateTime now = rtc.now();

  lcd.setCursor(0,0);
  lcd.print("SET MINUTE:");

  lcd.setCursor(0, 1);
  lcd.print(tmpMinute);

  
  while(!digitalRead(btnMode)) {
    if(digitalRead(btnAdd) == HIGH) { 
      if(tmpMinute == 59) {
        tmpMinute = 0;
      } else {
        tmpMinute = tmpMinute + 1;
      }
      delay(debounce);
    } if(digitalRead(btnMin) == HIGH) { 
      if(tmpMinute == 0) {
        tmpMinute = 59;
      } else {
        tmpMinute = tmpMinute - 1;
      }
      delay(debounce);
    }
    DateTime now = rtc.now();

    lcd.setCursor(0, 0);
    lcd.print("SET MINUTE:");

    lcd.setCursor(0, 1);
    lcd.print(tmpMinute);
    lcd.print(" ");
  }

  menu = 3;
  delay(debounce);
}

void saveSetup() {
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("SAVING IN");

  lcd.setCursor(0,1);
  lcd.print("PROGRESS");

  EEPROM.write(storageMinute, tmpMinute);
  EEPROM.write(storageHour, tmpHour);

  menu = 0;
  delay(200);
}

void alarm(int clock, int minute) {
  
  DateTime now = rtc.now();
  int jam = now.hour(); 
  int menit = now.minute(); 
  if (clock == jam && minute == menit) { 
    lcd.clear();
    lcd.print("ALARM ON !!!");

    for (int note = 0; note < sizeof(song)/sizeof(song[0]); note++) {
      tone(buzzer, song[note], durasi[note]); 
      delay(durasi[note]);

      
      digitalWrite(LED_PIN, HIGH);            
      delay(durasi[note] / 2);                 
      digitalWrite(LED_PIN, LOW);             
      delay(durasi[note] / 2);                
    }
  }
}