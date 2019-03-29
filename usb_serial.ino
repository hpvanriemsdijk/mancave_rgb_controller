/*
 * Get serial data 
 * {[string]command:[int]value}
 * 
 * Commands: 
 *  - get 
 *  -- all
 *  - brightness [0-255]
 *  - effect [0-10]
 *  -  [0-255]
 *  - effect_speed [0-255]
 *  - color [000000 - ffffff]
 *  - color_left [000000 - ffffff]
 *  - color_right [000000 - ffffff]
 *  - transition [0,1]
 *  
 */

void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '{';
    char endMarker = '}';
    char rc;

    while (Serial.available() > 0 && newSerialData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newSerialData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }

     if (newSerialData == true) {
        strcpy(tempChars, receivedChars);
        parseData();
        updateSettings();
        newSerialData = false;
    }
}

void parseData() {     
  char * strtokIndx; 
  
  strtokIndx = strtok(tempChars,":"); 
  strcpy(serialCommand, strtokIndx);  
  strtokIndx = strtok(NULL, ":");     
  strcpy(serialValue, strtokIndx);
}

void updateSettings() {
  String tmpCommand, tmpVal;
  int tmpIntVal;
  char tmpHexStr[9];
  tmpCommand = String(serialCommand);
  tmpVal = String(serialValue);
  tmpIntVal = atoi(serialValue);

  if(tmpCommand == "get"){
    if(tmpVal == "all"){
      returnChange("brightness", brightness);
      returnChange("effect", effect);  
      returnChange("effect_speed", effect_speed); 
      returnChange("transition", transition); 
      returnColorChange(color_left, "color_left");
      returnColorChange(color_right, "color_right");
    }
  }else if(tmpCommand == "brightness"){
    brightness = constrain(tmpIntVal,0,255);
    FastLED.setBrightness(brightness);
    returnChange("brightness", brightness);
  }else if(tmpCommand == "effect"){
    effect = constrain(tmpIntVal,0,10);
    returnChange("effect", effect);    
  }else if(tmpCommand == "effect_speed"){
    effect_speed = constrain(tmpIntVal,0,255);
    returnChange("effect_speed", effect_speed); 
  }else if(tmpCommand == "transition"){
    transition = constrain(tmpIntVal,0,10);
    returnChange("transition", transition); 
  }else if(tmpCommand == "color"){  
    long number = (long) strtol( &tmpVal[0], NULL, 16);
    color_left[0] = color_right[0] = constrain(number >> 16,0,255);
    color_left[1] = color_right[1] = constrain(number >> 8 & 0xFF,0,255);
    color_left[2] = color_right[2] = constrain(number & 0xFF,0,255);  
    returnColorChange(color_left, "color");
  }
}

void returnChange(String setting, int value){
  returnChange(setting, (String)value);
}

void returnChange(String setting, String value){
  Serial.print("{\"");
  Serial.print(setting);
  Serial.print("\":\"");
  Serial.print(value);
  Serial.println("\"}"); 
}

void returnColorChange(int color[3], String label){
    Serial.print("{\"");
    Serial.print(label); 
    Serial.print("\":\"");
    printHEX(color[0],2);
    printHEX(color[1],2);
    printHEX(color[2],2);
    Serial.println("\"}"); 
}

void printHEX(unsigned long DATA, unsigned char numChars) {
  unsigned long mask  = 0x0000000F;
  mask = mask << 4*(numChars-1);
  
  for (unsigned int i=numChars; i>0; --i) {
    Serial.print(((DATA & mask) >> (i-1)*4),HEX);
    mask = mask >> 4;
  }
}

void printColor(){
  Serial.print(color_left[0]); 
  Serial.print(","); 
  Serial.print(color_left[1]); 
  Serial.print(","); 
  Serial.print(color_left[2]); 
  Serial.print("||"); 
  Serial.print(color_right[0]); 
  Serial.print(","); 
  Serial.print(color_right[1]); 
  Serial.print(","); 
  Serial.println(color_right[2]);  
}

