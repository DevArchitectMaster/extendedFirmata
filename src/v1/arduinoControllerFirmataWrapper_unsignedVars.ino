
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <Firmata.h>
#include <LiquidCrystal.h>
#include <DHT.h>
// EDIT: //
/*
#include <SensorLibrary.h>
*/

/****************************************************************************/
/*                           I N I T   S E N S O R S                        */
/****************************************************************************/

// init LiquidCrystal lcd screen
LiquidCrystal lcd(5, 6, 7, 8, 9, 10);
int lastLine = 1;

// init dht sensor
#define DHTTYPE DHT22
int dhtpin = 2;
DHT dht(dhtpin, DHTTYPE);

// EDIT: //
/*
SensorLibrary sl(pin1);
*/


/****************************************************************************/
/*                              F U N C T I O N S                           */
/****************************************************************************/

// control code enum
enum eControlKey {operationKey, callnumberKey, resultKey};
// control code pattern
unsigned char ustrPattern[] = "###";
// length of the strings to be transmitted
// select 0 as longer if incoming and outgoing strings should not be truncated
unsigned int uiResponseLength = 32;

void setup() {
  Serial.begin(9600);
  // setup lcd
  lcd.begin(16,2);
  //lcd.init();
  //lcd.backlight();
  // setup sensors
  dht.begin();
  // EDIT: //
  /*
  sensor.begin();
  */

  // setup firmata
  Firmata.setFirmwareVersion(FIRMATA_MAJOR_VERSION, FIRMATA_MINOR_VERSION);
  // register string callback function
  Firmata.attach(STRING_DATA, stringDataCallback);
  Firmata.begin();  
}

void loop() {
  while(Firmata.available()) {
    Firmata.processInput();
  }
}

// string callback function
void stringDataCallback(char *stringData) {
  // due to the processor architecture a maximum of 32-digit strings can be sent, therefore longer strings are shortened here
  if(uiResponseLength != 0) {
    stringData[uiResponseLength] = '\0';
  }

  unsigned char *pucOperation;
  unsigned char *pucCallnumber;
  unsigned char *pucResult;
 
  int iOperation; // 4 Byte
  int iCallnumber; // 4 Byte
  double dResult; // 8 Byte [15-digit]

  if(validateInput(stringData, ustrPattern) == 4) {
    pucOperation = getKeyValue(operationKey, stringData, ustrPattern);
    pucCallnumber = getKeyValue(callnumberKey, stringData, ustrPattern);
    //pucResult = getKeyValue(resultKey, stringData, ustrPattern);
    
    iOperation = strtof(pucOperation, NULL);
    iCallnumber = strtof(pucCallnumber, NULL);
    //dResult = strtof(pucResult, NULL);
    dResult = getSensorValue(iOperation);
    
    // round
    unsigned int uiDigits = 2;
    dResult = (ceil(dResult*(10<<uiDigits-1)))/(10<<uiDigits-1);
    
    //sprintf(pucOperation, "%i", iOperation);
    //sprintf(pucCallnumber, "%i", iCallnumber);
    // convert 64 bit (8 byte) double in 64 bit unsigned char array
    unsigned char *pucResultTEMP;
    pucResultTEMP = malloc(sizeof(dResult));
    pucResult = malloc(sizeof(sprintf(pucResultTEMP, "%0.*f", uiDigits, dResult)));
    pucResultTEMP = NULL;
    free(pucResultTEMP);
    sprintf(pucResult, "%0.*f", uiDigits, dResult);

    unsigned char *pucResponse = malloc(strlen(ustrPattern) + strlen(pucOperation) + strlen(ustrPattern) + strlen(pucCallnumber) + strlen(ustrPattern) + strlen(pucResult) + strlen(ustrPattern) + 1); // +1 for the null-terminator
    strcpy(pucResponse, ustrPattern);
    strcat(pucResponse, pucOperation);
    strcat(pucResponse, ustrPattern);
    strcat(pucResponse, pucCallnumber);
    strcat(pucResponse, ustrPattern);
    strcat(pucResponse, pucResult);
    strcat(pucResponse, ustrPattern);
    strcat(pucResponse, "");
    
    // due to the processor architecture a maximum of 32-digit strings can be sent, therefore longer strings are shortened here
    if(uiResponseLength != 0) {
      pucResponse[uiResponseLength] = '\0';
    }
    Firmata.sendString(pucResponse);
  } else {
      // ERROR
      showOnLcd(stringData);
  }
}



