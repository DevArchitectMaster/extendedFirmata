
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

// unused analog pin for random seed
int randAnalogPin = 0;

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
char ustrPattern[] = "###";
// length of the strings to be transmitted
// select 0 as longer if incoming and outgoing strings should not be truncated
int uiResponseLength = 32;

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(randAnalogPin));
  // setup lcd
  lcd.begin(16, 2);
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
  while (Firmata.available()) {
    Firmata.processInput();
  }
}

// string callback function
void stringDataCallback(char *stringData) {
  // due to the processor architecture a maximum of 32-digit strings can be sent, therefore longer strings are shortened here
  if (uiResponseLength != 0) {
    stringData[uiResponseLength] = '\0';
  }

  char *pucOperation;
  char *pucCallnumber;
  char *pucResult;
char pucTEMP[16];

  int iOperation; // 4 Byte
  int iCallnumber; // 4 Byte
  double dResult; // 8 Byte [15-digit]

  if (validateInput(stringData, ustrPattern) == 4) {
    pucOperation = getKeyValue(operationKey, stringData, ustrPattern);
    pucCallnumber = getKeyValue(callnumberKey, stringData, ustrPattern);
    //pucResult = getKeyValue(resultKey, stringData, ustrPattern);

/*
Firmata.sendString("INPUT:");
Firmata.sendString("pucOperation");
Firmata.sendString(pucOperation);
Firmata.sendString("pucCallnumber");
Firmata.sendString(pucCallnumber);
Firmata.sendString("---------------------------");
*/

    iOperation = atoi(pucOperation);
    iCallnumber = atoi(pucCallnumber);
    //dResult = atoi(pucResult);
    dResult = getSensorValue(iOperation);

    // round
    int uiDigits = 2;
    dResult = (ceil(dResult * (10 << uiDigits - 1))) / (10 << uiDigits - 1);

/*
//sprintf(pucOperation, "%i", iOperation);
//sprintf(pucCallnumber, "%i", iCallnumber);
// convert 64 bit (8 byte) double in 64 bit char array
char *pucResultTEMP;
pucResultTEMP = (char *)malloc(sizeof(dResult));
pucResult = (char *)malloc(sizeof(sprintf(pucResultTEMP, "%0.*f", uiDigits, dResult)));
pucResultTEMP = NULL;
free(pucResultTEMP);
sprintf(pucResult, "%0.*f", uiDigits, dResult);
*/
//dtostrf(iOperation, 16, 0, pucTEMP);
//pucOperation = trim(pucTEMP);
//    itoa(iOperation, pucOperation, 10);
pucOperation = getKeyValue(operationKey, stringData, ustrPattern);
//dtostrf(iCallnumber, 16, 0, pucTEMP);
//pucCallnumber = trim(pucTEMP);
//    itoa(iCallnumber, pucCallnumber, 10);
pucCallnumber = getKeyValue(callnumberKey, stringData, ustrPattern);
dtostrf(dResult, 16, 2, pucTEMP);
pucResult = trim(pucTEMP);

    char *pucResponse = (char *)malloc(strlen(ustrPattern) + strlen(pucOperation) + strlen(ustrPattern) + strlen(pucCallnumber) + strlen(ustrPattern) + strlen(pucResult) + strlen(ustrPattern) + 1); // +1 for the null-terminator
//char pucResponse[32];
    strcpy(pucResponse, ustrPattern);
    strcat(pucResponse, pucOperation);
    strcat(pucResponse, ustrPattern);
    strcat(pucResponse, pucCallnumber);
    strcat(pucResponse, ustrPattern);
    strcat(pucResponse, pucResult);
    strcat(pucResponse, ustrPattern);
    strcat(pucResponse, "");

/*
Firmata.sendString("OUTPUT:");
Firmata.sendString("pucOperation");
Firmata.sendString(pucOperation);
Firmata.sendString("pucCallnumber");
Firmata.sendString(pucCallnumber);
Firmata.sendString("---------------------------");
*/

    // due to the processor architecture a maximum of 32-digit strings can be sent, therefore longer strings are shortened here
    if (uiResponseLength != 0) {
      pucResponse[uiResponseLength] = '\0';
    }
    Firmata.sendString(pucResponse);
  } else {
    // ERROR
    showOnLcd(stringData);
  }
}

char *ltrim(char *s) {
  while (isspace(*s)) s++;
  return s;
}

char *rtrim(char *s) {
  char *back = s + strlen(s);
  while (isspace(*--back));
  *(back + 1) = '\0';
  return s;
}

char *trim(char *s) {
  return rtrim(ltrim(s));
}



/****************************************************************************/
/*                         S E N S O R S - C O N F I G                      */
/****************************************************************************/

int validateInput(char *pucInputString, char *pucPattern) {
  int iCounter = 0;
  char *pucPointerStart = pucInputString;
  int uiLengthPattern = strlen(pucPattern);
  int uiLoopCondition = 1;
  while (uiLoopCondition) {
    pucPointerStart = strstr(pucPointerStart, pucPattern);
    if (pucPointerStart == NULL) {
      uiLoopCondition = 0;
    } else {
      pucPointerStart = pucPointerStart + uiLengthPattern;
      iCounter++;
    }
  }
  return iCounter;
}

char * getKeyValue(enum eControlKey eRequestKey, char *pucInputString, char *pucPattern) {
  char *pucPointerStart;
  char *pucPointerEnd;
  int uiLengthPattern = strlen(pucPattern);
  int uiAllocLength;
  char *pucValue;
  int iOffset = 0;

  for (int i = 0; i <= eRequestKey; i++) {
    pucPointerStart = strstr(pucInputString + iOffset, pucPattern) + uiLengthPattern;
    pucPointerEnd = strstr(pucPointerStart, pucPattern);
    uiAllocLength = pucPointerEnd - pucPointerStart;
    iOffset = (i + 1) * uiLengthPattern;
  }
  pucValue = (char *)malloc(uiAllocLength + 1);
  strncpy(pucValue, pucPointerStart, uiAllocLength);
  pucValue[uiAllocLength] = '\0';

  return pucValue;
}

double getRandomNumber(double dMin, double dMax) {
  /*
  double dRange = (dMax - dMin);
  double dDiv = RAND_MAX / dRange;
  srand(time(NULL));
  return dMin + (rand() / dDiv); //(double) rand() / RAND_MAX * 2.0 - 1.0; // float in range -1 to 1
  */
  return random(dMin, dMax);
}

double getSensorValue(int iInputSensorId) {
  double dSensorValue;

  switch (iInputSensorId) {
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
  if (lastLine) {
    lcd.clear();
    lastLine = 0;
  } else {
    lcd.setCursor(2, 1);
    lastLine = 1;
  }
  lcd.print(pucInputString);
}



/****************************************************************************/
/*                                S E N S O R S                             */
/****************************************************************************/

