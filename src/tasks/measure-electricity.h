#ifndef TASK_MEASURE_ELECTRICITY
#define TASK_MEASURE_ELECTRICITY

#include <Arduino.h>
#include "EmonLib.h"

#include "../config/config.dist.h"
#include "../config/enums.h"
#include "mqtt-aws.h"
#include "mqtt-home-assistant.h"

extern DisplayValues gDisplayValues;
extern EnergyMonitor emon1;
extern EnergyMonitor emon2;
extern unsigned short measurements1[];
extern unsigned short measurements2[];
extern unsigned char measureIndex; //? what does this part do

void measureElectricity(void * parameter)
{
    for(;;){
      serial_println("[ENERGY] Measuring...");
      long start = millis();//what does this line do?? - it triggeres a timer start to monitor how long the loop of measuring take time.
// MKcodeadditionstart: If this is the first measurement in the set, record the timestamp
      if(measureIndex == 0){
          gDisplayValues.beginning_timestamp = timeClient.getFormattedDate(); // Updating the beginning timestamp
      }
// MKcodeadditionfinish
      //read from first sensor
      double amps1 = emon1.calcIrms(1480);
      double watts1 = amps1 * HOME_VOLTAGE;
      gDisplayValues.amps = amps1;
      gDisplayValues.watt = watts1;
      measurements1[measureIndex] = watts1;

      //read from second sensor
      double amps2 = emon2.calcIrms(1480);
      double watts2 = amps2 * HOME_VOLTAGE;
      measurements2[measureIndex] = watts2;
      
      measureIndex++;

      if(measureIndex == LOCAL_MEASUREMENTS){
          // MKcodeadditionstart: 
          gDisplayValues.ending_timestamp = timeClient.getFormattedDate(); // Updating the ending timestamp
          // MKcodeadditionfinish
          #if AWS_ENABLED == true
            xTaskCreate(
              uploadMeasurementsToAWS,
              "Upload measurements to AWS",
              10000,             // Stack size (bytes)
              NULL,             // Parameter
              5,                // Task priority
              NULL              // Task handle
            );
          #endif

          #if HA_ENABLED == true
            xTaskCreate(
              sendEnergyToHA,
              "HA-MQTT Upload",
              10000,             // Stack size (bytes)
              NULL,             // Parameter
              5,                // Task priority
              NULL              // Task handle
            );
          #endif

          measureIndex = 0;
      }

      long end = millis();

      // Schedule the task to run again in 1 second (while
      // taking into account how long measurement took)
      vTaskDelay((1000-(end-start)) / portTICK_PERIOD_MS);
    }    
}

#endif
