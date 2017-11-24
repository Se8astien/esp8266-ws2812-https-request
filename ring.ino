#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>

int PIN = 13; //D7
const char* ssid     = "Box";
const char* password = "pwd";

const char* host = "test.com";

const int httpsPort = 443;

// Use web browser to view and copy
// SHA1 fingerprint of the certificate
const char* fingerprint = "B3 F1 34 53 8G D9 84 B1 F1 06 10 64 92 EF E9 8D 1C 2E 45 E1";

String percent = "0";

//   Parameter 1 = number of pixels in strip
//   Parameter 2 = pin number (most are valid)
//   Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   16 is the number of led
Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, PIN, NEO_GRB + NEO_KHZ800);

 
void setup() {
  Serial.begin(115200);
  delay(10);

  strip.begin();
  strip.setBrightness(30); //adjust brightness here
  strip.show(); // Initialize all pixels to 'off'
  
 
  //connecting to a WiFi network

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
  
  delay(100);

  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
   WiFiClientSecure client;
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  if (client.verify(fingerprint, host)) {
    Serial.println("Certificate matches");
  } else {
    Serial.println("Certificate doesn't match");
  }

  // We now create a URI for the request
  String url = "/api/url";

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") +url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(100);

  // Read all the lines of the reply from server and print them to Serial
  Serial.println("Respond:");
  
    String json = "";
  boolean httpBody = false;
  while (client.available()) {
    String line = client.readStringUntil('\r');
    if (!httpBody && line.charAt(1) == '{') {
      httpBody = true;
    }
    if (httpBody) {
      json += line;
    }
  }
  StaticJsonBuffer<400> jsonBuffer;
  Serial.println("Got data:");
  Serial.println(json);
  JsonObject& root = jsonBuffer.parseObject(json);
  String data = root["datas"];
  Serial.println(data);

 
   // count led to show
   int percent = root["datas"]["percent"];
   double nbled = (double)percent / (double)100;
   nbled = (int)(nbled * 16) ;

   if(nbled != 0 && nbled < 16 ){
      for(int i = 16; i >= 16-nbled; i-- ) { // show led
          if(i >= 10){
              strip.setPixelColor(i, Wheel(((i * 255 / (strip.numPixels()) + 43)  ) & 255));
          } 
          if (6 <= i && i <= 9){
              strip.setPixelColor(i, Wheel(((i * 255 / (strip.numPixels()) + 45)  ) & 255));
          }
          if (5 >= i){
            Serial.println("red");
             strip.setPixelColor(i, Wheel(((i * 255 / (strip.numPixels())  + 46) ) & 255));
          }
          
      }
      for(int i = 0; i < 16-nbled; i++ ) { //other led off
          strip.setPixelColor(i, 0, 0, 0);
      }
     }else if (nbled >= 16 ){
       for(int i = 0; i < 16; i++ ) { // all led red
          strip.setPixelColor(i, 255, 0, 0);
       }
     } else{
      for(int i = 0; i < 16; i++ ) { // all led off
          strip.setPixelColor(i, 0, 0, 0);
      }
     }
     
  strip.show(); // on affiche 
  strip.setBrightness(20); 
  Serial.println("closing connection");
  delay(100);
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
