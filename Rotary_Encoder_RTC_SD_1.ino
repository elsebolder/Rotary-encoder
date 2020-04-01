/*     Arduino Rotary Encoder Tutorial

    by Dejan Nedelkovski, www.HowToMechatronics.com
    https://howtomechatronics.com/tutorials/arduino/rotary-encoder-works-use-arduino/

*/
#include <Wire.h>//libraries for the RTC
#include <TimeLib.h>
#include <DS1307RTC.h>

#include <SD.h>//Libraries for the SD
#include <SPI.h>

#define outputA 2
#define outputB 3

const int buttonPin = 7;

int counter = 0;
int aState;
int aLastState;

int buttonState = 0;

const int CSpin = 10;
String dataString = ""; // holds the data to be written to the SD card.
File sensorData;

int     minute1;//make the locations where the minutes, seconds and hours can be stored.
int     second1;
int     hour1;
time_t times;

String hourString;// making strings that are later used to make a string out of the integers. Strings are needed for dataString to be able to make a CSV file.
String minuteString;
String secondString;
String buttonSateString;

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
//unsigned long previousMillis = 0;        // will store last time LED was updated
//unsigned long time = 0;

// constants won't change:
//const long interval = 50;           // interval at which to write data

void setup() {
  Serial.begin(9600);
  Serial.print("Initializing SD card...");
  pinMode(CSpin, OUTPUT);//sets CSpin (pin 10) for SD as output
  pinMode (outputA, INPUT);
  pinMode (outputB, INPUT);
  pinMode(buttonPin, INPUT);
  //while (!Serial) ; // wait for serial
  Serial.println("DS1307RTC Read Test");
  Serial.println("-------------------");
  if (!SD.begin(CSpin)) {
    Serial.println("Card failed, or not present");

    return;
  }
  Serial.println("card initialized.");
}

void loop() {
  tmElements_t tm;

  if (RTC.read(tm)) {

    // read the state of the pushbutton value:
    buttonState = digitalRead(buttonPin);

    // check if the pushbutton is pressed.
    // if it is, the buttonState is HIGH:
    if (buttonState == LOW) {
      buttonSateString = "1";
    }
    else {
      buttonSateString = "0";
    }

    aState = digitalRead(outputA); // Reads the "current" state of the outputA
    // If the previous and the current state of the outputA are different, that means a Pulse has occured
    if (aState != aLastState) {
      // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
      if (digitalRead(outputB) != aState) {
        counter ++;
      } else {
        counter --;
      }
      //    Serial.print("Position: ");
      //    Serial.println(counter);
    }
    aLastState = aState; // Updates the previous state of the outputA with the current state
    //delay (100);

    times = now();//this function gets the time for hour,minutes and seconds at once. If you ask for them one at at time, the time might be less accurate.
    minute1 = (tm.Minute);
    second1 = (tm.Second);
    hour1 = (tm.Hour);
    if (hour1 < 10)//if the time is for example 02:35:06, it normally would write 2:35:6 which might be unhandy when calculating. So the "0" is added.
    {
      hourString = "0" + String(hour1);
    }
    else
    {
      hourString = String(hour1);
    }
    if (minute1 < 10)
    {
      minuteString = "0" + String(minute1);
    }
    else
    {
      minuteString = String(minute1);
    }
    if (second1 < 10)
    {
      secondString = "0" + String(second1);
    }
    else
    {
      secondString = String(second1);
    }
    // build the data string
    dataString = hourString + ":" + minuteString + ":" + secondString + "," + String(counter) + "," + (buttonSateString); // convert to CSV

    //Serial.println(dataString);
    saveData(); // save to SD card
    //delay(1000); // delay before next write to SD Card, adjust as required
 
  } else {
    if (RTC.chipPresent()) {
      Serial.println("The DS1307 is stopped.  Please run the SetTime");
      Serial.println("example to initialize the time and begin running.");
      Serial.println();
    } else {
      Serial.println("DS1307 read error!  Please check the circuitry.");
      Serial.println();
    }
    delay(9000);// does not need to be turned off since it is whithin the else statement.
  }
}

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}

void saveData() {
  if (SD.exists("data.csv")) { // check the card is still there
    // now append new data file
    sensorData = SD.open("data.csv", FILE_WRITE);
    if (sensorData) {
      sensorData.println(dataString);
      sensorData.close(); // close the file
    }
    else {
      Serial.println ("error opening file.");
    }
  }
  else {
    Serial.println("Error writing to file !");
  }
}
