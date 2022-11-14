/*
 * eepromLoad - load settings from eeprom area
 * eepromSave - save settings to eeprom area
 * movetoFridgeStruct - load settings variables into fridge structure variable prior to save
 * movefromFridgeStruct - load settings variables from fridge structure variable after load
 * updTemp - converts string temperature into float variable
 * updTargetTemp - changes the target temperature and saves settings
 * resetStatistics - sets all statistics to zero and saves them
 * updIfanRuns - updates internal fan run statistics and saves them
 * updCompRuns - updates the compressor run statistics and saves them
 * updDoorOpens - updates the door open statistics and saves them
 * getiFanCount - returns count of internal fan runs as a string
 * getCompRuns - returns number of compressor runs as a string
 * getCompTime - returns total compressor run time as a string
 * getDoorOpens - returns count of door openings as a string
 * getDoorTime - returns total door open time as a string
 * convMillis - converts millisecond timings to hours, minutes and seconds
 * getTemp1 - returns the last reading of sensor 1 as a string
 * getTemp2 - returns the last reading of sensor 2 as a string
 * getThermostat - returns the current target temperature as a string
 * getFridgeState - returns the fridge power state as a string
 * getCompressorState - returns the compressor power state as a string
 * getInternalFanState - returns the internal fan state as a string
 * getExternalFanState - returns the external fan power state as a string
 * setTemperatureColor - returns a colour as a string based on whether the fridge is over or under temperature
 * updWiFiActive - updates WiFi status and saves it
 */
void eepromLoad() {
//  EEPROM.begin(sizeof(fridgeStruct));

  // Check if the EEPROM contains valid data from another run
  // If so, overwrite the 'default' values set up in our struct
//  if(EEPROM.percentUsed()>=0) {
//    EEPROM.get(0, eepromSave2);
    movefromFridgeStruct(); // load data from eeprom into variables
//  }   
}

void eepromSave() {

  // set the EEPROM data ready for writing

  movetoFridgeStruct();
//  EEPROM.put(0, eepromSave2);

  // write the data to EEPROM
 // boolean ok = EEPROM.commit();
}

//****** Save statistics to eprom after change
void movetoFridgeStruct() {
  eepromSave2.eepromTargetTemp = targetTemp;
  eepromSave2.eepromDoorOpens = totalDoorOpens;
  eepromSave2.eepromCompRuns = totalCompRuns;
  eepromSave2.eepromIfanRuns = totaliFanRuns;
  eepromSave2.eepromCompRunTime = totalCompTime;
  eepromSave2.eepromDoorOpenTime = totalDoorTime;
 // eepromSave2.eepromWiFiActive = wiFiActive;
//  strcpy(eepromSave2.eepromSSID, ssid);
//  strcpy(eepromSave2.eepromPSWD, password);
  
//  eepromSave2.eepromWiFitype;
//  eepromSave2.eepromIPaddress;
//  eepromSave2.eepromIPgateway;
//  IeepromSave2.eepromIPsubnet;
//  eepromSave2.eepromIPdns;
}

//****** Save statistics to eprom after change
void movefromFridgeStruct() {
  targetTemp = eepromSave2.eepromTargetTemp  ;
  totalDoorOpens = eepromSave2.eepromDoorOpens  ;
  totalCompRuns = eepromSave2.eepromCompRuns  ;
  totaliFanRuns = eepromSave2.eepromIfanRuns  ;
  totalCompTime = eepromSave2.eepromCompRunTime  ;
  totalDoorTime = eepromSave2.eepromDoorOpenTime  ;
 // wiFiActive = eepromSave2.eepromWiFiActive  ;
//  strcpy(ssid, eepromSave2.eepromSSID);
//  strcpy(password, eepromSave2.eepromPSWD);
  
//  eepromSave2.eepromWiFitype;
//  eepromSave2.eepromIPaddress;
//  eepromSave2.eepromIPgateway;
//  IeepromSave2.eepromIPsubnet;
//  eepromSave2.eepromIPdns;
}
//************* Routine to update target temperature
void updTemp(String settempvalue) {
  float tempf;
  tempf = settempvalue.toFloat();
  if ((tempf >= 1.0) && (tempf <= 10.0)) {
      updTargetTemp(tempf);
  }

  return;
}
//**************Routine to change target temp *****************
void updTargetTemp(float newTarget) {
  targetTemp = newTarget;
  eepromSave();
}




//****** Load statistics from eprom after power off/reset
void resetStatistics(void) {
totalCompTime = 0;
totaliFanRuns = 0;
totalDoorTime = 0;
totalDoorOpens = 0;
if (thermostatflag) {
    tempCompTime = millis();
    totalCompRuns = 1;
   } 
   else totalCompRuns = 0;
eepromSave();
}

//************************ Save internal fan stats to eeprom
void updIfanRuns() {
  totaliFanRuns++;
  eepromSave();
}


//************************ Save Compressor stats to eeprom
void updCompRuns() {
    totalCompTime = totalCompTime + (millis() - tempCompTime);
    totalCompRuns++;
    eepromSave();
}

//************************ Save door opening stats to eeprom
void updDoorOpens() {
    totalDoorTime = totalDoorTime + (millis() - tempDoorTime);
    totalDoorOpens++;
    eepromSave();
}




//************* Return ifan run count as a string
String getiFanCount(void) {
  return String(totaliFanRuns);
}

//************* Return total compressor runs as a string
String getCompRuns(void) {
  return String(totalCompRuns);
}

//************* Return total compressor time as a string
String getCompTime(void) {
  if (thermostatflag) {
     return String(convMillis(totalCompTime + (millis() - tempCompTime)));
  } else {
     return String(convMillis(totalCompTime));
  }
}

//************* Return total compressor runs as a string
String getDoorOpens(void) {
  return String(totalDoorOpens);
}

//************* Return total compressor time as a string
String getDoorTime(void) {
  return String(convMillis(totalDoorTime + tempDoorTime));
}

//************* Return time in millis as an HMS string
String convMillis(long inTime) {
  long convSeconds = inTime /1000;
  long convHours = convSeconds / 3600;
  long remSeconds = convSeconds % 3600;
  long convMinutes = remSeconds /60;
  convSeconds = convSeconds % 60;
  char convbuf[14];
  sprintf(convbuf,"%02d:%02d:%02d",convHours,convMinutes,convSeconds);
  return String(convbuf);
}

//************* Return temperature 1 as a string
String getTemp1(void) {
  return String(reading0);
}

//************* Return temperature 2 as a string
String getTemp2(void) {
  return String(reading1);
}

//************* Return thermostat setting as a string
String getThermostat(void) {
  return String(targetTemp);
}

//************* Return fridge state as a string
String getFridgeState(void) {
  if (powerstatus == 1) return "ON";
  else return "OFF";
}

//************* Return compressor state as a string
String getCompressorState(void) {
  if (thermostatflag == 1) return "ON";
  else return "OFF";
}

//************* Return intenal fan state as a string as a string
String getInternalFanState(void) {
  if (internalflag == 1) return "ON";
  else return "OFF";
}

//************* Return external fan state as a string
String getExternalFanState(void) {
  if (fanflag == 1) return "ON";
  else return "OFF";
}

//************* Return display color change based on as a string
//************* returns green if temperature below setting, red if above
String setTemperatureColor(void) {
  if (maxreading < hightemp) return "green ";
  else return "red    ";
}
//**************Routine to change wifi status *****************
//void updWiFiActive(int newWiFi) {
//  wiFiActive = newWiFi;
//  eepromSave();
//}
