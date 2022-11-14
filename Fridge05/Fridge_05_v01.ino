/*
 * 
 * V9 Updates to make wifi robust, still fails but may be string issue
 *   lots of menu updates to get working menu system, appears to work ok as a fridge controller
 *  but wifi now fails more often when operating  
 * V10 Updates to complete menu system and arrange code into more logical sequence for easier reading 
 *    Code added to use web server placeholder function and put webpage in PROGMEM, fixe web page failures
 *    Log and status display not complete, timing added to log number of times running etc. Door open timing not added yet.
 * V11 Almost final stretch. Rest of menu completed, rotary control performance sorted, log and status displays added as well as web equivalents
 * V12 created to make debug changes for wifi, not much further forward
 * v13 created to remove last odd bit, rotary control, not keyto operation so changed to basic polled operation
 * v14 added eeprom support
 * v15 add PCF8574 support to replace direct I/O output with I2C
 * v16 New core logic, variable cleanup
 * Rename to Fridge_03_V01 and divide into separate files 
 *  Fridge_03_Menus_v01 contains routines drawing menus on the SSD1306 screen used and reading the rotary control for input
 *  Fridge_03_PCF8574_v01 contains the output processing to the relays
 *  Fridge_03_Settings_v01 contains the routines updating settings, handling eeprom storage and returning settings for display
 *  Fridge_03_WiFi_v01 contains the routines to start the wifi and setup the webserver
 * Fridge_04 skipped 
 * Fridge_05_v01 wifi removed to be replaced with serial interface for remote comms
 *  base platform also changed to Arduino Nano/Uno using 5v native ports, removing PCF8574 
 *
 */
/*
 * setup - standard setup routine to initialise fridge control after reset
 * loop - the main processsing loop for the fridge control
 * milliwait - a wait calculation routine
 * changestatus - switches relays on and off to reflect desired operation
 */
#include <Arduino.h>
//#ifdef ESP32
//#include <WiFi.h>
//#include <AsyncTCP.h>
//#elif defined(ESP8266)
//#include <ESP8266WiFi.h>
//#include <ESPAsyncTCP.h>
//#endif
//#include <ESPAsyncWebServer.h>
//
//#include "PCF8574.h"
//PCF8574 pcf8574(0x38);

#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`


///////////////////EEPROM support//////////////////////////
// #include <ESP_EEPROM.h>
struct fridgeStruct {
  float    eepromTargetTemp;
  int      eepromDoorOpens;
  int      eepromCompRuns;
  int      eepromIfanRuns;
  long     eepromCompRunTime;
  long     eepromDoorOpenTime;
//  int      eepromWiFiActive;
  char     eepromSSID[20];
  char     eepromPSWD[20];
//  int      eepromWiFitype;
//  IPAddress  eepromIPaddress;
//  IPAddress  eepromIPgateway;
//  IPAddress  eepromIPsubnet;
//  IPAddress  eepromIPdns;
} eepromSave1, eepromSave2;



//////////////TEMPERATURE SENSOR CONTROL//////////////////////////

#define ONE_WIRE_BUS 3

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

/////////////DISPLAY DEFINITIONS////////////////////////

// Initialize the OLED display using Wire library
SSD1306Wire  display(0x3c, 4, 5);  //D2=SDK  D1=SCK  As per labeling on NodeMCU

//////////////MENU CONTROL///////////////////////////

int rotaryButtonPin = 2;  //<<<<< esp8266 pin used
int rotaryClockPin = 14;  //<<<<< esp8266 pin used
int rotaryDataPin = 12;   //<<<<< esp8266 pin used

 int rotaryPosCount = 0; 
 int rotaryClockLast;  
 int rotaryClockValue;
 boolean rotaryDirection;
 boolean rotaryFlip = false;

int rotaryButtonCount = 0;       // how many times we have seen new value
int rotaryButtonValue;           // the current value read from the input pin
int rotaryButtonState = HIGH;    // the debounced input value

long rotaryButtonTime = 0;         // the last time the output pin was sampled
int rotaryButtonDebounce = 10; // number of millis/samples to consider before declaring a debounced input

