//////////////ROUTINES TO DRAW FRIDGE MENU SYSTEM????????????????????
/*
 * setupScreen - runs initialisation of screen and menus for setup routine
 * getStatus - returns fridge status as a string
 * drawFridge0 - draws default basic fridge operation screen
 * drawFridge1 - draws high level menu 
 * drawFridge2 - thermostat temperature menu
 * drawFridge2a - thermostat temperature confirmation menu
 * drawFridge3 - Network settings
 * drawFridge4 - Statistics
 * drawFridge5 - Option for settings submenu
 * drawFridge5a - turn wifi off/on menu
 * drawFridge5b - reset statistics
 * drawFridge5c - reset WiFi
 * drawFridge5d - soft reset fridge
 * menuSend - send next menu, change menu status
 * setupRotary - initialise rotary control
 * rotaryRead - checks if control has been rotated
 * rotaryButton - checks for a button press
 * rotaryPress - process rotary button press, process menu selection or menu set/cancel operation
 * rotaryTurn - process rotary button turn, process menu value or position
 */
void setupScreen() {
    display.init();

  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);

//* Display initial menu 
  menuSend(0);
}
 
String getStatus(void) {
     switch (fridgeStatus) {
      case 0:
        return "Off";
        break; 
      case 1:
        return "Normal";
        break; 
      case 2:
        return "Circulation fan";
        break; 
      case 3:
        return "Running";
        break; 
      case 4:
        return "Cooldown";
        break; 
      case 5:
        return "Freezing";
        break; 
     }
}

