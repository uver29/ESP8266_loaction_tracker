#include "ESP8266WiFi.h"
#include <WiFiClientSecure.h>


char my_ssid[] = "Redmi";
char my_password[] = "password7";
char my_bssid[] = "AC:C1:EE:A6:12:69";

int index_loc = 0;
String bssid[10][15];
String rssi[10][15];

boolean connection_available = false;

String json_string = "";

WiFiClientSecure client;

void setup(){
  
  Serial.begin(9600);
  Serial.println();
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  Serial.println("Setup done");
  Serial.println();
  
  
  
}

void loop() {

  connection_available = false;
  int n = WiFi.scanNetworks();

  // unwiredlabs LocationAPI supports minimum of 2 and maximum of 15 WiFi objects in 1 request
  if(n>=1 && n<=15) {                                                               // change 2 to 1
    for (int j = 0; j < n; j++) {
      bssid[index_loc][j] = WiFi.BSSIDstr(j);
      rssi[index_loc][j] = String(WiFi.RSSI(j));
      // if the saved WiFi network is available
      if(bssid[index_loc][j] == my_bssid) {
        connection_available = true;
      }
    }
    for(int p = n; p<=14; p++) {
      bssid[index_loc][p] = "00:00:00:00:00:00";
      rssi[index_loc][p] = "0";
    }
    index_loc++;
  }

  if(WiFi.status() == WL_CONNECTED) {
    json_string = "{\n";
    for(int i = 0; i <= index_loc; i++) {
      json_string += "    \"" + String(i+1) + "\": {\n";
      for(int k = 0; k<=14; k++) {
        json_string += "        \"" + String(k+1) + "\": {\"bssid\": \"" + bssid[i][k] + "\", \"rssi\": \"" + rssi[i][k] + "\"}";
        if(k < 14) {
          json_string += ",\n";
        }
        else {
          json_string += "\n";
        }
      }
      if(i < index_loc) {
        json_string += "},\n";
      }
      else {
        json_string += "}\n";
      }
    }
    json_string += "}\n";
    Serial.println(json_string);
  
    
    client.setInsecure();
    delay(100);
    
    char host[] = "7au9zvw1k7.execute-api.us-west-2.amazonaws.com";
    String url = "/first_stage/bssi";
    
    if (client.connect(host, 443)) {
      Serial.println("Connected to amazon");
      client.println("POST " + url + " HTTP/1.1");
      client.print("Host: ");
      client.println(host);
      client.println("Connection: close");
      client.println("Content-Type: application/json");
      client.println("User-Agent: Arduino/1.0");
      client.print("Content-Length: ");
      client.println(json_string.length());
      client.println();
      client.print(json_string);
      delay(500);
    }
    
    while (client.available()) {
      String line = client.readStringUntil('\r');
      Serial.println(line);
    }

    // once all the data is sent to the database the index_loc resets itself
    index_loc = 0;
  }
  
  else if(connection_available) {
    Serial.print("Connecting to ");
    Serial.println(my_ssid);
    WiFi.begin(my_ssid, my_password);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected to wifi network");
  }


//  
//  delay(30000);  //Send a request every 30 seconds
}
