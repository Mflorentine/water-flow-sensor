#include<ESP8266WiFi.h>
#include<PubSubClient.h>
const char* mqtt_server = "broker.mqttdashboard.com";
WiFiClient espclient;
PubSubClient client(espclient);
const char* ssid = "Esperance";
const char* password = "Esperance2";
volatile int FlowPulse; //measuring the rising edges of the signal;
 int Calc;                               
int flowsensor = D3;    //The pin-2 location of the sensor Always use this pin as we are using interrupt 0
int ledPin= D5;
void callback(char* topic, byte* payload, unsigned int length) 
{
} //end callback
void reconnect() {
  while (!client.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      client.publish("eric/sensor/data","Flow Sensor Readings:");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(6000);
    }
  }
} //end reconnect()
void setup_wifi(){
   WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.print(".");
    }
  randomSeed(micros());
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
client.setServer(mqtt_server, 1883);
}
void rpm ()     //This is the function that the interupt calls 
{ 
  FlowPulse++;  //This function measures the rising and falling edge of the hall effect sensors signal
}
void setup() {
  pinMode(ledPin,OUTPUT);     
   pinMode(flowsensor, INPUT); //initializes digital pin 2 as an input
   Serial.begin(9600);         //This is the setup function where the serial port is initialised,
   attachInterrupt(0,rpm, RISING); //and the interrupt is attached on Pin 2 (INT 0)
   setup_wifi();
   reconnect();
client.setServer(mqtt_server, 1883);
}
 
void loop() { 
  if (!client.connected()) {
  reconnect();
  }
  client.loop();
  client.setCallback(callback);
  client.loop();
      
 FlowPulse = 0;      //Set NbTops to 0 ready for calculations
 sei();            //Enables interrupts
 delay (1000);      //Wait 1 second
 cli();            //Disable interrupts
 Calc = (FlowPulse * 60 / 7.5); //(Pulse frequency x 60) / 7.5Q, = flow rate in L/hour 
 if(Calc>50 & Calc<300)
 {
  digitalWrite(ledPin,HIGH);
 Serial.println("there is sufficient flow in the pipe");
 }
 else
 {
 digitalWrite(ledPin,LOW);
 Serial.println("leakage is detected");
 }
 Serial.print (Calc, DEC); //Prints the number calculated above
 Serial.println (" L/hour"); //Prints "L/hour"
 Serial.print("Publishing Data to Mqtt Broker: ");
 char str[16];
 itoa( Calc,str,10);
 Serial.println(str);  
 client.publish("eric/sensor/data",str);
 client.publish("water flow data",str);
 Serial.println("SENT!!!!!!!!!");
 }
  
 
