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

void measureElectricity(void * parameter) {
    bool timeSynced = false;
    for(;;) {
        // Only attempt to synchronize time and wait for the next full minute once
        if (!timeSynced) {
            String currentTime = timeClient.getFormattedDate();
            int year = currentTime.substring(0, 4).toInt(); // Extract the year part of the date string

            // Wait until the year is not 1970, indicating that the time has been synchronized
            while(year == 1970) {
                serial_println("[ENERGY] Waiting for time synchronization...");
                vTaskDelay(5000 / portTICK_PERIOD_MS);
                currentTime = timeClient.getFormattedDate();
                year = currentTime.substring(0, 4).toInt();
            }

            // Wait until the start of the next full minute
            int seconds = currentTime.substring(17, 19).toInt(); // Assuming format includes seconds here
            int delaySeconds = 60 - seconds;
            if (delaySeconds < 60) {
                serial_println("[ENERGY] Waiting for the next full minute...");
                vTaskDelay(delaySeconds * 1000 / portTICK_PERIOD_MS);
            }

            timeSynced = true; // Prevents re-entering this block
        }

        serial_println("[ENERGY] Measuring...");
        long start = millis(); // Start timing the measurement process

        // If this is the first measurement in the set, record the timestamp
        if(measureIndex == 0){
            gDisplayValues.beginning_timestamp = timeClient.getFormattedDate();
        }

        // Read from first sensor
        double amps1 = emon1.calcIrms(1480);
        double watts1 = amps1 * HOME_VOLTAGE;
        gDisplayValues.amps = amps1;
        gDisplayValues.watt = watts1;
        measurements1[measureIndex] = watts1;

        // Read from second sensor
        double amps2 = emon2.calcIrms(1480);
        double watts2 = amps2 * HOME_VOLTAGE;
        measurements2[measureIndex] = watts2;

        measureIndex++;

        if(measureIndex == LOCAL_MEASUREMENTS){
            gDisplayValues.ending_timestamp = timeClient.getFormattedDate(); // Record the ending timestamp

            // Handle AWS upload
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

            // Handle Home Assistant upload
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

            measureIndex = 0; // Reset for next set of measurements
        }

        long end = millis(); // End timing the measurement process

        // Schedule the task to run again in 1 second, accounting for measurement duration
        vTaskDelay((1000 - (end - start)) / portTICK_PERIOD_MS);
    }
}

#endif
