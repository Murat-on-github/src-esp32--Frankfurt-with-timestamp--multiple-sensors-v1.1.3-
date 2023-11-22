#ifndef CONFIG
#define CONFIG

/**
 * Set this to false to disable Serial logging
 */
#define DEBUG true

/**
 * The name of this device (as defined in the AWS IOT console).
 * Also used to set the hostname on the network
 */
#define DEVICE_NAME "ElectraFrankfurt"

/**
 * ADC input pin that is used to read out the CT sensor
 */
#define ADC_INPUT1 36
#define ADC_INPUT2 39


/**
 * The voltage of your home, used to calculate the wattage.
 * Try setting this as accurately as possible.
 */
#define HOME_VOLTAGE 238.0 //was 245

/**
 * WiFi credentials
 */
//#define WIFI_NETWORK "Giraffe"
//#define WIFI_PASSWORD "Simon@1973!"
//#define WIFI_NETWORK "TP-Link_766E"
//#define WIFI_PASSWORD "49601459"
//#define WIFI_NETWORK "MakerspaceMocap"
//#define WIFI_PASSWORD "M@kersp@ce-@"
#define WIFI_NETWORK "GLX MKH"
#define WIFI_PASSWORD "rs4nzjquy6x8mti"



/**
 * Timeout for the WiFi connection. When this is reached,
 * the ESP goes into deep sleep for 30seconds to try and
 * recover.
 */
#define WIFI_TIMEOUT 20000 // 20 seconds

/**
 * How long should we wait after a failed WiFi connection
 * before trying to set one up again.
 */
#define WIFI_RECOVER_TIME_MS 20000 // 20 seconds

/**
 * Dimensions of the OLED display attached to the ESP
 */
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

/**
 * Force Emonlib to assume a 3.3V supply to the CT sensor
 */
#define emonTxV3 1


/**
 * Local measurements
 */
#define LOCAL_MEASUREMENTS 30 //MK:Was 30 (this is kind of zero readings)


/**
 * The MQTT endpoint of the service we should connect to and receive messages
 * from. 
 * MK changed to false, if AWS connection is  needed-change back to true 
 * Alos eadded endpoint, (IOT topic is not clear to me)
 */
#define AWS_ENABLED true
#define AWS_IOT_ENDPOINT "a1exx4me3fk1it-ats.iot.eu-central-1.amazonaws.com"
#define AWS_IOT_TOPIC "iot:ElectraFrankfurt"

#define MQTT_CONNECT_DELAY 200
#define MQTT_CONNECT_TIMEOUT 20000 // 20 seconds


/**
 * Syncing time with an NTP server
 */
#define NTP_TIME_SYNC_ENABLED true
#define NTP_SERVER "pool.ntp.org"
#define NTP_OFFSET_SECONDS 3600
#define NTP_UPDATE_INTERVAL_MS 60000

/**
 * Wether or not you want to enable Home Assistant integration
 */
#define HA_ENABLED false
#define HA_ADDRESS "*** YOUR HOME ASSISTANT IP ADDRESSS ***"
#define HA_PORT 8883
#define HA_USER "*** MQTT USER ***"
#define HA_PASSWORD "*** MQTT PASSWORD ***"

// Check which core Arduino is running on. This is done because updating the 
// display only works from the Arduino core.
#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif
#endif