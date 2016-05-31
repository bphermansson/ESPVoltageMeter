/*
* Measure voltage from a solar cell
* and send the value to a webserver. 
* Here the value is sent to an Emoncms 
* installation but that can be altered.
* Uses a ESP8266 (Olimex ESP8266-DEV) and Arduino IDE.
* https://www.olimex.com/Products/IoT/ESP8266-EVB/resources/ESP8266-EVB-how-to-use-Arduino.pdf
*
* To measure the voltage a voltage divider connected to the ADC is used. The ESP:s ADC can not measure more than 
* 1 volt. We connect 1k to ground from ADC (pin16) and 10k to the source (+ on the solar cell, 
* ground goes to ESP:s ground, pin 2/3).
* 
* 
* https://learn.sparkfun.com/tutorials/esp8266-thing-hookup-guide/using-the-arduino-addon (Pin mappings, about the ADC)
*
* � Patrik Hermansson 2016
*/

#include <ESP8266WiFi.h>

const char* ssid = "dd-wrt"; // Which wireless network to use?
const char* password = "your-password";  // Wifi password, if any

// Values for Emoncms
const char* host = "192.168.1.3";
String apikey = "b0660d27cc88ee1a916b7bbb4b9f8baf";
String nodeid = "25";
String url = "/emoncms/input/post.json?apikey="+apikey+"&node="+nodeid+"&csv=";

// Remote Emoncms 
const char*  remotedomain="emoncms.org";
String remoteemoncmspath = "emoncms";
String remoteapikey = "bdd96271de3ff345c087afa276d1a619";
String remotenodeid = "25";
String remoteurl = "/input/post.json?node="+nodeid+"&csv=";

void setup() {
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
void loop() {
/* 
 *  
 *  Measure solar cell voltage
 */
 int adc = analogRead(A0);
 double Voltage;
 double realVoltage; // Real voltage, before the voltage divider
// The ESP:s ADC can be up to 1V, so we dont have to adjust for the supply voltage
Voltage = adc / 1023.0;
realVoltage = Voltage * 11,0; // Divider with 10k/1k gives a factor of about 11

 Serial.print("Solar voltage (raw):");
 Serial.println(adc);
 Serial.println(Voltage);
 Serial.println(realVoltage);
 
  // Add voltage to url
  //String newurl="";
  //newurl=url + String(realVoltage);
  String newremoteurl="";
  newremoteurl=remoteurl + String(realVoltage) + "&apikey="+remoteapikey;

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
/*
  // Connect to host
  Serial.print("connecting to ");
  Serial.println(host);
  

  
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  Serial.print("Requesting URL: ");
  Serial.println(newurl);
  
  // This will send the request to the server
  client.print(String("GET ") + newurl + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(100);
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");
*/
  // Connect to remote Emoncms
    // Connect to host
  Serial.print("connecting to ");
  Serial.println(remotedomain);
  
  if (!client.connect(remotedomain, httpPort)) {
    Serial.println("Remote connection failed");
    return;
  }
  
  Serial.print("Requesting URL: ");
  Serial.println(newremoteurl);
  
  // This will send the request to the server
  client.print(String("GET ") + newremoteurl + " HTTP/1.1\r\n" +
               "Host: " + remotedomain + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(100);
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");

  // Wait 2 minutes to midnight (or 10 to next measurement). Here we could/should go into deep sleep...
  Serial.println("Wait...");
  delay(600000);

}

