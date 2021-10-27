#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

enum key {operationKey, callnumberKey, resultKey} k;

unsigned char* concat(const char *s1, const char *s2);
unsigned int validateInput(unsigned char *inputString, unsigned char *pattern);
unsigned int getValueContent(unsigned char *inputString, unsigned char *pattern, unsigned char *value);
unsigned char * getKeyValue(enum key k, unsigned char *inputString, unsigned char *pattern);
double getSensorValue(int inputSensorId);

int main () {
  unsigned char string[] = "###1###3###9###";
  unsigned char pattern[] = "###";
  unsigned char operationSTR[99];
  unsigned char callnumberSTR[99];
  unsigned char resultSTR[99];
  unsigned char * pucOperation;
  unsigned char * pucCallnumber;
  unsigned char * pucResult;
  
  unsigned int offset;
  int operation;
  int callnumber;
  double result;
  
  if(validateInput(string, pattern) == 4) {
      k = operationKey;
      pucOperation = getKeyValue(k, string, pattern);
      printf("\nNEW: %s", pucOperation);
      k = callnumberKey;
      pucCallnumber = getKeyValue(k, string, pattern);
      printf("\nNEW: %s", pucCallnumber);
      k= resultKey;
      pucResult = getKeyValue(k, string, pattern);
      printf("\nNEW: %s", pucResult);
      
      
    offset = 0;
    offset = getValueContent(string+offset, pattern, operationSTR) + offset;
    offset = getValueContent(string+offset, pattern, callnumberSTR) + offset;
    offset = getValueContent(string+offset, pattern, resultSTR) + offset;
    
    printf("\n########################################");
    printf("\noperation: %s", operationSTR);
    printf("\ncallnumber: %s", callnumberSTR);
    printf("\nresult: %s", resultSTR);
    printf("\n########################################\n");
    
    operation = strtof(operationSTR, NULL);
    callnumber = strtof(callnumberSTR, NULL);
    //result = strtof(resultSTR, NULL);
    result = getSensorValue(operation);
    // round
    unsigned int digits = 2;
    result = (ceil(result*(10<<digits)))/(10<<digits);
    printf("\nRC: %f", result);
    
    sprintf(operationSTR, "%i", operation);
    sprintf(callnumberSTR, "%i", callnumber);
    sprintf(resultSTR, "%f", result);
    //TODO: resultSTR richtig groß machen vom speicher her
    unsigned char *pointPointer;
    pointPointer = strstr(resultSTR, ".")+1+digits;
printf("\npointPointer: %s", pointPointer);
    //strncpy(resultSTR, resultSTR, strlen(resultSTR) - strlen(pointPointer));
    resultSTR[strlen(resultSTR) - strlen(pointPointer)] = '\0';
printf("\nSTRING: %s", resultSTR);

    unsigned char *response = malloc(strlen(pattern) + strlen(operationSTR) + strlen(pattern) + strlen(callnumberSTR) + strlen(pattern) + strlen(resultSTR) + strlen(pattern) + 1); // +1 for the null-terminator
    strcpy(response, pattern);
    strcat(response, operationSTR);
    strcat(response, pattern);
    strcat(response, callnumberSTR);
    strcat(response, pattern);
    strcat(response, resultSTR);
    strcat(response, pattern);
    strcat(response, "");
    
    printf("\nRequest: %s", string);
    printf("\nResponse: %s", response);
  } else {
      // ERROR
      printf("ERROR");
  }
}



unsigned int validateInput(unsigned char *inputString, unsigned char *pattern) {
  unsigned int counter = 0;
  unsigned char *pointerStart = inputString;
  unsigned int lengthPattern = strlen(pattern);
  unsigned int loopCondition = 1;
  while(loopCondition) {
    pointerStart = strstr(pointerStart, pattern);
    if(pointerStart == NULL) {
      loopCondition = 0;
    } else {
      pointerStart = pointerStart+lengthPattern;
      counter++;
    }
  }
  return counter;
}

unsigned int getValueContent(unsigned char *inputString, unsigned char *pattern, unsigned char *value) {
  unsigned char *pointerStart;
  unsigned char *pointerEnd;
  unsigned char *start;
  unsigned int length;
  unsigned int lengthPattern = strlen(pattern);

  pointerStart = strstr(inputString, pattern)+lengthPattern;
  if(!pointerStart) {
    return 0;
  }
  length = pointerStart - inputString;
  start = inputString+length;
  pointerEnd = strstr(inputString+length, pattern);

  strncpy(value, start, strlen(start) - strlen(pointerEnd));
  value[strlen(start) - strlen(pointerEnd)] = '\0';
  
  return pointerEnd-inputString;
}

unsigned char * getKeyValue(enum key k, unsigned char *inputString, unsigned char *pattern) {
  unsigned char *pointerStart;
  unsigned char *pointerEnd;
  unsigned int lengthPattern = strlen(pattern);
  unsigned int allocLength;
  unsigned char *value;
  unsigned int keyPosition;
  int offset = 0;
  
  
  for(int i=0; i <= k; i++) {
  pointerStart = strstr(inputString+offset, pattern) + lengthPattern;
  pointerEnd = strstr(pointerStart, pattern);
  allocLength = pointerEnd - pointerStart;
  offset = (i+1) * lengthPattern;
  }
  value = malloc(allocLength+1);
  strncpy(value, pointerStart, allocLength);
  value[allocLength] = '\0';

return value;
}

double getSensorValue(int inputSensorId) {
  double sensorValue;

  switch(inputSensorId) {
    // read humidity
    case 1:
      //TODO:dht.readHumiity();
      sensorValue = 111.23456789;
      break;
    // read temperature as Celsius
    case 2:
      //TODO:dht.readTemperature();
      sensorValue = -10.10;
      break;
    // compute heat index in Celsius
    case 3:
      //TODO:dht.computeHeatIndex(t, h, false);
      sensorValue = 15.15;
      break;
    // error case
    default:
      printf("\n\n########################################");
      printf("\n### ERROR 404: Sensor <%i> not found! ###", inputSensorId);
      printf("\n########################################\n\n");
      sensorValue = 0.0;
      break;
  }
  return sensorValue;
}

/****************************************************************************/
/*                                S E N S O R S                             */
/****************************************************************************/

