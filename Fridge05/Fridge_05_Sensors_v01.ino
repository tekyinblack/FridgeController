/*
 * setupSensors - initialises temperature sensors and fridge control values
 * getSensorData - reads sensors and updates fridge control values
 */
 void setupSensors() {
  sensors.begin();
  sensors.setResolution(12);
  
  sensors.requestTemperatures(); // Send the command to get temperatures
  reading0 = sensors.getTempCByIndex(0);
  reading1 = sensors.getTempCByIndex(1);
  
 }

void getSensorData() {
      sensors.requestTemperatures(); // Send the command to get temperatures
      reading0 = sensors.getTempCByIndex(0);
      reading1 = sensors.getTempCByIndex(1);
      
      readingwait =  millis();
       
      maxreading = max(reading0,reading1);
      minreading = min(reading0,reading1);
    
      if (maxreading != oldmaxreading) {
        oldmaxreading = maxreading;
        displaychange = 1;
        }
}
