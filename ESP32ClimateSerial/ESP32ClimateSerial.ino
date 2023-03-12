/**************************************************************************************************************************************************
* File name     : ESP32ClimateSerial.c
* Compiler      : 
* Autor         : VIGASAN   
* Created       : 07/03/2023
* Modified      : 
* Last modified :
*
*
* Description   : 
*
* Other info    : Climate Control with  ESP32 Relays Board
**************************************************************************************************************************************************/


/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------Include Files----------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
#include <WiFi.h>
#include <PubSubClient.h>   //You need to change the value of costant MQTT_MAX_PACKET_SIZE to 600 in the file PubSubClient.h 
                            //because the MQTT payload could exceed standard value of 256
#include <ArduinoJson.h>
#include <OneWire.h>

/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------  Constants  ----------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
#define UNPRESSED                       0
#define DEBOUNCE                        1
#define PRESSED                         2

#define RELAY_OFF                       0
#define RELAY_ON                        1

#define SYSTEM_OFF                      0
#define SYSTEM_ON                       1

#define DELTA_HISTERESYS_TEMP           0.1
#define WATER_TEMP_THRESHOLD            28.0
#define HEATING_ST_OFF                  0
#define HEATING_ST_ON                   1


/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------I/O Definitions--------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
const int IN_DIN1 = 34;
const int IN_DIN2 = 35;
const int IN_DIN3 = 5;
const int IN_DIN4 = 4;
const int IN_DIN5 = 2;

const int OUT_RELAY1 = 32;
const int OUT_RELAY2 = 33;
const int OUT_RELAY3 = 25;
const int OUT_RELAY4 = 26;
const int OUT_RELAY5 = 27;
const int OUT_RELAY6 = 14;
const int OUT_RELAY7 = 13;
const int OUT_RELAY8 = 15;
const int OUT_RELAY9 = 16;
const int OUT_RELAY10 = 18;
const int OUT_RELAY11 = 19;
const int OUT_RELAY12 = 21;

const int DS18S20_PIN = 23;
const int OUT_STATUS_LED = 17;

/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------ SERIAL COMMUNICATION DEFINITIONS -------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
#define LODWORD(l)  (*( (unsigned long*)(&l)))
#define HIDWORD(l)  (*( ( (unsigned long*) (&l) ) + 1 ))
#define MAKEDWORD(hi,lo) ((unsigned long)(((unsigned long)(lo)) | (((unsigned long)(hi))<<16)))
//Word manipulation
#define LOWORD(l)   (*( (word*)(&l)))
#define HIWORD(l)   (*( ( (word*) (&l) ) + 1 ))
#define MAKEWORD(hi,lo) ((word)(((word)(lo)) | (((word)(hi))<<8)))
//Byte manipulation
#define LOBYTE(w)   (*( (byte*) (&w)))
#define HIBYTE(w)   (*( ( (byte*)  (&w) ) + 1 ))
#define UPBYTE(w)   (*( ( (byte*)  (&w) ) + 2 ))
#define MSBYTE(w)   (*( ( (byte*)  (&w) ) + 3 ))

#define GPL_ST_IDLE               0
#define GPL_ST_CMD                1
#define GPL_ST_NUM_BYTE           2
#define GPL_ST_DATA               3
#define GPL_ST_CHECKSUM           4

#define GPL_START_FRAME           0x8A
#define GPL_ESCAPE_CHAR           0x8B
#define GPL_XOR_CHAR              0x20     

#define GPL_BYTE_SOF              0
#define GPL_BYTE_CMD              1
#define GPL_BYTE_LENGTH           2
#define GPL_BYTE_FIRST_DATA       3

#define GPL_NUM_EXTRA_BYTES       4           

#define LEN_IN_BUFFER             50
#define LEN_OUT_BUFFER            30
#define LEN_OUT_FRAME_BUFFER      35

//---------------------------------------- SERIAL ID COMMANDS  -----------------------------------------------------------------------------------
#define CMD_INIT_SYSTEM           1
#define CMD_SYSTEM_STATUS         2
#define CMD_WATER_TEMP            3

#define CMD_ENV_TEMP_1            10
#define CMD_TARGET_TEMP_1         11
#define CMD_STATUS_HEATING_1		  12

#define CMD_ENV_TEMP_2            13
#define CMD_TARGET_TEMP_2         14
#define CMD_STATUS_HEATING_2      15

#define CMD_ENV_TEMP_3            16
#define CMD_TARGET_TEMP_3         17
#define CMD_STATUS_HEATING_3      18

#define CMD_ENV_TEMP_4            19
#define CMD_TARGET_TEMP_4         20
#define CMD_STATUS_HEATING_4      21

#define CMD_ENV_TEMP_5            22
#define CMD_TARGET_TEMP_5         23
#define CMD_STATUS_HEATING_5      24

#define CMD_ENV_TEMP_6            25
#define CMD_TARGET_TEMP_6         26
#define CMD_STATUS_HEATING_6      27


/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------MQTT DISCOVERY PARAMETERS----------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
const char* g_ssid = "yourWifiName";                              	// Wifi Name
const char* g_password = "wifipsw";                           		// Wifi Password
const char* g_mqtt_server = "192.168.1.25";                         // MQTT Server IP, same of Home Assistant
const char* g_mqttUser = "usermqtt";                                // MQTT Server User Name
const char* g_mqttPsw = "pswmqtt";                                 	// MQTT Server password
int         g_mqttPort = 1883;                                      // MQTT Server Port
const char* g_mqtt_DeviceName = "ClimaControl";          			// Device Name for Home Assistant

String      g_ClimateZone1 = "cucina";										// Climate Area Name
String      g_mqtt_Topic_Zone1 = "eps32relaysclima/" + g_ClimateZone1;		// Climate Area Topic
String      g_mqtt_Topic_SensorZone1 = "esp32iotsensor/TH_Cucina";			// Temperature sensor MQTT Topic to receive temperature data 

String      g_ClimateZone2 = "sala_tv";
String      g_mqtt_Topic_Zone2 = "eps32relaysclima/" + g_ClimateZone2;
String      g_mqtt_Topic_SensorZone2 = "esp32iotsensor/TH_SalaTV";

String      g_ClimateZone3 = "studio";
String      g_mqtt_Topic_Zone3 = "eps32relaysclima/" + g_ClimateZone3;
String      g_mqtt_Topic_SensorZone3 = "esp32iotsensor/TH_Studio";

String      g_ClimateZone4 = "cameretta";
String      g_mqtt_Topic_Zone4 = "eps32relaysclima/" + g_ClimateZone4;
String      g_mqtt_Topic_SensorZone4 = "esp32iotsensor/TH_Cameretta";

String      g_ClimateZone5 = "salone_st";
String      g_mqtt_Topic_Zone5 = "eps32relaysclima/" + g_ClimateZone5;
String      g_mqtt_Topic_SensorZone5 = "esp32iotsensor/TH_SaloneST";

String      g_ClimateZone6 = "bagno_st";
String      g_mqtt_Topic_Zone6 = "eps32relaysclima/" + g_ClimateZone6;
String      g_mqtt_Topic_SensorZone6 = "esp32iotsensor/TH_BagnoST";

String      g_mqttWaterTemp = "waterTemperature";
String      g_mqtt_Topic_WaterTemperature = "eps32relaysclima/" + g_mqttWaterTemp;

String      g_mqttSwitchStatus = "switchStatus";
String      g_mqtt_Topic_SwitchStatus = "eps32relaysclima/" + g_mqttSwitchStatus;

/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------Global variables-------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
WiFiClient          g_WiFiClient;
PubSubClient        g_mqttPubSub(g_WiFiClient);
OneWire 			g_tempSensor(DS18S20_PIN);		// Temperarature Probe 

unsigned long       g_TimeInputs = 0;
unsigned long       g_TimeLed = 0;
unsigned long       g_TimeTemperature = 0;
int                 g_mqttCounterConn = 0;
String              g_UniqueId;
bool                g_InitSystem = true;
int                 g_canPublish = 0;