/****************************************************************************/
/*                         S E N S O R S - C O N F I G                      */
/****************************************************************************/

unsigned int validateInput(unsigned char *pucInputString, unsigned char *pucPattern) {
  unsigned int iCounter = 0;
  unsigned char *pucPointerStart = pucInputString;
  unsigned int uiLengthPattern = strlen(pucPattern);
  unsigned int uiLoopCondition = 1;
  while(uiLoopCondition) {
    pucPointerStart = strstr(pucPointerStart, pucPattern);
    if(pucPointerStart == NULL) {
      uiLoopCondition = 0;
    } else {
      pucPointerStart = pucPointerStart+uiLengthPattern;
      iCounter++;
    }
  }
  return iCounter;
}

unsigned char * getKeyValue(enum eControlKey eRequestKey, unsigned char *pucInputString, unsigned char *pucPattern) {
  unsigned char *pucPointerStart;
  unsigned char *pucPointerEnd;
  unsigned int uiLengthPattern = strlen(pucPattern);
  unsigned int uiAllocLength;
  unsigned char *pucValue;
  int iOffset = 0;
  
  for(int i=0; i <= eRequestKey; i++) {
    pucPointerStart = strstr(pucInputString+iOffset, pucPattern) + uiLengthPattern;
    pucPointerEnd = strstr(pucPointerStart, pucPattern);
    uiAllocLength = pucPointerEnd - pucPointerStart;
    iOffset = (i+1) * uiLengthPattern;
  }
  pucValue = malloc(uiAllocLength+1);
  strncpy(pucValue, pucPointerStart, uiAllocLength);
  pucValue[uiAllocLength] = '\0';

  return pucValue;
}

double getRandomNumber(double dMin, double dMax) {
  double dRange = (dMax - dMin); 
  double dDiv = RAND_MAX / dRange;
  srand(time(NULL));
  return dMin + (rand() / dDiv); //(double) rand() / RAND_MAX * 2.0 - 1.0; // float in range -1 to 1
}

double getSensorValue(int iInputSensorId) {
  double dSensorValue;

  switch(iInputSensorId) {
    // read random sensor value
    case 1:
      dSensorValue = getRandomNumber(-10, 10);
      break;
    // read humidity
    case 2:
      dSensorValue = dht.readHumidity();
      break;
    // read temperature as Celsius
    case 3:
      dSensorValue = dht.readTemperature();
      break;
    // compute heat index in Celsius
    case 4:
      dSensorValue = dht.computeHeatIndex(dht.readTemperature(), dht.readHumidity(), false);
      break;
    // EDIT: //
    /*
    case X:
      dSensorValue = sensor.read();
      break;
    */
    // error case
    default:
      printf("\n\n########################################");
      printf("\n### ERROR 404: Sensor <%i> not found! ###", iInputSensorId);
      printf("\n########################################\n\n");
      dSensorValue = 0.0;
      break;
  }
  return dSensorValue;
}



/****************************************************************************/
/*                                D I S P L A Y                             */
/****************************************************************************/

void showOnLcd(char *pucInputString) {
  if(lastLine) {
    lcd.clear();
    lastLine = 0;
  } else {
    lcd.setCursor(2,1);
    lastLine = 1;
  }
  lcd.print(pucInputString);
}



/****************************************************************************/
/*                                S E N S O R S                             */
/****************************************************************************/

