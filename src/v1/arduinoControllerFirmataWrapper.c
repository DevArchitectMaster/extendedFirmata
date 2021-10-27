#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// control code enum
enum eControlKey {operationKey, callnumberKey, resultKey};
// control code pattern
unsigned char ustrPattern[] = "###";
// length of the strings to be transmitted
// select 0 as longer if incoming and outgoing strings should not be truncated
unsigned int uiResponseLength = 32;

unsigned int validateInput(unsigned char *inputString, unsigned char *pattern);
unsigned char * getKeyValue(enum eControlKey eRequestKey, unsigned char *inputString, unsigned char *pattern);
double getRandomNumber(double min, double max);
double getSensorValue(int inputSensorId);

int main () {
  unsigned char stringData[] = "###1###-23###-789###";

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
    
    printf("\n########################################");
    printf("\noperation: %s", pucOperation);
    printf("\ncallnumber: %s", pucCallnumber);
    //printf("\nresult: %s", pucResult);
    printf("\n########################################\n");
    
    /*
    iOperation = strtof(pucOperation, NULL);
    iCallnumber = strtof(pucCallnumber, NULL);
    //dResult = strtof(pucResult, NULL);
    dResult = getSensorValue(iOperation);
    */
    iOperation = atoi(pucOperation);
    iCallnumber = atoi(pucCallnumber);
    //dResult = atoi(pucResult);
    dResult = getSensorValue(iOperation);
    
    // round
    unsigned int uiDigits = 2;
    dResult = (ceil(dResult*(10<<uiDigits-1)))/(10<<uiDigits-1);
printf("\nRC: %f", dResult);
    
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
    // => Firmata.sendString(pucResponse);
    printf("\nRequest: %s", stringData);
    printf("\nResponse: %s", pucResponse);
  } else {
      // ERROR
      printf("ERROR");
      // => send to lcd display
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
      //TODO:dht.readHumidity();
      dSensorValue = 111.23456789;
      break;
    // read temperature as Celsius
    case 3:
      //TODO:dht.readTemperature();
      dSensorValue = -10.10;
      break;
    // compute heat index in Celsius
    case 4:
      //TODO:dht.computeHeatIndex(dht.readTemperature(), dht.readHumidity(), false);
      dSensorValue = 15.15;
      break;
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