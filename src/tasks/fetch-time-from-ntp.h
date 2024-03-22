#ifndef TASK_FETCH_TIME_NTP
#define TASK_FETCH_TIME_NTP

#if NTP_TIME_SYNC_ENABLED == true
    #include <Arduino.h>
    #include <WiFi.h>
    #include <NTPClient.h>
    #include <WiFiUdp.h>
    #include "../config/enums.h"
    #include "time.h"

    extern void reconnectWifiIfNeeded();
    extern DisplayValues gDisplayValues;

    WiFiUDP ntpUDP;

    // TODO: this does not take timezones into account! Only UTC for now.
    NTPClient timeClient(ntpUDP, NTP_SERVER, NTP_OFFSET_SECONDS, NTP_UPDATE_INTERVAL_MS);


    void fetchTimeFromNTP(void * parameter){
        for(;;){
            if(!WiFi.isConnected()){
                vTaskDelay(10*1000 / portTICK_PERIOD_MS);
                continue;
            }

            serial_println("[NTP] Updating...");
            timeClient.update();

            String timestring = timeClient.getFormattedTime();
            String timestring2= timeClient.getFormattedDate();
             
            short tIndex = timestring.indexOf("T");
            gDisplayValues.time = timestring.substring(tIndex + 1, timestring.length() -3);
        


            serial_println("[NTP] Done");
            serial_println(gDisplayValues.time);
            serial_println(gDisplayValues.time2);
            serial_println(timestring2);
            
            // Sleep for a minute before checking again
            vTaskDelay(NTP_UPDATE_INTERVAL_MS / portTICK_PERIOD_MS);
            //conclusion2: this code now produces timestamp but this timestamo is not used in MQTT message yet. It is only printed in the serial monitor. 
        }
    }
#endif
#endif
