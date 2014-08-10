// (c) Copyright 2010-2012 MCQN Ltd.
// Released under Apache License, version 2.0
//
// Simple example to show how to use the HttpClient library
// Get's the web page given at http://<kHostname><kPath> and
// outputs the content to the serial port

#include <SPI.h>
#include <HttpClient.h>
#include <Ethernet.h>
#include <EthernetClient.h>

// Colour export pin decleration
int redPin = 9;
int greenPin = 10;
int bluePin = 3; 

// Name of the server we want to connect to
const char kHostname[] = "mywingman.alexander-thomas.net";
// Path to download (this is the bit after the hostname in the URL
// that you want to download
const char kPath[] = "/user/6b8189b581066829239fecb4f3190e89354e29f0e989b021c0292c8fe987fee5/color";

byte mac[] = { 
  0xDE, 0xB8, 0xF5, 0x71, 0xA8, 0xC1 };

// Number of milliseconds to wait without receiving any data before we give up
const int kNetworkTimeout = 30*1000;
// Number of milliseconds to wait if no data is available before trying again
const int kNetworkDelay = 1000;

int colors[3] = {0, 0, 0};  

void setup()
{
  // initialize serial communications at 9600 bps:
  Serial.begin(9600); 
  
  //Initialize the lamp with 0,0,0
  writeColorsToLamp(colors);
  
  while (Ethernet.begin(mac) != 1)
  {
    Serial.println("Error getting IP address via DHCP, trying again...");
    delay(15000);
  }  
}

void loop()
{  
  String colorsString;

  //Fetch colors from the remote api
  colorsString = fetchColorsFromRemote();
  //Process return valuer from remote
  processColorsString(colorsString, colors);
  //Write color valuzes to the lamp
  writeColorsToLamp(colors);

  // And just stop, now that we've tried a download
  while(1);
}

void writeColorsToLamp(int colors[]){
  Serial.println("writeColorsToLamp()");

  Serial.println("Setting red to " + String(colors[0]));
  Serial.println("Setting green to " + String(colors[1]));
  Serial.println("Setting blue to " + String(colors[2]));

  analogWrite(redPin, colors[0]);
  analogWrite(greenPin, colors[1]);
  analogWrite(bluePin, colors[2]);
}

void processColorsString(String colorsString, int colors[]){
  Serial.println("processColorsString()");

  String redStr;
  String greenStr;
  String blueStr;

  redStr = colorsString.substring(3,6);
  colors[0] = redStr.toInt();

  greenStr = colorsString.substring(7,10);
  colors[1] = greenStr.toInt();

  blueStr = colorsString.substring(11,14);
  colors[2] = blueStr.toInt();  
}

//Fetches color code from the wingman api
String fetchColorsFromRemote(){
  Serial.println("fetchColorsFromRemote()");

  String response;
  int err = 0;
  EthernetClient c;
  HttpClient http(c);

  err = http.get(kHostname, kPath);
  if (err == 0)
  {
    Serial.println("startedRequest ok");

    err = http.responseStatusCode();
    if (err >= 0)
    {
      Serial.print("Got status code: ");
      Serial.println(err);

      // Usually you'd check that the response code is 200 or a
      // similar "success" code (200-299) before carrying on,
      // but we'll print out whatever response we get

      err = http.skipResponseHeaders();
      if (err >= 0)
      {
        int bodyLen = http.contentLength();
        Serial.print("Content length is: ");
        Serial.println(bodyLen);
        Serial.println();
        Serial.println("Body returned follows:");

        // Now we've got to the body, so we can print it out
        unsigned long timeoutStart = millis();
        char c;
        // Whilst we haven't timed out & haven't reached the end of the body
        while ( (http.connected() || http.available()) &&
          ((millis() - timeoutStart) < kNetworkTimeout) )
        {
          if (http.available())
          {
            c = http.read();
            // Print out this character
            Serial.print(c);
            response += c;

            bodyLen--;
            // We read something, reset the timeout counter
            timeoutStart = millis();
          }
          else
          {
            // We haven't got any data, so let's pause to allow some to
            // arrive
            delay(kNetworkDelay);
          }
        }
      }
      else
      {
        Serial.print("Failed to skip response headers: ");
        Serial.println(err);
      }
    }
    else
    {    
      Serial.print("Getting response failed: ");
      Serial.println(err);
    }
  }
  else
  {
    Serial.print("Connect failed: ");
    Serial.println(err);
  }
  http.stop();
  return response;
}