byte                g_SystemStatus = SYSTEM_OFF;
byte                g_Input1 = 0;
byte                g_Input2 = 0;
byte                g_Input3 = 0;
byte                g_Input4 = 0;
byte                g_Input5 = 0;
byte                g_RelayStatus1 = RELAY_OFF;
byte                g_RelayStatus2 = RELAY_OFF;
byte                g_RelayStatus3 = RELAY_OFF;
byte                g_RelayStatus4 = RELAY_OFF;
byte                g_RelayStatus5 = RELAY_OFF;
byte                g_RelayStatus6 = RELAY_OFF;
byte                g_RelayStatus7 = RELAY_OFF;
byte                g_RelayStatus8 = RELAY_OFF;
byte                g_RelayStatus9 = RELAY_OFF;
byte                g_RelayStatus10 = RELAY_OFF;
byte                g_RelayStatus11 = RELAY_OFF;
byte                g_RelayStatus12 = RELAY_OFF;

byte                g_st_input1 = UNPRESSED;
byte                g_st_input2 = UNPRESSED;
byte                g_st_input3 = UNPRESSED;
byte                g_st_input4 = UNPRESSED;

float               g_WaterTemp = 30.0;
float               g_TargetTemp1 = 15.0;
float               g_TargetTemp2 = 15.0;
float               g_TargetTemp3 = 15.0;
float               g_TargetTemp4 = 15.0;
float               g_TargetTemp5 = 15.0;
float               g_TargetTemp6 = 15.0;

float               g_CurrentTemp1 = 40.0;
float               g_CurrentTemp2 = 40.0;
float               g_CurrentTemp3 = 40.0;
float               g_CurrentTemp4 = 40.0;
float               g_CurrentTemp5 = 40.0;
float               g_CurrentTemp6 = 40.0;

byte                st_Heating_Zone1 = HEATING_ST_OFF;
byte                st_Heating_Zone2 = HEATING_ST_OFF;
byte                st_Heating_Zone3 = HEATING_ST_OFF;
byte                st_Heating_Zone4 = HEATING_ST_OFF;
byte                st_Heating_Zone5 = HEATING_ST_OFF;
byte                st_Heating_Zone6 = HEATING_ST_OFF;


byte                g_InputBuffer[LEN_IN_BUFFER];
byte                g_OutputBuffer[LEN_OUT_BUFFER];
byte                g_FrameBuffer[LEN_OUT_FRAME_BUFFER];
byte                dataReceived = 0;
byte                gplStatus = GPL_ST_IDLE;
byte                xored = 0x00;
byte                checkSum;
int                 numByte = 0, i = 0;
bool                g_SerialDeviceIsOn = false;
bool                g_SerialDeviceConnected = false;

/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------ SETUP ----------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
void setup() 
{
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // I/O configuration
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    pinMode(OUT_STATUS_LED, OUTPUT);
    pinMode(IN_DIN1, INPUT);
    pinMode(IN_DIN2, INPUT);
    pinMode(IN_DIN3, INPUT);
    pinMode(IN_DIN4, INPUT);
    pinMode(IN_DIN5, INPUT);
    pinMode(OUT_RELAY1, OUTPUT);
    pinMode(OUT_RELAY2, OUTPUT);
    pinMode(OUT_RELAY3, OUTPUT);
    pinMode(OUT_RELAY4, OUTPUT);
    pinMode(OUT_RELAY5, OUTPUT);
    pinMode(OUT_RELAY6, OUTPUT);
    pinMode(OUT_RELAY7, OUTPUT);
    pinMode(OUT_RELAY8, OUTPUT);
    pinMode(OUT_RELAY9, OUTPUT);
    pinMode(OUT_RELAY10, OUTPUT);
    pinMode(OUT_RELAY11, OUTPUT);
    pinMode(OUT_RELAY12, OUTPUT);

    Serial.begin(115200);
    delay(500); 
  
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Wifi setup
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    setup_wifi();

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // MQTT setup
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    g_mqttPubSub.setServer(g_mqtt_server, g_mqttPort);
    g_mqttPubSub.setCallback(MqttReceiverCallback);
}