int blankDisplay = 1;    // <<<<<< variable stores display status, 0=blank/dark, 1=lit
long blankDisplaywait = 0;   // <<<< timer variable for display, usually set to 5 minutes
int menuDisplay = 0;
int menu1choice = 0;
int menu5choice = 0;
int buttonpos=0;
int oldbuttonpos=0;
int displaychange = 0;   //<<<<<<<<<< indicates that information has changed and display needs to be updated
int confirmBox = 0;   // confirmation box defaults to cancel




///////////////FRIDGE CONTROL////////////////////
#define POWER_ON_PIN 5
#define THERMOSTAT_PIN 6
#define INTERNAL_FAN_PIN 7
#define EXTERNAL_FAN_PIN 8
#define POWER_SIGNAL_PIN 9



int powerstatus = 0; // set power from external supply signal, supports remote switch with indicator light
int prev_powerstatus = 0;

int fridgeStatus = 0; // fridge status indicator 0=power off, 1=Normal, 2=Circulation, 3=Running, 4=Cooldown, 5=Freezing
#define POWER_OFF  0
#define NORMAL 1
#define CIRCULATION 2
#define RUNNING 3
#define COOLDOWN 4
#define FREEZING 5
long fridgeWait = 0; // fridge wait time
long fridgeTime = 0; // fridge current time
#define POWER_OFF_TIME  0
#define NORMAL_TIME 10000
#define CIRCULATION_TIME 120000
#define RUNNING_TIME 3
#define COOLDOWN_TIME 60000
#define FREEZING_TIME 300000

int fanflag = 0;     // fan on flag

int internalflag = 0; // internal fan flag

long readingwait = 0; // wait to do temperature readings

int thermostatflag = 0; // thermostat flag on flag

float tempDifferential = 3; // <<<----temperature differential between high and low temperatures in fridge


float reading1 = 0;   // Variables holding temperature readings
float reading0 = 0;
float oldmaxreading = 0;
float minreading = 0;
float maxreading = 0;

float hightemp = 5;   // initial high temperature, subsequently loaded from thermostat setting
float lowtemp = 2;  // initial low temperature

long totalCompTime = 0; // statistic variables for processing, updated from eeprom in normal running
int totalCompRuns = 0;
long tempCompTime = 0;
int totaliFanRuns = 0;
long tempDoorTime = 0;
long totalDoorTime = 0;
int totalDoorOpens = 0;


float targetTemp = 24.0; //<<<<<<<<<<<<default thermostat setting, is updated from eeprom in normal running
float newTargetTemp;
int   newTargetTempSet = 0;


////////////////WIFI CONTROL//////////////////////////

//IPAddress fridgeIPaddress;
//int oldWiFistatus = WL_DISCONNECTED;


//char ssid[20] = "SKYBC4E3";         // wifi SSID and password
//char password[20] = "RYTDXUFF";
//
//int wiFiActive = 1;

//////////////WEBSERVER DEFINITION//////////////////////////

//AsyncWebServer server(80);

