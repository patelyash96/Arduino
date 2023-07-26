#include "Arduino.h"
#include "WiFi.h"
#include "Audio.h"

#define I2S_DOUT 25
#define I2S_BCLK 27
#define I2S_LRC 26

Audio audio;

//String ssid = "RokuGoHouse";
//String password = "rokugohouse2";

String ssid = "HUAWEI P30 lite";
String password = "12345678";

void setup() {

  Serial.begin(115200);
  delay(10);

  WiFi.disconnect();
  WiFi.mode(WIFI_STA);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid.c_str(), password.c_str());
   while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(100);
  Serial.println("Ready Play...");

}

void loop()
{

  while (!Serial.available())
  {
    audio.loop();
  }
  while (Serial.available())
  {
    String input = Serial.readString();
    delay(100);
    audio.connecttospeech(input.c_str(), "ja-jp");
    //audio.connecttospeech(input.c_str(), "en-us");
  }

}

