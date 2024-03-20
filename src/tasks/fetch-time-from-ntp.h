#ifndef TASK_FETCH_TIME_NTP
#define TASK_FETCH_TIME_NTP

#if NTP_TIME_SYNC_ENABLED == true
    #include <Arduino.h>
    #include <WiFi.h>
    #include <NTPClient.h>
    #include <WiFiUdp.h>
    #include "../config/enums.h"
    #include <time.h>

    extern void reconnectWifiIfNeeded();
    extern DisplayValues gDisplayValues;

    WiFiUDP ntpUDP;

    // TODO: this does not take timezones into account! Only UTC for now.
    NTPClient timeClient(ntpUDP, NTP_SERVER, NTP_OFFSET_SECONDS, NTP_UPDATE_INTERVAL_MS);

    // Function to initialize and update the ESP32 system clock using SNTP
    void initialize_sntp() {
        configTime(0, 0, "pool.ntp.org", "time.nist.gov"); // Adjust for your timezone if needed
        Serial.println("[SNTP] Time sync initiated with NTP servers.");
    }

// Function to get the current date and time in AWS Redshift compatible format
    String getAWSRedshiftTimestamp() {
    time_t now = time(nullptr); // Get the current time as a time_t object
    struct tm timeStruct; 
    gmtime_r(&now, &timeStruct); // Convert time_t to tm as UTC time

    char timestamp[20]; // Buffer to hold the formatted timestamp
    // Format the timeStruct into a string in the format YYYY-MM-DD HH:MM:SS
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &timeStruct);

    return String(timestamp); // Convert char array to String and return
}
    void fetchTimeFromNTP(void * parameter){
        for(;;){
            if(!WiFi.isConnected()){
                vTaskDelay(10*1000 / portTICK_PERIOD_MS);
                continue;
            }

            serial_println("[NTP] Updating...");
            timeClient.update();

            String timestring = timeClient.getFormattedTime();
            short tIndex = timestring.indexOf("T");
            gDisplayValues.time = timestring.substring(tIndex + 1, timestring.length() -3);
            

// New functionality to get timestamp in AWS Redshift compatible format
            String awsRedshiftTimestamp = getAWSRedshiftTimestamp();
            // Assuming you have a way to store or use this new timestamp
            // For example, storing it in a global structure or printing it
            serial_println("[NTP] AWS Redshift Timestamp: " + awsRedshiftTimestamp);

            serial_println("[NTP] Done");
            serial_println(gDisplayValues.time);
            serial_println(gDisplayValues.time2);



            // Sleep for a minute before checking again
            vTaskDelay(NTP_UPDATE_INTERVAL_MS / portTICK_PERIOD_MS);
        }
    }
#endif
#endif