void drawFridge0() {    // Main display subroutine
    // clear the display
  display.clear();
  if (blankDisplay == 0) {
    display.display();
    return;
  }

  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "Lemmie Fridge");
  display.setFont(ArialMT_Plain_24);
  ///****************  If power is off then display OFF and no other information, 
  ///****************  if ON then temperature, status and WIFI status or IP Address
    if (powerstatus == 1) { 
      display.drawString(0, 12, String(max(reading0,reading1))+"C");
      display.setFont(ArialMT_Plain_16);
      display.drawString(78, 18, String(targetTemp));
      display.drawString(0, 36, getStatus());
   //   display.drawString(64, 36, String(loopTimer4));  <<<---was reporting loop time to display
      display.setFont(ArialMT_Plain_10);
  
//      switch (WiFi.status()) {
//        case WL_CONNECTED:
//               fridgeIPaddress = WiFi.localIP();
//               display.drawString(0, 54, fridgeIPaddress.toString());
//               break;
//        case WL_CONNECT_FAILED:
//               display.drawString(0, 54, "Wifi Connect Failed");
//               break;
//        case WL_CONNECTION_LOST:
//               display.drawString(0, 54, "Wifi Connection Lost");
//               break;
//        case WL_IDLE_STATUS:
//               display.drawString(0, 54, "Wifi Idle");
//               break;
//        case WL_NO_SSID_AVAIL:
//               display.drawString(0, 54, "Wifi Connection Lost");
//               break;
//        case WL_DISCONNECTED:
//               display.drawString(0, 54, "Wifi Disconnected");
//               break;
//        case WL_SCAN_COMPLETED:
//               display.drawString(0, 54, "Wifi Scan Completed");
//               break;
//        case WL_NO_SHIELD:
//               display.drawString(0, 54, "Wifi No Shield");
//               break;
//               
//        default:
//               display.drawString(0, 54, "Wifi Unknown Error");
//        
//      }
    // write the buffer to the display
    }
    else {
      display.drawString(0, 12, String("OFF"));  /// if off then simple message OFF
    }
  display.display();
}
//****************Routine to display menu with selection highlighted with > sign
void drawFridge1() {  // clear the display
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  if (menu1choice == 1) {
    display.drawString(0, 0, ">Set Temperature");
  }
  else {
    display.drawString(0, 0, "Set Temperature");
  }
  if (menu1choice == 2) {
    display.drawString(0, 16, ">Network");
  }
  else {
    display.drawString(0, 16, "Network");
  }
  if (menu1choice == 3) {
    display.drawString(0, 32, ">Statistics");
  }
  else {
    display.drawString(0, 32, "Statistics");
  }
  if (menu1choice == 4) {
     display.drawString(0, 48, ">Settings");
  }
  else {
     display.drawString(0, 48, "Settings");
  }  
 
  // write the buffer to the display
  display.display();
}
///*************  Routine to display the temperature setting and update confirmation
void drawFridge2() {
  // clear the display
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "Set Temperature");
  display.setFont(ArialMT_Plain_24);
  display.drawString(0, 18, String(newTargetTemp));
  display.setFont(ArialMT_Plain_16);
  if (newTargetTempSet == 1) {
       display.drawString(0,48, String("Thermostat SET"));
       newTargetTempSet = 0;
  }
  
  // write the buffer to the display
  display.display();
}
///*************  Routine to display the temperature setting and CANCEL/SET options
void drawFridge2a() {
  // clear the display
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "Set Temperature");
  display.setFont(ArialMT_Plain_24);
  display.drawString(0, 18, String(newTargetTemp));
  display.setFont(ArialMT_Plain_16);
  display.drawString(3, 45, "CANCEL");
  display.drawString(80, 45, "SET");
  if (confirmBox == 0) {
    display.drawRect(0, 45, 72, 18);
  }
  else {
    display.drawRect(73, 45, 43, 18);
  }
  // write the buffer to the display
  display.display();
}
///*************** Routine to display network information
//void drawFridge3() {
//  // clear the display
//  String currentSSID;
//  display.clear();
//  display.setTextAlignment(TEXT_ALIGN_LEFT);
//  display.setFont(ArialMT_Plain_10);
//  display.drawString(0, 14, "SSID:");
//  //currentSSID = testRoutine();
//  display.drawString(30, 14, WiFi.SSID());
//  display.drawString(0, 24, "IP:");
//  fridgeIPaddress = WiFi.localIP();
//  display.drawString(30, 24, fridgeIPaddress.toString());
//  display.drawString(0, 34, "GW:");
//  fridgeIPaddress = WiFi.gatewayIP();
//  display.drawString(30, 34, fridgeIPaddress.toString());
//  display.drawString(0, 44, "SN:");
//  fridgeIPaddress = WiFi.subnetMask();
//  display.drawString(30, 44, fridgeIPaddress.toString());
//  display.drawString(0, 54, "DNS:");
//  fridgeIPaddress = WiFi.dnsIP();
//  display.drawString(30, 54, fridgeIPaddress.toString());
//  // write the buffer to the display
//  display.display();
//}
///************** Routine to display internal diagnostic info
void drawFridge4() {
  // clear the display display T1 T2 last connected IP output status set temp wifi up/down thermostat times ifan times
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "Statistics");
  display.drawString(0, 10, "iFan count:");
  display.drawString(55, 10, getiFanCount());
  display.drawString(0, 20, "Running Count:");
  display.drawString(75, 20, getCompRuns());
  display.drawString(0, 30, "Running time:");
  display.drawString(70, 30, getCompTime());
  display.drawString(0, 40, "Open Door count:");
  display.drawString(85, 40, getDoorOpens());
  display.drawString(0, 50, "Open Door time:");
   display.drawString(80, 50, getDoorTime());

  // write the buffer to the display
  display.display();
}
///************** Routine to display setting options when implemented
void drawFridge5() {
  // clear the display
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  if (menu5choice == 1) {
    display.drawString(0, 0, ">WiFi ON/OFF");
  }
  else {
    display.drawString(0, 0, "WiFi ON/OFF");
  }
  if (menu5choice == 2) {
    display.drawString(0, 16, ">Reset Stats");
  }
  else {
    display.drawString(0, 16, "Reset Stats");
  }
  if (menu5choice == 3) {
    display.drawString(0, 32, ">Reset WiFi");
  }
  else {
    display.drawString(0, 32, "Reset WiFi");
  }
  if (menu5choice == 4) {
     display.drawString(0, 48, ">Reset Fridge");
  }
  else {
     display.drawString(0, 48, "Reset Fridge");
  }  
 
  // write the buffer to the display
  display.display();

  // write the buffer to the display
  display.display();
}
//void drawFridge5a() {
//  // clear the display
//  display.clear();
//  display.setTextAlignment(TEXT_ALIGN_LEFT);
//  display.setFont(ArialMT_Plain_16);
//  display.drawString(0, 0, "Turn WiFI ON/OFF");
//  display.setFont(ArialMT_Plain_24);
//  if (wiFiActive) {
//    display.drawString(0, 18, "Turn OFF");
//  } else  display.drawString(0, 18, "Turn ON");
//  display.setFont(ArialMT_Plain_16);
//  display.drawString(3, 45, "CANCEL");
//  display.drawString(80, 45, "OK");
//  if (confirmBox == 0) {
//    display.drawRect(0, 45, 72, 18);
//  }
//  else {
//    display.drawRect(73, 45, 43, 18);
//  }
//  // write the buffer to the display
//  display.display();
//}
void drawFridge5b() {
  // clear the display
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "Reset Statistics");
  display.setFont(ArialMT_Plain_16);
  display.drawString(3, 45, "CANCEL");
  display.drawString(80, 45, "OK");
  if (confirmBox == 0) {
    display.drawRect(0, 45, 72, 18);
  }
  else {
    display.drawRect(73, 45, 43, 18);
  }
  // write the buffer to the display
  display.display();
}
//void drawFridge5c() {
//  // clear the display
//  display.clear();
//  display.setTextAlignment(TEXT_ALIGN_LEFT);
//  display.setFont(ArialMT_Plain_16);
//  display.drawString(0, 0, "Reset Wifi");
//  display.setFont(ArialMT_Plain_16);
//  display.drawString(3, 45, "CANCEL");
//  display.drawString(80, 45, "OK");
//  if (confirmBox == 0) {
//    display.drawRect(0, 45, 72, 18);
//  }
//  else {
//    display.drawRect(73, 45, 43, 18);
//  }
//  // write the buffer to the display
//  display.display();
//}
void drawFridge5d() {
  // clear the display
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "Reset Fridge");
  display.setFont(ArialMT_Plain_16);
  display.drawString(3, 45, "CANCEL");
  display.drawString(80, 45, "OK");
  if (confirmBox == 0) {
    display.drawRect(0, 45, 72, 18);
  }
  else {
    display.drawRect(73, 45, 43, 18);
  }
  // write the buffer to the display
  display.display();
}
////*************Routine translates a numeric menu number to a menu draw routine
void menuSend(int menuNo) {
  switch (menuNo) {
     case 0:
     drawFridge0();   // default status display
     break;
     case 1:
     drawFridge1();   // selection menu
     break;
     case 2:
     drawFridge2();   // set temperature display
     break;
//     case 3:
//     drawFridge3();   // network info display
     break;
     case 4:
     drawFridge4();   // variable status display
     break;
     case 5:
     drawFridge5();   // Settings
     break;
     case 6:
     drawFridge2a();  // set temperature confirmation
     break;
//     case 7:
//     drawFridge5a();  // Turn WiFi ON/OFF
     break;
     case 8:
     drawFridge5b();  // Reset Stats
     break;
//     case 9:
//     drawFridge5c();  // Reset WiFi
     break;
     case 10:
     drawFridge5d();  // Reset Fridge
     break;
     default:
     menuDisplay=0;
     drawFridge0();  
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
void setupRotary() {
  
pinMode(rotaryButtonPin, INPUT_PULLUP);  // <<<<---------Reinstate, turned off for diagnosis
pinMode(rotaryClockPin, INPUT);   // <<<<---------Reinstate, turned off for diagnosis
pinMode(rotaryDataPin, INPUT);   // <<<<---------Reinstate, turned off for diagnosis
rotaryClockLast = digitalRead(rotaryClockPin);
}

boolean rotaryRead() { 
  boolean returnValue = false;
   rotaryClockValue = digitalRead(rotaryClockPin);
   if (rotaryClockValue != rotaryClockLast){ // Means the knob is rotating
     // if the knob is rotating, we need to determine direction
     // We do that by reading pin B.
     if (digitalRead(rotaryDataPin) != rotaryClockValue) {  // Means pin A Changed first - We're Rotating Clockwise
       rotaryPosCount ++;
       rotaryDirection = true;
     } else {// Otherwise B changed first and we're moving CCW
       rotaryDirection = false;
       rotaryPosCount--;
     }

     if (rotaryFlip) {
        returnValue=true;
        rotaryFlip = false;
     }
     else rotaryFlip = true;
   } 
   rotaryClockLast = rotaryClockValue;
   return returnValue;
 } 

/////////////////////////////////////////////////////////////////////////////////////////


boolean rotaryButton(void) {
  // If we have gone on to the next millisecond
  boolean returnValue = false;
  if(millis() != rotaryButtonTime)
  {
    rotaryButtonValue = digitalRead(rotaryButtonPin);

    if(rotaryButtonValue == rotaryButtonState && rotaryButtonCount > 0)
    {
      rotaryButtonCount--;
    }
    if(rotaryButtonValue != rotaryButtonState)
    {
       rotaryButtonCount++; 
    }
    // If the Input has shown the same value for long enough let's switch it
    if(rotaryButtonCount >= rotaryButtonDebounce)
    {
      rotaryButtonCount = 0;
      rotaryButtonState = rotaryButtonValue;
      if (rotaryButtonState == LOW) {
        returnValue = true;
      }
    }
    rotaryButtonTime = millis();
  }
  return returnValue;
}

// on change

  void rotaryTurn() {

  buttonpos = rotaryPosCount;
    if (buttonpos != oldbuttonpos) {
    switch (menuDisplay) {
      case 0:
          // if status screen and blank, turn display on
          if (blankDisplay == 0) {
            blankDisplay = 1;
          } 
          else {
            menuDisplay = 2;
          }
      break;
      
      case 1:
          if (buttonpos > oldbuttonpos) menu1choice++;
          else menu1choice--;
          if (menu1choice < 0) menu1choice = 0;
          if (menu1choice > 4) menu1choice = 4;
        break;
        
      case 2:
        if (buttonpos > oldbuttonpos) newTargetTemp= newTargetTemp+0.5;
        else newTargetTemp= newTargetTemp-0.5;
        if (newTargetTemp < 1) newTargetTemp = 1;
        if (newTargetTemp > 25) newTargetTemp = 25;
        break;
        
      case 5:
          if (buttonpos > oldbuttonpos) menu5choice++;
          else menu5choice--;
          if (menu5choice < 0) menu5choice = 0;
          if (menu5choice > 4) menu5choice = 4;
        break;
        
      case 6:
        if (confirmBox == 0) confirmBox = 1;
        else confirmBox = 0;
        break;
      case 7:
        if (confirmBox == 0) confirmBox = 1;
        else confirmBox = 0;
        break;
      case 8:
        if (confirmBox == 0) confirmBox = 1;
        else confirmBox = 0;
        break;
      case 9:
        if (confirmBox == 0) confirmBox = 1;
        else confirmBox = 0;
        break;
      case 10:
        if (confirmBox == 0) confirmBox = 1;
        else confirmBox = 0;
        break;
    }
      blankDisplaywait=millis();

  }
  oldbuttonpos=buttonpos;
  blankDisplaywait=millis();
  displaychange = 1;
}




// single click

void rotaryPress() {
//  String temptemp ;
  switch (menuDisplay) {
    case 0:  // active status screen
    // if status screen and blank, turn display on
    if (blankDisplay == 0) {
      blankDisplay = 1;
    }
    else {
      // if status screen and showing, turn show menu 
      menuDisplay = 1;
      menu1choice = 0;
    }
    break;
    case 1:   // main menu
      switch (menu1choice) {
        case 1:
        newTargetTemp = targetTemp;
        menuDisplay = 2;
        break;
        case 2: 
        menuDisplay = 3;
        break;
        case 3:
        menuDisplay = 4;
        break;
        case 4:
        menuDisplay = 5;
        menu5choice = 0;
        break;
        default:
        menuDisplay = 0;
      }
    break;
    case 2:  // set new thermostat temperature
       if (newTargetTemp == targetTemp) {
            menuDisplay = 0;
       }
       else {
            menuDisplay = 6;
            confirmBox = 0;
       }
    break;   
    case 5:  // display setting menu
      switch (menu5choice) {
        case 1:
        menuDisplay = 7;
        break;
        case 2: 
        menuDisplay = 8;
        break;
        case 3:
        menuDisplay = 9;
        break;
        case 4:
        menuDisplay = 10;
        break;
        default:
        menuDisplay = 1;
        menu1choice = 0;
        menu5choice = 0;
      }
    break;
    case 6:  // Thermostat change confirmation
       if (confirmBox == 0) {
            menuDisplay = 2;
            newTargetTemp = targetTemp;
       }
       else {
            menuDisplay = 2;
            updTargetTemp(newTargetTemp);
            newTargetTempSet = 1;
       }
    break; 
//        case 7:  // Turn wifi off/on
//       if (confirmBox == 0) {
//            menuDisplay = 5;
//            menu5choice = 0;
//       }
//       else {
//            if (wiFiActive) {
//            //  updWiFiActive(0);
//            //  wiFiStop();
//            } 
//            else {
//            //  updWiFiActive(1);
//         //     wiFiStart();
//            }
//       }
//    break; 
        case 8:  // reset statistics
       if (confirmBox == 0) {
            menuDisplay = 5;
            menu5choice = 0;
       }
       else {
            menuDisplay = 5;
            menu5choice = 0;
            resetStatistics();
       }
    break; 
        case 9:  // reset wifi
       if (confirmBox == 0) {
            menuDisplay = 5;
            menu5choice = 0;
       }
       else {
          //  wiFiReset();
            menuDisplay = 3;  // takes user to network display
            menu5choice = 0;
       }
    break; 
        case 10:  // reset fridge operation
       if (confirmBox == 0) {
            menuDisplay = 5;
            menu5choice = 0;
       }
       else {
            menuDisplay = 0;  // takes user back to status display
            menu5choice = 0;
            fridgeTime = changeStatus(NORMAL);
            targetTemp = 50.0;
       }
    break; 
    default:
    menuDisplay = 0;
  }
blankDisplaywait=millis();
confirmBox = 0;
displaychange = 1;

}
