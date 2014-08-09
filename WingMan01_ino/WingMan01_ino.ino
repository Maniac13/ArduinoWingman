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

// This example downloads the URL "http://arduino.cc/"

// Name of the server we want to connect to
const char kHostname[] = "mywingman.alexander-thomas.net ";
// Path to download (this is the bit after the hostname in the URL
// that you want to download
const char kPath[] = "/user/6b8189b581066829239fecb4f3190e89354e29f0e989b021c0292c8fe987fee5/color";

byte mac[] = { 0xDE, 0xB8, 0xF5, 0x71, 0xA8, 0xC1 };

// Number of milliseconds to wait without receiving any data before we give up
const int kNetworkTimeout = 30*1000;
// Number of milliseconds to wait if no data is available before trying again
const int kNetworkDelay = 1000;

void setup()
{
  // initialize serial communications at 9600 bps:
  Serial.begin(9600); 

  while (Ethernet.begin(mac) != 1)
  {
    Serial.println("Error getting IP address via DHCP, trying again...");
    delay(15000);
  }  
}

void loop()
{
    Serial.println("Loop");
    String colors;
    // String color1;
    // String color2;
    // String color3;
    
    // if(magicBluetoothEvent == true)
    // {
        colors = fetchColorCode();
        logMsg(colors);
    //     color1 = color.substr(1,3);
    //     color2 = color.substr(5,8);
    //     color3 = color.substr(10,13);
    //
    //     //Sende Farben an die Lampe
    //
    //
    //     //Warte 90s
    //     delay(90);
    //
    //     if(magicBluetoothEvent == false){
    //         //Schalte Lampe aus
    //     }else{
    //         delay(90);
    //     }
    // }

delay(2000);
    // And just stop, now that we've tried a download
    //while(1);
}

String fetchColorCode(){
    int err = 0;
  
    EthernetClient c;
    HttpClient http(c);
    String response;
  
    err = http.get(kHostname, kPath);
    if (err == 0)
    {
      logMsg("startedRequest ok");

      err = http.responseStatusCode();
      if (err >= 0)
      {
        logMsg("Got status code: ");
        logMsg(String(err));

        // Usually you'd check that the response code is 200 or a
        // similar "success" code (200-299) before carrying on,
        // but we'll print out whatever response we get
        err = http.skipResponseHeaders();
        if (err >= 0)
        {
          int bodyLen = http.contentLength();
          logMsg("Content length is: ");
          logMsg(String(bodyLen));
          logMsg("");
      
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
                  response += c;
                
                  // Print out this character
                  logMsg(String(c));
               
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
          logMsg("Failed to skip response headers: ");
          logMsg(String(err));
        }
      }
      else
      {    
        logMsg("Getting response failed: ");
        logMsg(String(err));
      }
    }
    else
    {
      logMsg("Connect failed: ");
      logMsg(String(err));
    }
    http.stop();
    
    return response;
}

void logMsg(String msg){
    bool debug = true;
    if(debug = true)
    {
       Serial.println(msg); 
    }
}