////////////////WEB SERVER CONTROL//////////////////////////
//************************** Start of static Fridge status page
//const char Fridge_Page[] PROGMEM = R"rawliteral(
//<!DOCTYPE html>
//<html>
//<head>
//<meta http-equiv="refresh" content="30">
//<title>Lemmie Fridge</title>
//</head>
//<body style="font-family:verdana;font-size:300%%;">
//<h1>Lemmie Fridge</h1>
//
//<p style="color:%COLOUR%">Temperatures %TEMP1%<sup>o</sup>C %TEMP2%<sup>o</sup>C</p>
//<p>Status: %STATUS%</p>
//<p style="color:%COLOUR%">Thermostat Setting %THERMOSTAT%<sup>o</sup>C</p>
//<p><a href="setThermostat">Set Thermostat</a></p>
//<p>Statistics<br>
//<pre>
//Internal Fan Run Count: %IFANCOUNT%
//Compressor Run Count:   %COMPRUNS%
//Compressor Run Time:    %COMPTIME%
//Door Open Count:        %DOOROPENS%
//Door Open Time:         %DOORTIME%
//</pre>
//
//</body>
//</html>)rawliteral";
////************************** End of static Fridge status page 1
//
//
////************************** Start of static Fridge status page 2
//const char Fridge_Page2[] PROGMEM = R"rawliteral(
//<!DOCTYPE html>
//<html>
//<head>
//<title>Lemmie Fridge</title>
//</head>
//<body style="font-family:verdana;font-size:300%%;">
//<h1>Lemmie Fridge</h1>
//
//
//<form action="/updatetemp" method="get">
//<label for="temp">Set temperature <sup>o</sup>C</label>
//<select id="temp" name="temp" style="font-family:verdana;font-size:100%%;">
//<option value='%THERMOSTAT%' selected>%THERMOSTAT%</option>
//<option value='10.0'         >10</option>
//<option value='9.5'         >9.5</option>
//<option value='9.0'         >9.0</option>
//<option value='8.5'         >8.5</option>
//<option value='8.0'         >8.0</option>
//<option value='7.5'         >7.5</option>
//<option value='7.0'         >7.0</option>
//<option value='6.5'         >6.5</option>
//<option value='6.0'         >6.0</option>
//<option value='5.5'         >5.5</option>
//<option value='5.0'         >5.0</option>
//<option value='4.5'         >4.5</option>
//<option value='4.0'         >4.0</option>
//<option value='3.5'         >3.5</option>
//<option value='3.0'         >3.0</option>
//<option value='2.5'         >2.5</option>
//<option value='2.0'         >2.0</option>
//<option value='1,5'         >1.5</option>
//<option value='1.0'         >1.0</option>
//</select>
//<input type="submit" value="Set" style="font-family:verdana;font-size:100%;">
//</form>
//</p>
//<p><a href="home">Back</a></p>
//</body>
//</html>)rawliteral";
//************************** End of static Fridge status page 2


///////////END OF DATA DEFINITIONS/////////////////////////////////////////////////////////////


//************ Start of processing first routine is Setup 
void setup() {

//*****************Setup pins for fridge control*******************

  setupControlPins();

//*****run reinitialisation of all fridge I/O

  reStart();


//******* Load saved data
  eepromLoad();

// Initialise temperature sensors

  setupSensors();


  delay(1000); // breathe

//************ Setup of wifi

// setupWiFi();

//************ Setup of of rotary encoder

  setupRotary();


//************ Start of setup of display

  setupScreen();



}

void reStart(void) {
// come here to process restart of fridge

// turn off relays
offThermostat();
offFan();
offInternalFan();
}

////////////////MAIN PROCESSING LOOP STARTS HERE//////////////////

