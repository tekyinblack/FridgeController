/*
 * setupControlPins - called to initialise control pins from setup routine
 * onThermostat - switch thermostat relay ON
 * offThermostat - switch thermostat relay off
 * onFan - switch external fan relay ON
 * offFan - switch external fan relay OFF
 * onInternalFan - switch internal fan relay ON
 * offInternalFan - switch internal fan OFF
 * onPower - switch fridge electronics ON
 * offPower - switch fridge electronics OFF
 */
void setupControlPins() {
  pinMode(POWER_ON_PIN, OUTPUT); // powercntlpin
  pinMode(THERMOSTAT_PIN, OUTPUT); // thermostatpin
  pinMode(INTERNAL_FAN_PIN, OUTPUT); // internalpin
  pinMode(EXTERNAL_FAN_PIN, OUTPUT); // externalfan
  pinMode(POWER_SIGNAL_PIN, INPUT); // power signal
#define POWER_ON_PIN 5
#define THERMOSTAT_PIN 6
#define INTERNAL_FAN_PIN 7
#define EXTERNAL_FAN_PIN 8
#define POWER_SIGNAL_PIN 9
}

void onThermostat() {
  digitalWrite(THERMOSTAT_PIN, LOW);
  thermostatflag = 1; // thermostat flag flag
  tempCompTime = millis(); // record current time for statistics
}
void offThermostat() {
  digitalWrite(THERMOSTAT_PIN, HIGH);
  thermostatflag = 0; // thermostat flag flag
}
void onFan() {
  digitalWrite(EXTERNAL_FAN_PIN, LOW);
  fanflag = 1;     // fan flag
}
void offFan() {
  digitalWrite(EXTERNAL_FAN_PIN, HIGH);
  fanflag = 0;     // fan flag
}
void onInternalFan() {
  digitalWrite(INTERNAL_FAN_PIN, LOW);
  internalflag = 1; // internal fan flag
}
void offInternalFan() {
  digitalWrite(INTERNAL_FAN_PIN, HIGH);
  internalflag = 0; // internal fan flag
}
void onPower() {
  digitalWrite(POWER_ON_PIN, LOW);
}
void offPower() {
  digitalWrite(POWER_ON_PIN, HIGH);
}

int getPowerCntl() {
  if (digitalRead(POWER_SIGNAL_PIN == HIGH)) {
    return 1;
  } else return 0;
}
