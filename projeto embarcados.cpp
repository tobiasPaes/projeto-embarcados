#include <Arduino.h>
#include <Servo.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/queue.h"

Servo servo1;
Servo servo2;

const int rainSensor1Pin = 34; 
const int rainSensor2Pin = 35; 
const int servo1Pin = 18;      
const int servo2Pin = 19;      

QueueHandle_t rainQueue1;
QueueHandle_t rainQueue2;

TimerHandle_t rainSensor1Timer;
TimerHandle_t rainSensor2Timer;
TimerHandle_t servo1ControlTimer;
TimerHandle_t servo2ControlTimer;


void checkRainSensor1Callback(TimerHandle_t xTimer) {
  int sensorValue = analogRead(rainSensor1Pin);
  bool isRaining = (sensorValue < 500); 

  xQueueSend(rainQueue1, &isRaining, portMAX_DELAY);
}


void checkRainSensor2Callback(TimerHandle_t xTimer) {
  int sensorValue = analogRead(rainSensor2Pin);
  bool isRaining = (sensorValue < 500); 

  xQueueSend(rainQueue2, &isRaining, portMAX_DELAY);
}

void controlServo1Callback(TimerHandle_t xTimer) {
  bool isRaining;

  
  if (xQueueReceive(rainQueue1, &isRaining, portMAX_DELAY) == pdTRUE) {
    if (isRaining) {
      servo1.write(90);
    } else {
      servo1.write(0); 
    }
  }
}


void controlServo2Callback(TimerHandle_t xTimer) {
  bool isRaining;

  
  if (xQueueReceive(rainQueue2, &isRaining, portMAX_DELAY) == pdTRUE) {
    if (isRaining) {
      servo2.write(90); 
    } else {
      servo2.write(0);  
    }
  }
}

void setup() {
  servo1.attach(servo1Pin);
  servo2.attach(servo2Pin);

  rainQueue1 = xQueueCreate(10, sizeof(bool));
  rainQueue2 = xQueueCreate(10, sizeof(bool));

  if (rainQueue1 == NULL || rainQueue2 == NULL) {
    Serial.println("Falha ao criar filas");
    while (1);
  }

  rainSensor1Timer = xTimerCreate(
      "Rain Sensor 1 Timer",             
      pdMS_TO_TICKS(500),                 
      pdTRUE,                             
      (void *)0,                          
      checkRainSensor1Callback);          

  
  rainSensor2Timer = xTimerCreate(
      "Rain Sensor 2 Timer",              
      pdMS_TO_TICKS(500),                 
      pdTRUE,                            
      (void *)0,                          

  servo1ControlTimer = xTimerCreate(
      "Servo 1 Control Timer",           
      pdMS_TO_TICKS(100),                 
      pdTRUE,                             
      (void *)0,                         
      controlServo1Callback);             

  
  servo2ControlTimer = xTimerCreate(
      "Servo 2 Control Timer",           
      pdMS_TO_TICKS(100),                
      pdTRUE,                             
      (void *)0,                         
      controlServo2Callback);             

  
  if (rainSensor1Timer != NULL) {
    xTimerStart(rainSensor1Timer, 0);
  }

  if (rainSensor2Timer != NULL) {
    xTimerStart(rainSensor2Timer, 0);
  }

  if (servo1ControlTimer != NULL) {
    xTimerStart(servo1ControlTimer, 0);
  }

  if (servo2ControlTimer != NULL) {
    xTimerStart(servo2ControlTimer, 0);
  }
}

void loop() {
  
}