/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------ LOOP -----------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
void loop() 
{
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // MQTT Connection
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if(WiFi.status() == WL_CONNECTED)
    {
        if(!g_mqttPubSub.connected())
            MqttReconnect();
        else
            g_mqttPubSub.loop();
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // MQTT Discovery Init
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if(g_InitSystem)
    {
        delay(1000);
        g_InitSystem = false;
        MqttHomeAssistantDiscovery();

        delay(1000); 

        if(g_mqttPubSub.connected())
        {
            // Zona 1
            String topic = g_mqtt_Topic_Zone1 + "/state_temp";
            g_mqttPubSub.publish(topic.c_str(), String(g_TargetTemp1).c_str()); 
            MqttPublishStateZone1();

            // Zona 2
            topic = g_mqtt_Topic_Zone2 + "/state_temp";
            g_mqttPubSub.publish(topic.c_str(), String(g_TargetTemp2).c_str()); 
            MqttPublishStateZone2();

            // Zona 3
            topic = g_mqtt_Topic_Zone3 + "/state_temp";
            g_mqttPubSub.publish(topic.c_str(), String(g_TargetTemp3).c_str()); 
            MqttPublishStateZone3();

             // Zona 4
            topic = g_mqtt_Topic_Zone4 + "/state_temp";
            g_mqttPubSub.publish(topic.c_str(), String(g_TargetTemp4).c_str()); 
            MqttPublishStateZone4();

             // Zona 5
            topic = g_mqtt_Topic_Zone5 + "/state_temp";
            g_mqttPubSub.publish(topic.c_str(), String(g_TargetTemp5).c_str()); 
            MqttPublishStateZone5();

             // Zona 6
            topic = g_mqtt_Topic_Zone6 + "/state_temp";
            g_mqttPubSub.publish(topic.c_str(), String(g_TargetTemp6).c_str()); 
            MqttPublishStateZone6();

            MqttPublishStatusSwitchSystem();
        }
        
       
    }
	
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Serial Communication with Raspberry User Interface
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if (Serial.available() > 0)
    {
        dataReceived = (byte)Serial.read();
        if(dataReceived == GPL_ESCAPE_CHAR)
        {
            xored = GPL_XOR_CHAR;
        } else
        {
            dataReceived ^= xored;
            xored = 0x00;

            switch(gplStatus)
            {
                case GPL_ST_IDLE:
                {
                    if(dataReceived == GPL_START_FRAME)
                    {   
                        i = 0;
                        g_InputBuffer[i++] = dataReceived;
                        checkSum = dataReceived;
                        gplStatus = GPL_ST_CMD;
                    }
                } break;
    
                case GPL_ST_CMD:
                {
                    g_InputBuffer[i++] = dataReceived;
                    checkSum += dataReceived;
                    gplStatus = GPL_ST_NUM_BYTE;
                } break;
    
                case GPL_ST_NUM_BYTE:
                {
                    numByte = dataReceived;
                    if(numByte > 0)
                    {
                        g_InputBuffer[i++] = dataReceived;
                        checkSum += dataReceived;
                        gplStatus = GPL_ST_DATA;
                    } else
                    {   
                        gplStatus = GPL_ST_IDLE;
                    }
                } break;
    
                case GPL_ST_DATA:
                {
                    g_InputBuffer[i++] = dataReceived;
                    checkSum += dataReceived;
                    if(--numByte == 0)
                        gplStatus = GPL_ST_CHECKSUM;
                } break;
    
                case GPL_ST_CHECKSUM:
                {
                    if(dataReceived == checkSum)
                    {   
                        gplStatus = GPL_ST_IDLE;
                        g_InputBuffer[i++] = dataReceived;

                        switch(g_InputBuffer[GPL_BYTE_CMD])
                        {
                            case CMD_INIT_SYSTEM:
                            {
                                g_SerialDeviceConnected = true;
                                
                                GPL_SendByte(CMD_SYSTEM_STATUS, g_SystemStatus);
                                delay(100); 
                                GPL_SendWord(CMD_WATER_TEMP, (word)(g_WaterTemp * 10));
                                delay(100); 

                                GPL_SendWord(CMD_ENV_TEMP_1, (word)(g_CurrentTemp1 * 10));
                                delay(100); 
                                GPL_SendWord(CMD_TARGET_TEMP_1, (word)(g_TargetTemp1 * 10));
                                delay(100); 
                                GPL_SendByte(CMD_STATUS_HEATING_1, st_Heating_Zone1);
                                delay(100); 

                                GPL_SendWord(CMD_ENV_TEMP_2, (word)(g_CurrentTemp2 * 10));
                                delay(100); 
                                GPL_SendWord(CMD_TARGET_TEMP_2, (word)(g_TargetTemp2 * 10));
                                delay(100); 
                                GPL_SendByte(CMD_STATUS_HEATING_2, st_Heating_Zone2);
                                delay(100); 

                                GPL_SendWord(CMD_ENV_TEMP_3, (word)(g_CurrentTemp3 * 10));
                                delay(100); 
                                GPL_SendWord(CMD_TARGET_TEMP_3, (word)(g_TargetTemp3 * 10));
                                delay(100); 
                                GPL_SendByte(CMD_STATUS_HEATING_3, st_Heating_Zone3);
                                delay(100); 

                                GPL_SendWord(CMD_ENV_TEMP_4, (word)(g_CurrentTemp4 * 10));
                                delay(100); 
                                GPL_SendWord(CMD_TARGET_TEMP_4, (word)(g_TargetTemp4 * 10));
                                delay(100); 
                                GPL_SendByte(CMD_STATUS_HEATING_4, st_Heating_Zone4);
                                delay(100); 

                                GPL_SendWord(CMD_ENV_TEMP_5, (word)(g_CurrentTemp5 * 10));
                                delay(100); 
                                GPL_SendWord(CMD_TARGET_TEMP_5, (word)(g_TargetTemp5 * 10));
                                delay(100); 
                                GPL_SendByte(CMD_STATUS_HEATING_5, st_Heating_Zone5);
                                delay(100); 

                                GPL_SendWord(CMD_ENV_TEMP_6, (word)(g_CurrentTemp6 * 10));
                                delay(100); 
                                GPL_SendWord(CMD_TARGET_TEMP_6, (word)(g_TargetTemp6 * 10));
                                delay(100); 
                                GPL_SendByte(CMD_STATUS_HEATING_6, st_Heating_Zone6);
                                delay(100); 
                            } break;

                            case CMD_SYSTEM_STATUS:
                            {
                                g_SystemStatus = GPL_GetByte(g_InputBuffer);
                                MqttPublishStatusSwitchSystem();
                            } break;
                            
                            case CMD_TARGET_TEMP_1:
                            {
                                g_TargetTemp1 = (float)GPL_GetWord(g_InputBuffer) / 10;
                                String topic = g_mqtt_Topic_Zone1 + "/state_temp";
                                g_mqttPubSub.publish(topic.c_str(), String(g_TargetTemp1).c_str()); 
                                MqttPublishStateZone1();
                            } break;

                            case CMD_TARGET_TEMP_2:
                            {
                                g_TargetTemp2 = (float)GPL_GetWord(g_InputBuffer) / 10;
                                String topic = g_mqtt_Topic_Zone2 + "/state_temp";
                                g_mqttPubSub.publish(topic.c_str(), String(g_TargetTemp2).c_str()); 
                                MqttPublishStateZone2();
                            } break;

                            case CMD_TARGET_TEMP_3:
                            {
                                g_TargetTemp3 = (float)GPL_GetWord(g_InputBuffer) / 10;
                                String topic = g_mqtt_Topic_Zone3 + "/state_temp";
                                g_mqttPubSub.publish(topic.c_str(), String(g_TargetTemp3).c_str()); 
                                MqttPublishStateZone3();
                            } break;

                            case CMD_TARGET_TEMP_4:
                            {
                                g_TargetTemp4 = (float)GPL_GetWord(g_InputBuffer) / 10;
                                String topic = g_mqtt_Topic_Zone4 + "/state_temp";
                                g_mqttPubSub.publish(topic.c_str(), String(g_TargetTemp4).c_str()); 
                                MqttPublishStateZone4();
                            } break;

                            case CMD_TARGET_TEMP_5:
                            {
                                g_TargetTemp5 = (float)GPL_GetWord(g_InputBuffer) / 10;
                                String topic = g_mqtt_Topic_Zone5 + "/state_temp";
                                g_mqttPubSub.publish(topic.c_str(), String(g_TargetTemp5).c_str()); 
                                MqttPublishStateZone5();
                            } break;

                            case CMD_TARGET_TEMP_6:
                            {
                                g_TargetTemp6 = (float)GPL_GetWord(g_InputBuffer) / 10;
                                String topic = g_mqtt_Topic_Zone6 + "/state_temp";
                                g_mqttPubSub.publish(topic.c_str(), String(g_TargetTemp6).c_str()); 
                                MqttPublishStateZone6();
                            } break;
                        }
                    }
                    else
                    {
                        gplStatus = GPL_ST_IDLE;
                    }
                } break;
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Areas Temperature Management (System is active only if switch is on and water temperature is over threshold (28.0°C))
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if((g_SystemStatus == SYSTEM_ON) && (g_WaterTemp > WATER_TEMP_THRESHOLD))
    {
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // AREA 1
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        switch(st_Heating_Zone1)
        {
            case HEATING_ST_OFF:
            {
                if(g_CurrentTemp1 < (g_TargetTemp1 - DELTA_HISTERESYS_TEMP))
                {
                    g_RelayStatus1 = RELAY_ON;
                    digitalWrite(OUT_RELAY1, g_RelayStatus1);
                    g_RelayStatus2 = RELAY_ON;
                    digitalWrite(OUT_RELAY2, g_RelayStatus2);
                    
                    st_Heating_Zone1 = HEATING_ST_ON;
                    MqttPublishStateZone1();
                    GPL_SendByte(CMD_STATUS_HEATING_1, st_Heating_Zone1);
                    delay(50);
                }
            } break;

            case HEATING_ST_ON:
            {
                if(g_CurrentTemp1 > (g_TargetTemp1 + DELTA_HISTERESYS_TEMP))
                {
                    g_RelayStatus1 = RELAY_OFF;
                    digitalWrite(OUT_RELAY1, g_RelayStatus1);
                    g_RelayStatus2 = RELAY_OFF;
                    digitalWrite(OUT_RELAY2, g_RelayStatus2);
                    
                    st_Heating_Zone1 = HEATING_ST_OFF;
                    MqttPublishStateZone1();
                    GPL_SendByte(CMD_STATUS_HEATING_1, st_Heating_Zone1);
                    delay(50);
                }
            } break;
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // AREA 2
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        switch(st_Heating_Zone2)
        {
            case HEATING_ST_OFF:
            {
                if(g_CurrentTemp2 < (g_TargetTemp2 - DELTA_HISTERESYS_TEMP))
                {
                    g_RelayStatus3 = RELAY_ON;
                    digitalWrite(OUT_RELAY3, g_RelayStatus3);
                    g_RelayStatus4 = RELAY_ON;
                    digitalWrite(OUT_RELAY4, g_RelayStatus4);
                    
                    st_Heating_Zone2 = HEATING_ST_ON;
                    MqttPublishStateZone2();
                    GPL_SendByte(CMD_STATUS_HEATING_2, st_Heating_Zone2);
                    delay(50);
                }
            } break;

            case HEATING_ST_ON:
            {
                if(g_CurrentTemp2 > (g_TargetTemp2 + DELTA_HISTERESYS_TEMP))
                {
                    g_RelayStatus3 = RELAY_OFF;
                    digitalWrite(OUT_RELAY3, g_RelayStatus3);
                    g_RelayStatus4 = RELAY_OFF;
                    digitalWrite(OUT_RELAY4, g_RelayStatus4);
                    
                    st_Heating_Zone2 = HEATING_ST_OFF;
                    MqttPublishStateZone2();
                    GPL_SendByte(CMD_STATUS_HEATING_2, st_Heating_Zone2);
                    delay(50);
                }
            } break;
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // AREA 3
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        switch(st_Heating_Zone3)
        {
            case HEATING_ST_OFF:
            {
                if(g_CurrentTemp3 < (g_TargetTemp3 - DELTA_HISTERESYS_TEMP))
                {
                    g_RelayStatus5 = RELAY_ON;
                    digitalWrite(OUT_RELAY5, g_RelayStatus5);
                    
                    st_Heating_Zone3 = HEATING_ST_ON;
                    MqttPublishStateZone3();
                    GPL_SendByte(CMD_STATUS_HEATING_3, st_Heating_Zone3);
                    delay(50);
                }
            } break;

            case HEATING_ST_ON:
            {
                if(g_CurrentTemp3 > (g_TargetTemp3 + DELTA_HISTERESYS_TEMP))
                {
                    g_RelayStatus5 = RELAY_OFF;
                    digitalWrite(OUT_RELAY5, g_RelayStatus5);
                    
                    st_Heating_Zone3 = HEATING_ST_OFF;
                    MqttPublishStateZone3();
                    GPL_SendByte(CMD_STATUS_HEATING_3, st_Heating_Zone3);
                    delay(50);
                }
            } break;
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // AREA 4
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        switch(st_Heating_Zone4)
        {
            case HEATING_ST_OFF:
            {
                if(g_CurrentTemp4 < (g_TargetTemp4 - DELTA_HISTERESYS_TEMP))
                {
                    g_RelayStatus6 = RELAY_ON;
                    digitalWrite(OUT_RELAY6, g_RelayStatus6);
                    g_RelayStatus7 = RELAY_ON;
                    digitalWrite(OUT_RELAY7, g_RelayStatus7);
                    
                    st_Heating_Zone4 = HEATING_ST_ON;
                    MqttPublishStateZone4();
                    GPL_SendByte(CMD_STATUS_HEATING_4, st_Heating_Zone4);
                    delay(50);
                }
            } break;

            case HEATING_ST_ON:
            {
                if(g_CurrentTemp4 > (g_TargetTemp4 + DELTA_HISTERESYS_TEMP))
                {
                    g_RelayStatus6 = RELAY_OFF;
                    digitalWrite(OUT_RELAY6, g_RelayStatus6);
                    g_RelayStatus7 = RELAY_OFF;
                    digitalWrite(OUT_RELAY7, g_RelayStatus7);
                    
                    st_Heating_Zone4 = HEATING_ST_OFF;
                    MqttPublishStateZone4();
                    GPL_SendByte(CMD_STATUS_HEATING_4, st_Heating_Zone4);
                    delay(50);
                }
            } break;
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // AREA 5
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        switch(st_Heating_Zone5)
        {
            case HEATING_ST_OFF:
            {
                if(g_CurrentTemp5 < (g_TargetTemp5 - DELTA_HISTERESYS_TEMP))
                {
                    g_RelayStatus8 = RELAY_ON;
                    digitalWrite(OUT_RELAY8, g_RelayStatus8);
                    g_RelayStatus9 = RELAY_ON;
                    digitalWrite(OUT_RELAY9, g_RelayStatus9);
                    
                    st_Heating_Zone5 = HEATING_ST_ON;
                    MqttPublishStateZone5();
                    GPL_SendByte(CMD_STATUS_HEATING_5, st_Heating_Zone5);
                    delay(50);
                }
            } break;

            case HEATING_ST_ON:
            {
                if(g_CurrentTemp5 > (g_TargetTemp5 + DELTA_HISTERESYS_TEMP))
                {
                    g_RelayStatus8 = RELAY_OFF;
                    digitalWrite(OUT_RELAY8, g_RelayStatus8);
                    g_RelayStatus9 = RELAY_OFF;
                    digitalWrite(OUT_RELAY9, g_RelayStatus9);
                    
                    st_Heating_Zone5 = HEATING_ST_OFF;
                    MqttPublishStateZone5();
                    GPL_SendByte(CMD_STATUS_HEATING_5, st_Heating_Zone5);
                    delay(50);
                }
            } break;
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // AREA 6
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        switch(st_Heating_Zone6)
        {
            case HEATING_ST_OFF:
            {
                if(g_CurrentTemp6 < (g_TargetTemp6 - DELTA_HISTERESYS_TEMP))
                {
                    g_RelayStatus10 = RELAY_ON;
                    digitalWrite(OUT_RELAY10, g_RelayStatus10);
                    g_RelayStatus11 = RELAY_ON;
                    digitalWrite(OUT_RELAY11, g_RelayStatus11);
                    
                    st_Heating_Zone6 = HEATING_ST_ON;
                    MqttPublishStateZone6();
                    GPL_SendByte(CMD_STATUS_HEATING_6, st_Heating_Zone6);
                    delay(50);
                }
            } break;

            case HEATING_ST_ON:
            {
                if(g_CurrentTemp6 > (g_TargetTemp6 + DELTA_HISTERESYS_TEMP))
                {
                    g_RelayStatus10 = RELAY_OFF;
                    digitalWrite(OUT_RELAY10, g_RelayStatus10);
                    g_RelayStatus11 = RELAY_OFF;
                    digitalWrite(OUT_RELAY11, g_RelayStatus11);
                    
                    st_Heating_Zone6 = HEATING_ST_OFF;
                    MqttPublishStateZone6();
                    GPL_SendByte(CMD_STATUS_HEATING_6, st_Heating_Zone6);
                    delay(50);
                }
            } break;
        }
        
    } else
    {
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // AREA 1
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if(st_Heating_Zone1 == HEATING_ST_ON)
        {
            st_Heating_Zone1 = HEATING_ST_OFF;
            g_RelayStatus1 = RELAY_OFF;
            digitalWrite(OUT_RELAY1, g_RelayStatus1);
            g_RelayStatus2 = RELAY_OFF;
            digitalWrite(OUT_RELAY2, g_RelayStatus2);
            MqttPublishStateZone1();
            GPL_SendByte(CMD_STATUS_HEATING_1, st_Heating_Zone1);
            delay(50); 
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // AREA 2
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if(st_Heating_Zone2 == HEATING_ST_ON)
        {
            st_Heating_Zone2 = HEATING_ST_OFF;
            g_RelayStatus3 = RELAY_OFF;
            digitalWrite(OUT_RELAY3, g_RelayStatus3);
            g_RelayStatus4 = RELAY_OFF;
            digitalWrite(OUT_RELAY4, g_RelayStatus4);
            MqttPublishStateZone2();
            GPL_SendByte(CMD_STATUS_HEATING_2, st_Heating_Zone2);
            delay(50); 
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // AREA 3
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if(st_Heating_Zone3 == HEATING_ST_ON)
        {
            st_Heating_Zone3 = HEATING_ST_OFF;
            g_RelayStatus5 = RELAY_OFF;
            digitalWrite(OUT_RELAY5, g_RelayStatus5);
            MqttPublishStateZone3();
            GPL_SendByte(CMD_STATUS_HEATING_3, st_Heating_Zone3);
            delay(50); 
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // AREA 4
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if(st_Heating_Zone4 == HEATING_ST_ON)
        {
            st_Heating_Zone4 = HEATING_ST_OFF;
            g_RelayStatus6 = RELAY_OFF;
            digitalWrite(OUT_RELAY6, g_RelayStatus6);
            g_RelayStatus7 = RELAY_OFF;
            digitalWrite(OUT_RELAY7, g_RelayStatus7);
            MqttPublishStateZone4();
            GPL_SendByte(CMD_STATUS_HEATING_4, st_Heating_Zone4);
            delay(50); 
        }
                
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // AREA 5
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if(st_Heating_Zone5 == HEATING_ST_ON)
        {
            st_Heating_Zone5 = HEATING_ST_OFF;
            g_RelayStatus8 = RELAY_OFF;
            digitalWrite(OUT_RELAY8, g_RelayStatus8);
            g_RelayStatus9 = RELAY_OFF;
            digitalWrite(OUT_RELAY9, g_RelayStatus9);
            MqttPublishStateZone5();
            GPL_SendByte(CMD_STATUS_HEATING_5, st_Heating_Zone5);
            delay(50); 
        }
        
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // AREA 6
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if(st_Heating_Zone6 == HEATING_ST_ON)
        {
            st_Heating_Zone6 = HEATING_ST_OFF;
            g_RelayStatus10 = RELAY_OFF;
            digitalWrite(OUT_RELAY10, g_RelayStatus10);
            g_RelayStatus11 = RELAY_OFF;
            digitalWrite(OUT_RELAY11, g_RelayStatus11);
            MqttPublishStateZone6();
            GPL_SendByte(CMD_STATUS_HEATING_6, st_Heating_Zone6);
            delay(50); 
        }
        
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Inputs Monitor for button that power on Raspberry and User Interface
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if(millis() - g_TimeInputs > 200)
    {
        g_TimeInputs = millis();

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Inputs 1
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        switch(g_st_input1)
        {
            case UNPRESSED:
            {
                if(digitalRead(IN_DIN1) == 0)            
                {
                    g_st_input1 = DEBOUNCE;
                }
            } break;
    
            case DEBOUNCE:
            {
                if(digitalRead(IN_DIN1) == 0)           
                {
                    g_st_input1 = PRESSED;
                    g_Input1 = 1;   
                    if(g_SerialDeviceIsOn == false)
                    {
                        g_SerialDeviceIsOn = true;
                        digitalWrite(OUT_RELAY12, RELAY_ON);                  
                    } else
                    {
                        g_SerialDeviceIsOn = false;
                        g_SerialDeviceConnected = false;
                        digitalWrite(OUT_RELAY12, RELAY_OFF);
                    }
                } else                                  
                {
                    g_st_input1 = UNPRESSED;
                    g_Input1 = 0;                       
                }
            } break;
    
            case PRESSED:
            {
                if(digitalRead(IN_DIN1) == 1)
                {
                    g_st_input1 = DEBOUNCE;
                }
            } break;
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Status Led
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if(millis() - g_TimeLed > 500)
    {
        g_TimeLed = millis();
        digitalWrite(OUT_STATUS_LED, !digitalRead(OUT_STATUS_LED));
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Read Water Temperature every 5 sec
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if(millis() - g_TimeTemperature > 5000)
    {
        g_TimeTemperature = millis();
        g_WaterTemp = GetTemperature();
        MqttPublishWaterTemperature();
        GPL_SendWord(CMD_WATER_TEMP, (word)(g_WaterTemp * 10));
    }
 
}


/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------ Public Functions -----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
void setup_wifi() 
{
    int counter = 0;
    byte mac[6];
    delay(10);


    WiFi.begin(g_ssid, g_password);

    WiFi.macAddress(mac);
    g_UniqueId =  String(mac[0],HEX) + String(mac[1],HEX) + String(mac[2],HEX) + String(mac[3],HEX) + String(mac[4],HEX) + String(mac[5],HEX); 
    
    while(WiFi.status() != WL_CONNECTED && counter++ < 5) 
    {
        delay(500);
    }

}

void MqttReconnect() 
{
    // Loop until we're MqttReconnected
    while (!g_mqttPubSub.connected()  && (g_mqttCounterConn++ < 4))
    {
        if (g_mqttPubSub.connect(g_mqtt_DeviceName, g_mqttUser, g_mqttPsw)) 
        {
            // Home assistant status
            g_mqttPubSub.subscribe("homeassistant/status");

            // Zone 1
            g_mqttPubSub.subscribe(g_mqtt_Topic_SensorZone1.c_str());				// Subscribe topic for receive Area Temperature
            g_mqttPubSub.subscribe((g_mqtt_Topic_Zone1  + "/set_temp").c_str());	// Subscribe topic for setting Target Temperature

            // Zone 2
            g_mqttPubSub.subscribe(g_mqtt_Topic_SensorZone2.c_str());
            g_mqttPubSub.subscribe((g_mqtt_Topic_Zone2  + "/set_temp").c_str());

            // Zone 3
            g_mqttPubSub.subscribe(g_mqtt_Topic_SensorZone3.c_str());
            g_mqttPubSub.subscribe((g_mqtt_Topic_Zone3  + "/set_temp").c_str());

            // Zone 4
            g_mqttPubSub.subscribe(g_mqtt_Topic_SensorZone4.c_str());
            g_mqttPubSub.subscribe((g_mqtt_Topic_Zone4  + "/set_temp").c_str());

            // Zone 5
            g_mqttPubSub.subscribe(g_mqtt_Topic_SensorZone5.c_str());
            g_mqttPubSub.subscribe((g_mqtt_Topic_Zone5  + "/set_temp").c_str());

            // Zone 6
            g_mqttPubSub.subscribe(g_mqtt_Topic_SensorZone6.c_str());
            g_mqttPubSub.subscribe((g_mqtt_Topic_Zone6  + "/set_temp").c_str());

            // Switch Status
            g_mqttPubSub.subscribe((g_mqtt_Topic_SwitchStatus + "/set").c_str());
            
            delay(500);
        } else 
        {
            delay(3000);
        }
    }  
    g_mqttCounterConn = 0;
}

void MqttHomeAssistantDiscovery()
{
    String discoveryTopic;
    String payload;
    String strPayload;
    int uniqueId_increment = 0;
    if(g_mqttPubSub.connected())
    {
        StaticJsonDocument<600> payload;
        JsonArray modes;
        JsonObject device;
        JsonArray identifiers;

        
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Area 1 
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        discoveryTopic = "homeassistant/climate/" + g_ClimateZone1 + "/config";

        uniqueId_increment++;
        payload["name"] = "clima." + g_ClimateZone1;                        // Nome dell'entità           
        payload["uniq_id"] = g_UniqueId + "_" + String(uniqueId_increment); // Id univoco dell'entità
        //payload["mode_cmd_t"] = g_mqtt_Topic_Zone1 + "/set_mode";           // Topic per impostare la modalità
        payload["mode_stat_t"] = g_mqtt_Topic_Zone1 + "/state_mode";        // Topic per la modalità corrente
        payload["temp_cmd_t"] = g_mqtt_Topic_Zone1 + "/set_temp";           // Topic per impostare la temperatura target
        payload["temp_stat_t"] = g_mqtt_Topic_Zone1 + "/state_temp";        // Topic per la temperatura target corrente
        payload["curr_temp_t"] = g_mqtt_Topic_SensorZone1;                  // Topic per la temperatura corrente (deve essere quello del sensore)
        payload["curr_temp_tpl"] = "{{ value_json.temp }}"; 
        payload["min_temp"] = "13";
        payload["max_temp"] = "28";
        payload["temp_step"] = "0.1";
        modes = payload.createNestedArray("modes");
        modes.add("off");
        modes.add("heat");
        device = payload.createNestedObject("device");
        device["name"] = "ClimateRelays";
        device["model"] = "ESP32Relays";
        device["manufacturer"] = "Vigasan";
        identifiers = device.createNestedArray("identifiers");
        identifiers.add(g_UniqueId);
        
        serializeJson(payload, strPayload);

        g_mqttPubSub.publish(discoveryTopic.c_str(), strPayload.c_str());
        delay(100);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Area 2
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        discoveryTopic = "homeassistant/climate/" + g_ClimateZone2 + "/config";
        payload.clear();
        modes.clear();
        device.clear();
        identifiers.clear();
        strPayload.clear();
        
        uniqueId_increment++;
        payload["name"] = "clima." + g_ClimateZone2;                        // Nome dell'entità           
        payload["uniq_id"] = g_UniqueId + "_" + String(uniqueId_increment); // Id univoco dell'entità
        //payload["mode_cmd_t"] = g_mqtt_Topic_Zone2 + "/set_mode";           // Topic per impostare la modalità
        payload["mode_stat_t"] = g_mqtt_Topic_Zone2 + "/state_mode";        // Topic per la modalità corrente
        payload["temp_cmd_t"] = g_mqtt_Topic_Zone2 + "/set_temp";           // Topic per impostare la temperatura target
        payload["temp_stat_t"] = g_mqtt_Topic_Zone2 + "/state_temp";        // Topic per la temperatura target corrente
        payload["curr_temp_t"] = g_mqtt_Topic_SensorZone2;                  // Topic per la temperatura corrente (deve essere quello del sensore)
        payload["curr_temp_tpl"] = "{{ value_json.temp }}"; 
        payload["min_temp"] = "13";
        payload["max_temp"] = "28";
        payload["temp_step"] = "0.1";
        modes = payload.createNestedArray("modes");
        modes.add("off");
        modes.add("heat");
        device = payload.createNestedObject("device");
        device["name"] = "ClimateRelays";
        device["model"] = "ESP32Relays";
        device["manufacturer"] = "Vigasan";
        identifiers = device.createNestedArray("identifiers");
        identifiers.add(g_UniqueId);
        
        serializeJson(payload, strPayload);
        g_mqttPubSub.publish(discoveryTopic.c_str(), strPayload.c_str());
        delay(100);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Area 3
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        discoveryTopic = "homeassistant/climate/" + g_ClimateZone3 + "/config";
        payload.clear();
        modes.clear();
        device.clear();
        identifiers.clear();
        strPayload.clear();
        
        uniqueId_increment++;
        payload["name"] = "clima." + g_ClimateZone3;                        // Nome dell'entità           
        payload["uniq_id"] = g_UniqueId + "_" + String(uniqueId_increment); // Id univoco dell'entità
        //payload["mode_cmd_t"] = g_mqtt_Topic_Zone3 + "/set_mode";           // Topic per impostare la modalità
        payload["mode_stat_t"] = g_mqtt_Topic_Zone3 + "/state_mode";        // Topic per la modalità corrente
        payload["temp_cmd_t"] = g_mqtt_Topic_Zone3 + "/set_temp";           // Topic per impostare la temperatura target
        payload["temp_stat_t"] = g_mqtt_Topic_Zone3 + "/state_temp";        // Topic per la temperatura target corrente
        payload["curr_temp_t"] = g_mqtt_Topic_SensorZone3;                  // Topic per la temperatura corrente (deve essere quello del sensore)
        payload["curr_temp_tpl"] = "{{ value_json.temp }}"; 
        payload["min_temp"] = "13";
        payload["max_temp"] = "28";
        payload["temp_step"] = "0.1";
        modes = payload.createNestedArray("modes");
        modes.add("off");
        modes.add("heat");
        device = payload.createNestedObject("device");
        device["name"] = "ClimateRelays";
        device["model"] = "ESP32Relays";
        device["manufacturer"] = "Vigasan";
        identifiers = device.createNestedArray("identifiers");
        identifiers.add(g_UniqueId);
        
        serializeJson(payload, strPayload);
        g_mqttPubSub.publish(discoveryTopic.c_str(), strPayload.c_str());
        delay(100);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Area 4
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        discoveryTopic = "homeassistant/climate/" + g_ClimateZone4 + "/config";
        payload.clear();
        modes.clear();
        device.clear();
        identifiers.clear();
        strPayload.clear();
        
        uniqueId_increment++;
        payload["name"] = "clima." + g_ClimateZone4;                        // Nome dell'entità           
        payload["uniq_id"] = g_UniqueId + "_" + String(uniqueId_increment); // Id univoco dell'entità
        //payload["mode_cmd_t"] = g_mqtt_Topic_Zone4 + "/set_mode";           // Topic per impostare la modalità
        payload["mode_stat_t"] = g_mqtt_Topic_Zone4 + "/state_mode";        // Topic per la modalità corrente
        payload["temp_cmd_t"] = g_mqtt_Topic_Zone4 + "/set_temp";           // Topic per impostare la temperatura target
        payload["temp_stat_t"] = g_mqtt_Topic_Zone4 + "/state_temp";        // Topic per la temperatura target corrente
        payload["curr_temp_t"] = g_mqtt_Topic_SensorZone4;                  // Topic per la temperatura corrente (deve essere quello del sensore)
        payload["curr_temp_tpl"] = "{{ value_json.temp }}"; 
        payload["min_temp"] = "13";
        payload["max_temp"] = "28";
        payload["temp_step"] = "0.1";
        modes = payload.createNestedArray("modes");
        modes.add("off");
        modes.add("heat");
        device = payload.createNestedObject("device");
        device["name"] = "ClimateRelays";
        device["model"] = "ESP32Relays";
        device["manufacturer"] = "Vigasan";
        identifiers = device.createNestedArray("identifiers");
        identifiers.add(g_UniqueId);
        
        serializeJson(payload, strPayload);
        g_mqttPubSub.publish(discoveryTopic.c_str(), strPayload.c_str());
        delay(100);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Area 5
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        discoveryTopic = "homeassistant/climate/" + g_ClimateZone5 + "/config";
        payload.clear();
        modes.clear();
        device.clear();
        identifiers.clear();
        strPayload.clear();
        
        uniqueId_increment++;
        payload["name"] = "clima." + g_ClimateZone5;                        // Nome dell'entità           
        payload["uniq_id"] = g_UniqueId + "_" + String(uniqueId_increment); // Id univoco dell'entità
        //payload["mode_cmd_t"] = g_mqtt_Topic_Zone5 + "/set_mode";           // Topic per impostare la modalità
        payload["mode_stat_t"] = g_mqtt_Topic_Zone5 + "/state_mode";        // Topic per la modalità corrente
        payload["temp_cmd_t"] = g_mqtt_Topic_Zone5 + "/set_temp";           // Topic per impostare la temperatura target
        payload["temp_stat_t"] = g_mqtt_Topic_Zone5 + "/state_temp";        // Topic per la temperatura target corrente
        payload["curr_temp_t"] = g_mqtt_Topic_SensorZone5;                  // Topic per la temperatura corrente (deve essere quello del sensore)
        payload["curr_temp_tpl"] = "{{ value_json.temp }}"; 
        payload["min_temp"] = "13";
        payload["max_temp"] = "28";
        payload["temp_step"] = "0.1";
        modes = payload.createNestedArray("modes");
        modes.add("off");
        modes.add("heat");
        device = payload.createNestedObject("device");
        device["name"] = "ClimateRelays";
        device["model"] = "ESP32Relays";
        device["manufacturer"] = "Vigasan";
        identifiers = device.createNestedArray("identifiers");
        identifiers.add(g_UniqueId);
        
        serializeJson(payload, strPayload);
        g_mqttPubSub.publish(discoveryTopic.c_str(), strPayload.c_str());
        delay(100);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Area 6
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        discoveryTopic = "homeassistant/climate/" + g_ClimateZone6 + "/config";
        payload.clear();
        modes.clear();
        device.clear();
        identifiers.clear();
        strPayload.clear();
        
        uniqueId_increment++;
        payload["name"] = "clima." + g_ClimateZone6;                        // Nome dell'entità           
        payload["uniq_id"] = g_UniqueId + "_" + String(uniqueId_increment); // Id univoco dell'entità
        //payload["mode_cmd_t"] = g_mqtt_Topic_Zone6 + "/set_mode";           // Topic per impostare la modalità
        payload["mode_stat_t"] = g_mqtt_Topic_Zone6 + "/state_mode";        // Topic per la modalità corrente
        payload["temp_cmd_t"] = g_mqtt_Topic_Zone6 + "/set_temp";           // Topic per impostare la temperatura target
        payload["temp_stat_t"] = g_mqtt_Topic_Zone6 + "/state_temp";        // Topic per la temperatura target corrente
        payload["curr_temp_t"] = g_mqtt_Topic_SensorZone6;                  // Topic per la temperatura corrente (deve essere quello del sensore)
        payload["curr_temp_tpl"] = "{{ value_json.temp }}"; 
        payload["min_temp"] = "13";
        payload["max_temp"] = "28";
        payload["temp_step"] = "0.1";
        modes = payload.createNestedArray("modes");
        modes.add("off");
        modes.add("heat");
        device = payload.createNestedObject("device");
        device["name"] = "ClimateRelays";
        device["model"] = "ESP32Relays";
        device["manufacturer"] = "Vigasan";
        identifiers = device.createNestedArray("identifiers");
        identifiers.add(g_UniqueId);
        
        serializeJson(payload, strPayload);
        g_mqttPubSub.publish(discoveryTopic.c_str(), strPayload.c_str());
        delay(100);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Water Temperature
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        discoveryTopic = "homeassistant/sensor/water_temp/config";
        payload.clear();
        modes.clear();
        device.clear();
        identifiers.clear();
        strPayload.clear();
        
        uniqueId_increment++;
        payload["name"] = "clima." + g_mqttWaterTemp;                         // Nome dell'entità           
        payload["uniq_id"] = g_UniqueId + "_" + String(uniqueId_increment);      // Id univoco dell'entità
        payload["stat_t"] = g_mqtt_Topic_WaterTemperature;
        payload["dev_cla"] = "temperature";
        payload["unit_of_meas"] = "°C";
        device = payload.createNestedObject("device");
        device["name"] = "ClimateRelays";
        device["model"] = "ESP32Relays";
        device["manufacturer"] = "Vigasan";
        identifiers = device.createNestedArray("identifiers");
        identifiers.add(g_UniqueId);
        
        serializeJson(payload, strPayload);
        g_mqttPubSub.publish(discoveryTopic.c_str(), strPayload.c_str());
        delay(100);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Switch Status
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        discoveryTopic = "homeassistant/switch/switchstatus/config";
        payload.clear();
        device.clear();
        identifiers.clear();
        strPayload.clear();

        uniqueId_increment++;
        payload["name"] = "clima." + g_mqttSwitchStatus;                 
        payload["uniq_id"] = g_UniqueId + "_" + String(uniqueId_increment);                                                     
        payload["stat_t"] = g_mqtt_Topic_SwitchStatus + "/state";    
        payload["cmd_t"] = g_mqtt_Topic_SwitchStatus + "/set"; 
        device = payload.createNestedObject("device");
        device["name"] = "ClimateRelays";
        device["model"] = "ESP32Relays";
        device["manufacturer"] = "Vigasan";
        identifiers = device.createNestedArray("identifiers");
        identifiers.add(g_UniqueId);
        
        serializeJson(payload, strPayload);

        g_mqttPubSub.publish(discoveryTopic.c_str(), strPayload.c_str());
        delay(100);

    }
}

void MqttReceiverCallback(char* topic, byte* inFrame, unsigned int length) 
{
    byte state = 0;
    String payload;
    String topicMsg;
    StaticJsonDocument<256> doc;
    
    for (int i = 0; i < length; i++) 
    {
        payload += (char)inFrame[i];
    }


    if(String(topic) == String("homeassistant/status")) 
    {
        if(payload == "online")
        {
            MqttHomeAssistantDiscovery();
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Area 1
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if(String(topic) == g_mqtt_Topic_SensorZone1) 
    {
        deserializeJson(doc, inFrame);
        g_CurrentTemp1 = doc["temp"];
        GPL_SendWord(CMD_ENV_TEMP_1, (word)(g_CurrentTemp1 * 10));
    }

    if(String(topic) == (g_mqtt_Topic_Zone1  + "/set_temp")) 
    {
        g_TargetTemp1 = payload.toFloat();
        topicMsg = g_mqtt_Topic_Zone1 + "/state_temp";
        g_mqttPubSub.publish(topicMsg.c_str(), String(g_TargetTemp1).c_str());
        GPL_SendWord(CMD_TARGET_TEMP_1, (word)(g_TargetTemp1 * 10)); 
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Area 2
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if(String(topic) == g_mqtt_Topic_SensorZone2) 
    {
        deserializeJson(doc,inFrame);
        g_CurrentTemp2 = doc["temp"];
        GPL_SendWord(CMD_ENV_TEMP_2, (word)(g_CurrentTemp2 * 10));
    }

    if(String(topic) == (g_mqtt_Topic_Zone2  + "/set_temp")) 
    {
        g_TargetTemp2 = payload.toFloat();
        topicMsg = g_mqtt_Topic_Zone2 + "/state_temp";
        g_mqttPubSub.publish(topicMsg.c_str(), String(g_TargetTemp2).c_str()); 
        GPL_SendWord(CMD_TARGET_TEMP_2, (word)(g_TargetTemp2 * 10));
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Area 3
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if(String(topic) == g_mqtt_Topic_SensorZone3) 
    {
        deserializeJson(doc,inFrame);
        g_CurrentTemp3 = doc["temp"];
        GPL_SendWord(CMD_ENV_TEMP_3, (word)(g_CurrentTemp3 * 10));
    }

    if(String(topic) == (g_mqtt_Topic_Zone3  + "/set_temp")) 
    {
        g_TargetTemp3 = payload.toFloat();
        topicMsg = g_mqtt_Topic_Zone3 + "/state_temp";
        g_mqttPubSub.publish(topicMsg.c_str(), String(g_TargetTemp3).c_str()); 
        GPL_SendWord(CMD_TARGET_TEMP_3, (word)(g_TargetTemp3 * 10));
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Area 4
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if(String(topic) == g_mqtt_Topic_SensorZone4) 
    {
        deserializeJson(doc,inFrame);
        g_CurrentTemp4 = doc["temp"];
        GPL_SendWord(CMD_ENV_TEMP_4, (word)(g_CurrentTemp4 * 10));
    }

    if(String(topic) == (g_mqtt_Topic_Zone4  + "/set_temp")) 
    {
        g_TargetTemp4 = payload.toFloat();
        topicMsg = g_mqtt_Topic_Zone4 + "/state_temp";
        g_mqttPubSub.publish(topicMsg.c_str(), String(g_TargetTemp4).c_str()); 
        GPL_SendWord(CMD_TARGET_TEMP_4, (word)(g_TargetTemp4 * 10));
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Area 5
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if(String(topic) == g_mqtt_Topic_SensorZone5) 
    {
        deserializeJson(doc,inFrame);
        g_CurrentTemp5 = doc["temp"];
        GPL_SendWord(CMD_ENV_TEMP_5, (word)(g_CurrentTemp5 * 10));
    }

    if(String(topic) == (g_mqtt_Topic_Zone5  + "/set_temp")) 
    {
        g_TargetTemp5 = payload.toFloat();
        topicMsg = g_mqtt_Topic_Zone5 + "/state_temp";
        g_mqttPubSub.publish(topicMsg.c_str(), String(g_TargetTemp5).c_str()); 
        GPL_SendWord(CMD_TARGET_TEMP_5, (word)(g_TargetTemp5 * 10));
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Area 6
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if(String(topic) == g_mqtt_Topic_SensorZone6) 
    {
        deserializeJson(doc,inFrame);
        g_CurrentTemp6 = doc["temp"];
        GPL_SendWord(CMD_ENV_TEMP_6, (word)(g_CurrentTemp6 * 10));
    }

    if(String(topic) == (g_mqtt_Topic_Zone6  + "/set_temp")) 
    {
        g_TargetTemp6 = payload.toFloat();
        topicMsg = g_mqtt_Topic_Zone6 + "/state_temp";
        g_mqttPubSub.publish(topicMsg.c_str(), String(g_TargetTemp6).c_str()); 
        GPL_SendWord(CMD_TARGET_TEMP_6, (word)(g_TargetTemp6 * 10));
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Switch Status
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if(String(topic) == String(g_mqtt_Topic_SwitchStatus + "/set")) 
    {
        if(payload == "ON") 
        {
            g_SystemStatus = SYSTEM_ON;

        } else if(payload == "OFF") 
        {
            g_SystemStatus = SYSTEM_OFF;
        }
        MqttPublishStatusSwitchSystem();
        GPL_SendByte(CMD_SYSTEM_STATUS, g_SystemStatus);
    }
}

void MqttPublishStateZone1()
{
    String topicMsg;
    String payload;
    if(g_mqttPubSub.connected())
    {
        if(st_Heating_Zone1 == HEATING_ST_ON)
            payload = "heat";
        else
            payload = "off";
        
        topicMsg = g_mqtt_Topic_Zone1 + "/state_mode";
        g_mqttPubSub.publish(topicMsg.c_str(), payload.c_str());
    }
}

void MqttPublishStateZone2()
{
    String topicMsg;
    String payload;
    if(g_mqttPubSub.connected())
    {
        if(st_Heating_Zone2 == HEATING_ST_ON)
            payload = "heat";
        else
            payload = "off";
        
        topicMsg = g_mqtt_Topic_Zone2 + "/state_mode";
        g_mqttPubSub.publish(topicMsg.c_str(), payload.c_str());
    }
}

void MqttPublishStateZone3()
{
    String topicMsg;
    String payload;
    if(g_mqttPubSub.connected())
    {
        if(st_Heating_Zone3 == HEATING_ST_ON)
            payload = "heat";
        else
            payload = "off";
        
        topicMsg = g_mqtt_Topic_Zone3 + "/state_mode";
        g_mqttPubSub.publish(topicMsg.c_str(), payload.c_str());
    }
}

void MqttPublishStateZone4()
{
    String topicMsg;
    String payload;
    if(g_mqttPubSub.connected())
    {
        if(st_Heating_Zone4 == HEATING_ST_ON)
            payload = "heat";
        else
            payload = "off";
        
        topicMsg = g_mqtt_Topic_Zone4 + "/state_mode";
        g_mqttPubSub.publish(topicMsg.c_str(), payload.c_str());
    }
}

void MqttPublishStateZone5()
{
    String topicMsg;
    String payload;
    if(g_mqttPubSub.connected())
    {
        if(st_Heating_Zone5 == HEATING_ST_ON)
            payload = "heat";
        else
            payload = "off";
        
        topicMsg = g_mqtt_Topic_Zone5 + "/state_mode";
        g_mqttPubSub.publish(topicMsg.c_str(), payload.c_str());
    }
}

void MqttPublishStateZone6()
{
    String topicMsg;
    String payload;
    if(g_mqttPubSub.connected())
    {
        if(st_Heating_Zone6 == HEATING_ST_ON)
            payload = "heat";
        else
            payload = "off";
        
        topicMsg = g_mqtt_Topic_Zone6 + "/state_mode";
        g_mqttPubSub.publish(topicMsg.c_str(), payload.c_str());
    }
}

void MqttPublishStatusSwitchSystem()
{
    String topicMsg;
    String payload;
    if(g_mqttPubSub.connected())
    {
        if(g_SystemStatus == SYSTEM_OFF)
            payload = "OFF";
        else
            payload = "ON";
        
        topicMsg = g_mqtt_Topic_SwitchStatus + "/state";
        g_mqttPubSub.publish(topicMsg.c_str(), payload.c_str());
    }
}

void MqttPublishWaterTemperature()
{
    String topicMsg;
    String payload;
    if(g_mqttPubSub.connected())
    {
        payload = g_WaterTemp;
        topicMsg = g_mqtt_Topic_WaterTemperature;
        g_mqttPubSub.publish(topicMsg.c_str(), payload.c_str());
    }
}

float GetTemperature()
{
    byte data[12];
    byte addr[8];
  
    if ( !g_tempSensor.search(addr)) {
        //no more sensors on chain, reset search
        g_tempSensor.reset_search();
        return -1000;
    }
  
    if ( OneWire::crc8( addr, 7) != addr[7]) {
        return -1000;
    }
  
    if ( addr[0] != 0x10 && addr[0] != 0x28) {
        return -1000;
    }
  
    g_tempSensor.reset();
    g_tempSensor.select(addr);
    g_tempSensor.write(0x44,1); // start conversion, with parasite power on at the end
  
    byte present = g_tempSensor.reset();
    g_tempSensor.select(addr);
    g_tempSensor.write(0xBE); // Read Scratchpad
  
  
    for (int i = 0; i < 9; i++) { // we need 9 bytes
      data[i] = g_tempSensor.read();
    }
  
    g_tempSensor.reset_search();
  
    byte MSB = data[1];
    byte LSB = data[0];
  
    float tempRead = ((MSB << 8) | LSB); //using two's compliment
    float TemperatureSum = tempRead / 16;
  

    return TemperatureSum;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Serial Communication Protocol Functions
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
byte CalculateChecksum(byte numByte)
{
    byte rv = 0, index;
    for(index = 0; index < numByte; index++)
    {
        rv += g_FrameBuffer[index];
    }
    return rv;
}

void GplSendData(byte* pBuff, int length)
{
    int i;
    byte dataToSend = 0;

    if(g_SerialDeviceConnected)
    {
        g_OutputBuffer[dataToSend++] = GPL_START_FRAME;
    
        for(i = 1; i < length; i++)
        {
            if(pBuff[i] == GPL_START_FRAME || pBuff[i] == GPL_ESCAPE_CHAR)
            {
                g_OutputBuffer[dataToSend++] = GPL_ESCAPE_CHAR;
                g_OutputBuffer[dataToSend++] = pBuff[i] ^ GPL_XOR_CHAR;
            } else
                g_OutputBuffer[dataToSend++] = pBuff[i];
        }
        
        Serial.write(g_OutputBuffer, dataToSend);
    }
}

/*************************************************************************************************************************************************/
//NAME:         GPL_SendByte
//DESCRPTION:   Send a byte
//              (byte) Command
//              (byte) Data
//RETURN:       void
//NOTE:
/*************************************************************************************************************************************************/
void GPL_SendByte(byte cmd, byte data)
{
    g_FrameBuffer[0] = GPL_START_FRAME;                         // Start Frame
    g_FrameBuffer[1] = cmd;                                     // Comando
    g_FrameBuffer[2] = 0x01;                                    // Lunghezza campo dati
    g_FrameBuffer[3] = data;                                    // Data
    g_FrameBuffer[4] = CalculateChecksum(4);                    // Checksum

    GplSendData(g_FrameBuffer, 5);
}

/*************************************************************************************************************************************************/
//NAME:         GPL_SendWord
//DESCRPTION:   Send a 2 byte word
//              (byte) Command
//              (word) Data
//RETURN:       void
//NOTE:
/*************************************************************************************************************************************************/
void GPL_SendWord(byte cmd, word data)
{
    g_FrameBuffer[0] = GPL_START_FRAME;                         // Start Frame
    g_FrameBuffer[1] = cmd;                                     // Comando
    g_FrameBuffer[2] = 0x02;                                    // Lunghezza campo dati
    g_FrameBuffer[3] = HIBYTE(data);                            // Data
    g_FrameBuffer[4] = LOBYTE(data);
    g_FrameBuffer[5] = CalculateChecksum(5);                    // Checksum

    GplSendData(g_FrameBuffer, 6);
}

/*************************************************************************************************************************************************/
//NAME:         GPL_SendDWord
//DESCRPTION:   Send a 4 byte word
//              (byte) Command
//              (unsigned long) Data
//RETURN:       void
//NOTE:
/*************************************************************************************************************************************************/
void GPL_SendDWord(byte cmd, unsigned long data)
{
    g_FrameBuffer[0] = GPL_START_FRAME;                          // Start Frame
    g_FrameBuffer[1] = cmd;                                     // Comando
    g_FrameBuffer[2] = 0x04;                                    // Lunghezza campo dati
    g_FrameBuffer[3] = MSBYTE(data);                            // Data
    g_FrameBuffer[4] = UPBYTE(data);
    g_FrameBuffer[5] = HIBYTE(data);
    g_FrameBuffer[6] = LOBYTE(data);
    g_FrameBuffer[7] = CalculateChecksum(7);                    // Checksum

    GplSendData(g_FrameBuffer, 8);
}

/*************************************************************************************************************************************************/
//NAME:         GPL_SendMessage
//DESCRPTION:   Send a buffer data
//              (byte) Command
//              (byte*) Buffer
//              (int) Buffer length
//RETURN:       void
//NOTE:
/*************************************************************************************************************************************************/
void GPL_SendMessage(byte cmd, byte* pBuff, int length)
{
    int i = 0;
    g_FrameBuffer[0] = GPL_START_FRAME;                         // Start Frame
    g_FrameBuffer[1] = cmd;                                     // Comando
    g_FrameBuffer[2] = length;                                  // Lunghezza campo dati
    for(i = 0; i < length; i++)
        g_FrameBuffer[i + 3] = pBuff[i];
    g_FrameBuffer[length + 3] = CalculateChecksum(length + 3);  // Checksum

    GplSendData(g_FrameBuffer, length + 4);
}

/*************************************************************************************************************************************************/
//NAME:         GPL_SendFrame
//DESCRPTION:   Send a Protocol Frame 
//INPUT:        (byte*) Pointer to Frame buffer
//RETURN:       void
//NOTE:
/*************************************************************************************************************************************************/
void GPL_SendFrame(byte* pBuff)
{
    int i = 0, length;
    length = pBuff[GPL_BYTE_LENGTH] + GPL_NUM_EXTRA_BYTES;
    for(i = 0; i < length; i++)
        g_FrameBuffer[i] = pBuff[i];

    GplSendData(g_FrameBuffer, length);
}

/*************************************************************************************************************************************************/
//NAME:         GPL0_GetByte
//DESCRPTION:   Get Frame Data Byte
//INPUT:        (byte*) Pointer to Frame buffer
//RETURN:       (INT8U) Byte in the frame
//NOTE:         
/*************************************************************************************************************************************************/
byte GPL_GetByte(byte* frame)
{
    byte rv = 0;
    if(frame)
    {
        rv = frame[GPL_BYTE_FIRST_DATA];
    }
    return rv;
}

/*************************************************************************************************************************************************/
//NAME:         GPL0_GetWord
//DESCRPTION:   Get Frame Data Word (2 Byte)
//INPUT:        (byte*) Pointer to Frame buffer
//RETURN:       (INT16U) Word in the frame
//NOTE:         
/*************************************************************************************************************************************************/
word GPL_GetWord(byte* frame)
{
    word rv = 0;
    if(frame)
    {
        rv = MAKEWORD(frame[GPL_BYTE_FIRST_DATA], frame[GPL_BYTE_FIRST_DATA + 1]);
    }
    return rv;
}

/*************************************************************************************************************************************************/
//NAME:         GPL0_GetDWord
//DESCRPTION:   Get Frame Data Unsigned Long (4 Byte)
//INPUT:        (byte*) Puntatore al buffer del frame
//RETURN:       (unsigned long) Unsigned Long in the frame
//NOTE:         
/*************************************************************************************************************************************************/
unsigned long GPL_GetDWord(byte* frame)
{
    unsigned long rv = 0;
    word hiWord = 0, loWord = 0;
    if(frame)
    {
        hiWord = MAKEWORD(frame[GPL_BYTE_FIRST_DATA], frame[GPL_BYTE_FIRST_DATA + 1]);
        loWord = MAKEWORD(frame[GPL_BYTE_FIRST_DATA + 2], frame[GPL_BYTE_FIRST_DATA + 3]);
        rv = (unsigned long)(((unsigned long)(loWord)) | (((unsigned long)(hiWord))<<16));
    }
    return rv;
}
