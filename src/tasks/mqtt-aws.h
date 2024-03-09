#ifndef TASK_MQTT_AWS
#define TASK_MQTT_AWS

#if AWS_ENABLED == true
    #include <Arduino.h>
    #include <WiFiClientSecure.h>
    #include <MQTTClient.h>
    #include "../config/config.dist.h"

    extern unsigned short measurements1[];
    extern unsigned short measurements2[];
    extern DisplayValues gDisplayValues; //MKcode Added this line to access gDisplayValues struct


    #define AWS_MAX_MSG_SIZE_BYTES 300

    WiFiClientSecure AWS_net;
    MQTTClient AWS_mqtt = MQTTClient(AWS_MAX_MSG_SIZE_BYTES);

    extern const uint8_t aws_root_ca_pem_start[] asm("_binary_certificates_amazonrootca1_pem_start");
    extern const uint8_t aws_root_ca_pem_end[] asm("_binary_certificates_amazonrootca1_pem_end");
//below iI substituded crt with key
    extern const uint8_t certificate_pem_crt_start[] asm("_binary_certificates_certificate_pem_crt_start");
    extern const uint8_t certificate_pem_crt_end[] asm("_binary_certificates_certificate_pem_crt_end");

    extern const uint8_t private_pem_key_start[] asm("_binary_certificates_private_pem_key_start");
    extern const uint8_t private_pem_key_end[] asm("_binary_certificates_private_pem_key_end");

    void keepAWSConnectionAlive(void * parameter){
        for(;;){
            if(AWS_mqtt.connected()){
                AWS_mqtt.loop();
                vTaskDelay(500 / portTICK_PERIOD_MS);
                continue;
            }

            if(!WiFi.isConnected()){
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                continue;
            }

            // Configure certificates
            AWS_net.setCACert((const char *) aws_root_ca_pem_start);
            AWS_net.setCertificate((const char *) certificate_pem_crt_start);
            AWS_net.setPrivateKey((const char *) private_pem_key_start);

            serial_println(F("[MQTT] Connecting to AWS..."));
            AWS_mqtt.begin(AWS_IOT_ENDPOINT, 8883, AWS_net);

            long startAttemptTime = millis();
        
            while (!AWS_mqtt.connect(DEVICE_NAME, HA_USER, HA_PASSWORD) &&
                    millis() - startAttemptTime < MQTT_CONNECT_TIMEOUT)
            {
                vTaskDelay(MQTT_CONNECT_DELAY);
            }

            if(!AWS_mqtt.connected()){
                serial_println(F("[MQTT] AWS connection timeout. Retry in 30s."));
                vTaskDelay(30000 / portTICK_PERIOD_MS);
            }

            serial_println(F("[MQTT] AWS Connected!"));
        }
    }

    /**
     * TASK: Upload measurements to AWS. This only works when there are enough
     * local measurements. It's called by the measurement function.
     */
    void uploadMeasurementsToAWS(void * parameter){
        if(!WiFi.isConnected() || !AWS_mqtt.connected()){
            serial_println("[MQTT] AWS: no connection. Discarding data..");
            vTaskDelete(NULL);
        }
        //MK Code start
         unsigned long sum = 0; // This will hold the sum of all measurements

        for (short i = 0; i < LOCAL_MEASUREMENTS; i++){
            sum += measurements1[i];
         }
        //MK Code finish
    double average = (double)sum / LOCAL_MEASUREMENTS; // This will compute the average

        char msg[AWS_MAX_MSG_SIZE_BYTES];
        char msg2[AWS_MAX_MSG_SIZE_BYTES];
        
        strcpy(msg, "{\"readings\":[");
        strcpy(msg2, "{\"readings2\":[");

        for (short i = 0; i < LOCAL_MEASUREMENTS-1; i++){
            strcat(msg, String(measurements1[i]).c_str());
            strcat(msg, ",");
        }

        for (short i = 0; i < LOCAL_MEASUREMENTS-1; i++){
            strcat(msg2, String(measurements2[i]).c_str());
            strcat(msg2, ",");
        }

        strcat(msg, String(measurements1[LOCAL_MEASUREMENTS-1]).c_str());
        strcat(msg, "],\"start_time\":\""); //MKcode: modified slightly contents of the string
        //MKcodestart
        strcat(msg, gDisplayValues.beginning_timestamp.c_str());
        strcat(msg, "\",\"end_time\":\"");
        strcat(msg, gDisplayValues.ending_timestamp.c_str());
         strcat(msg, "\",\"sum\":"); //added
        strcat(msg, String(sum).c_str()); //added
        strcat(msg, ",\"average\":"); //added
        strcat(msg, String(average).c_str()); //added
        strcat(msg, "\"");
        //MKcodefinish

        strcat(msg2, String(measurements2[LOCAL_MEASUREMENTS-1]).c_str());
            
        serial_print("[MQTT] AWS publish: ");
        serial_println(msg);
        serial_println(msg2);
        AWS_mqtt.publish(AWS_IOT_TOPIC, msg);

        // Task is done!
        vTaskDelete(NULL);
    }
#endif
#endif
