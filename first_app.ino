#include <PubSubClient.h>
#include <LWiFi.h>
#include <LWiFiClient.h>
#include <ATT_IOT.h>
#include <Servo.h>

#include <SPI.h>  //required to have support for signed/unsigned long type.

#define WIFI_AP "cdb guest"
#define WIFI_PASSWORD "kodbihajnd"
#define WIFI_AUTH LWIFI_WPA  // choose from LWIFI_OPEN, LWIFI_WPA, or LWIFI_WEP according to your WiFi AP configuration
#define LS_PEAK_VALUE   540
#define THRESHOLD       ((10 * LS_PEAK_VALUE) / 100)


char deviceId[] = "1QWMqf4kj8I8chbi4hgQPt2o";
char clientId[] = "Nour";
char clientKey[] = "y1shgmub1wj";
byte mac[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01};  //Adapt to your Arduino MAC address if needed



void callback(char* topic, byte* payload, unsigned int length);
LWiFiClient wifiClient;
char httpServer[] = "api.smartliving.io";
char* mqttServer = "broker.smartliving.io";
PubSubClient pubSub(mqttServer, 1883, callback, wifiClient);
ATTDevice Device(deviceId, clientId, clientKey);  //create the object that provides the connection to the cloud to manage the device.
int pos = 0;    // variable to store the servo position
Servo myservo;  // create servo object to control a servo
// int turn = 0;
void setup()
{  myservo.attach(3);  // attaches the servo on pin 9 to the servo object

  Serial.begin(9600);  // init serial link for debugging

  while (0 == LWiFi.connect(WIFI_AP, LWiFiLoginInfo(WIFI_AUTH, WIFI_PASSWORD))){
    Serial.println("Connecting to WiFi ");
    Serial.print(WIFI_AP);
    Serial.println();
    delay(600);
  }
  Serial.println("Connected!");

  while(!Device.Connect(&wifiClient, httpServer))  // connect the device with the IOT platform
    Serial.println("retrying");
  while(!Device.Subscribe(pubSub))  // make certain that we can receive message from the iot platform (activate mqtt)
    Serial.println("retrying");

}

unsigned long time;  //only send every x amount of time.
unsigned int prevVal =0;
void loop()
{
delay(1000);

  Device.Process();
 /* if (turn = 1 )
  {
    for (pos = 0; pos <= 180; pos += 1) 
     { // goes from 0 degrees to 180 degrees  in steps of 1 degree
     myservo.write(pos);              // tell servo to go to position in variable 'pos'
     delay(15);                       // waits 15ms for the servo to reach the position
   }
   }
   else {
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }}
 */  
}

// Callback function: handles messages that were sent from the IOT platform to this device.
void callback(char* topic, byte* payload, unsigned int length)
{
  String msgString;
  
    char message_buff[length + 1];  //need to copy over the payload so that we can add a /0 terminator, this can then be wrapped inside a string for easy manipulation.
    strncpy(message_buff, (char*)payload, length);  //copy over the data
    message_buff[length] = '\0';   //make certain that it ends with a null

    msgString = String(message_buff);
    msgString.toLowerCase();  //to make certain that our comparison later on works ok (it could be that a True or False was sent)
  

  int* idOut = NULL;

  
    // get asset pin
    int pinNr = Device.GetPinNr(topic, strlen(topic));

    if (pinNr == 3) {
      if ( strcmp(message_buff, "true") ) {
        for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
        myservo.write(pos);              // tell servo to go to position in variable 'pos'
        delay(15);                       // waits 15ms for the servo to reach the position
      }
      } else {
        for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
        myservo.write(pos);              // tell servo to go to position in variable 'pos'
        delay(15);                       // waits 15ms for the servo to reach the position
      } 
      }
    }
    
    Serial.print("Payload: ");
    Serial.println(msgString);
    Serial.print("topic: ");
    Serial.println(topic);
    
  
  if(idOut != NULL)
    Device.Send(msgString, *idOut);
}
