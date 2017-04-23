/*  AP datalogger

  The circuit:
   ESP8266
   SD card attached to SPI bus as follows:
 ** MOSI - pin 13
 ** MISO - pin 12
 ** CLK - pin 14
 ** CS - pin 15

*/
#include "ESP8266WiFi.h"
#include <SPI.h>
#include <SD.h>

#define LED 2
const short chipSelect = 15;

void setup() {

  Serial.begin(115200);
  pinMode(LED, OUTPUT);

  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  //WiFi.disconnect();
  delay(100);

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
  Serial.println("Setup done");
}


void loop() {
  // make a string for assembling the data to log:
  short count = 10;
  //Serial.println("READY");
  //delay (1000);
  while (Serial.available()) {
    String xy = Serial.readString();
    while (count > 0) {
      count --;

      String dataString = "";
      dataString += xy;
      dataString.remove(dataString.length() - 1);
      dataString += ", 2.39,";

      //      Serial.println("scan start");
      // WiFi.scanNetworks will return the number of networks found
      int n = WiFi.scanNetworks();
      //      Serial.println("scan done");
      if (n == 0)
        Serial.println("no networks found");
      else
      {

        // sort by RSSI
        int indices[n];
        for (int i = 0; i < n; i++) {
          indices[i] = i;
        }
        for (int i = 0; i < n; i++) {
          for (int j = i + 1; j < n; j++) {
            //        Serial.print("WiFi.RSSI(indices[j] = ");
            //        Serial.print(WiFi.RSSI(indices[j]));
            //        Serial.print("WiFi.RSSI(indices[i] = ");
            //        Serial.println(WiFi.RSSI(indices[i]));
            if (WiFi.RSSI(indices[j]) > WiFi.RSSI(indices[i])) {
              //          int temp = indices[j];
              //          indices[j] = indices[i];
              //          indices[i] = temp;
              std::swap(indices[i], indices[j]);
            }
          }
        }

        //        Serial.print(n);
        //        Serial.println(" networks found");
        for (int i = 0; i < n; ++i)
        {
          // Print SSID and RSSI for each network found
          //      Serial.print(WiFi.RSSI(i));
          //      Serial.print(" ");
          dataString += String(WiFi.RSSI(indices[i]));
          dataString += String(" ");
          delay(10);
          //      Serial.print(WiFi.BSSIDstr(i));
          dataString += String(WiFi.BSSIDstr(indices[i]));
          dataString += String(",");
          //      Serial.print(",");
          delay(10);
        }
      }
      File dataFile = SD.open("datalog.txt", FILE_WRITE);
      if (dataFile) {
        dataString.remove(dataString.length() - 1);
        dataString += String(n);
        // print to the serial port too:
        Serial.println(dataString);
        digitalWrite(LED, HIGH);
      }
      // if the file isn't open, pop up an error:
      else {
        Serial.println("error opening datalog.txt");
      }
      //      Serial.println("");

      // Wait a bit before scanning again
      // delay(200);
            if (count == 0) // Serial.println("READY");
            digitalWrite(LED, LOW);
    }
  }
}