void loop() {
  
delay(1); // delay included to stop race condition and allow processing in asynchronous threads



  // Read power on status
  //powerstatus = getPowerCntl();
//  if (analogRead(A0) > 512) {
//    powerstatus = 1;
//  }
//  else powerstatus=0;

   powerstatus = 1;  // fudge until power control cable is implemented


  // if power on status low, reset all outputs and flags, turn off power
  // power off really only impacts the power consumption on the light and relays, Arduino will continue to consume about 20mA
  if (powerstatus == 0) {
           fridgeTime = changeStatus(POWER_OFF);
           prev_powerstatus = 0;
           }
  
  
  // If power on status high, turn on power, set power status high, if previous power status low then reset all outputs and flags.
  if (powerstatus == 1 and prev_powerstatus == 0) {
           fridgeTime = changeStatus(NORMAL);
           blankDisplay = 1;
           blankDisplaywait = millis();
           menuSend(0); 
           delay(2000); // wait two seconds to allow power up of circuits.     
           readingwait=millis();
           prev_powerstatus = 1;
           }
           
  // Get set temperatures
  hightemp=targetTemp;
  lowtemp=targetTemp-tempDifferential;
  
  //***** check the fridge status every two seconds
  if (milliwait(readingwait, 2000) and fridgeStatus != POWER_OFF) {
    
     // *** get and update the temperature
     getSensorData();

   // ***************** check wifi status to update status screen
   //  wifiCheck();
        
      }

 
switch (fridgeStatus) {
  case POWER_OFF:  // power is off so no action taken
  break;
  
  case NORMAL: // In normal status
  if (milliwait(fridgeTime, NORMAL_TIME)) {
    if (minreading < 1.5) {
      fridgeTime = changeStatus(FREEZING); // change to freezing
      fridgeWait = FREEZING_TIME;
      } 
    else if (minreading >= hightemp) {
      fridgeTime = changeStatus(RUNNING); // change to Running as temperatures to high 
      fridgeWait = RUNNING_TIME;
      updIfanRuns();  //   update statistics
      }     
    else if (maxreading >= hightemp) {
      fridgeTime = changeStatus(CIRCULATION); // change to Circulation 
      fridgeWait = CIRCULATION_TIME;
      updIfanRuns();  //   update statistics
      }
  }  
  break;
  
  case CIRCULATION:  // Circulation fan running
  if (milliwait(fridgeTime,CIRCULATION_TIME) and maxreading >= hightemp) {
    fridgeTime = changeStatus(RUNNING); // change to Circulation 
    fridgeWait = RUNNING_TIME;
    }
  if (milliwait(fridgeTime,CIRCULATION_TIME) and maxreading < hightemp) {
    fridgeTime = changeStatus(NORMAL); // change back to normal 
    fridgeWait = NORMAL_TIME;
    }  
  break;
    
  case RUNNING:
  if (minreading < 1.5) {
    fridgeTime = changeStatus(FREEZING); // change to cooldown
    fridgeWait = FREEZING_TIME;
    }  
  else if (minreading < lowtemp and maxreading < hightemp) {
    fridgeTime = changeStatus(COOLDOWN); // change to cooldown
    fridgeWait = COOLDOWN_TIME;
    } 
  if (fridgeStatus != RUNNING) {  // If no longer running status then update statistics
        updCompRuns();  // update statistics 
        }
  break; 
   
  case COOLDOWN:
  if (milliwait(fridgeTime,COOLDOWN_TIME)) {
    fridgeTime = changeStatus(NORMAL); // change to cooldown
    fridgeWait = NORMAL_TIME;
    }  
  break;
  
  case FREEZING:
    if (milliwait(fridgeTime,FREEZING_TIME)) {
    fridgeTime = changeStatus(NORMAL); // change to cooldown
    fridgeWait = NORMAL_TIME;
    }  
  break;
}

//*****  process rotary control every time round
 if (rotaryRead()) {
  rotaryTurn();
 }
  if (rotaryButton()) {
  rotaryPress();
 }

     
// Blank display after 5 minutes
  if (milliwait(blankDisplaywait,300000) and blankDisplay == 1) {
      blankDisplay = 0;
      menuDisplay = 0;
      menuSend(0);
     }
  if (displaychange) {
    displaychange = 0;
    menuSend(menuDisplay );
    }
}
/////////////////////////////////////////////////////////////////
// routine used to process delays
int milliwait(long starttime, long delaytime) {
  if (millis() > (starttime+delaytime)) {
    return 1;
}
  else return 0; 
}
///////////////////////////////////////////
long changeStatus(int newStatus) {
  int returnValue = 0;
  switch (newStatus) {
    case POWER_OFF:   // Power off fridge
      offInternalFan();
      offThermostat();
      offFan();
      offPower();
      break;
    case NORMAL:   // Fridge in normal state
      onPower();
      offInternalFan();
      offThermostat();
      offFan();
      break;
    case CIRCULATION:   // Circulation
      onInternalFan();
      offThermostat();
      offFan();
      returnValue = millis();
      break;
    case RUNNING:  // Running
      onInternalFan();
      onThermostat();
      onFan();
      returnValue = millis();
      break;
    case COOLDOWN:   // Cooldown
      offInternalFan();
      offThermostat();
      onFan();
      returnValue = millis();
      break;
    case FREEZING:   // Freezing
      onInternalFan();
      offThermostat();
      offFan();
      returnValue = millis();
      break;
  }
  displaychange = 1;
  fridgeStatus = newStatus;
  return returnValue;
}
