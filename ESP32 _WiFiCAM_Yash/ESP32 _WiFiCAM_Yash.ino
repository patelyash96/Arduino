#include <WebServer.h>
#include <WiFi.h>
#include <esp32cam.h>

const char* WIFI_SSID = "elecom-c403af";
const char* WIFI_PASS = "5pmubh3w";

WebServer server(80);

static auto loRes = esp32cam::Resolution::find(320, 240);
static auto hiRes = esp32cam::Resolution::find(800, 600);

void handleBmp(){
  
  if (!esp32cam::Camera.changeResolution(loRes)){
    Serial.println("SET-LOW_RES FAIL");
  }

  auto frame = esp32cam::capture();

  if(frame == nullptr){
    Serial.println("CAPTURE FAIL");
    server.send(503, "", "");
    return;
  }
  Serial.println("CAPTURE OK!! %dx%d %db\n", frame->getWidth(), frame->getHight(), static_cast<int>(frame->size()));

  if(!frame->toBmp()){
    Serial.println("CONVERT FAIL");
    server.send(503, "", "");
    return;    
  }
  Serial.println("CONVERT OK!! %dx%d %db\n", frame->getWidth(), frame->getHight(), static_cast<int>(frame->size()));

  server.setContentLength(frame->size());
  server.send(200, "img/bmp");
  WiFiClient client = server.client();
  frame->writeTo(client);
}

void serveJpg(){

  auto frame = esp32cam::capture();

  if(frame == nullptr){
    Serial.println("CAPTURE FAIL");
    server.send(503, "", "");
    return;
  }
  Serial.println("CAPTURE OK!! %dx%d %db\n", frame->getWidth(), frame->getHight(), static_cast<int>(frame->size()));

  server.setContentLength(frame->size());
  server.send(200, "img/jpeg");
  WiFiClient client = server.client();
  frame->writeTo(client);
}

void handleJpgLo(){

  if (!esp32cam::Camera.changeResolution(loRes)){
    Serial.println("SET-LOW_RES FAIL");
  }  
  serveJpg();
}

void handleJpgHi(){

  if (!esp32cam::Camera.changeResolution(hiRes)){
    Serial.println("SET-HIGH_RES FAIL");
  }  
  serveJpg();
}

void handleJpg(){

  server.sendHeader("Location", "cam-hi.jpg");
  server.send(302, "", "");
}

void handleMjpeg(){

  if (!esp32cam::Camera.changeResolution(hiRes)){
    Serial.println("SET-HIGH_RES FAIL");
  }

  Serial.println("STREAM BEGIN");
  WiFiClient client = server.client();
  auto startTime = milis();
  int res = esp32::Camera.streamMjpg(client);
  if(res <= 0){
    Serial.println("STREAM ERROR!! %d\n", res);
  }

  auto duration = milis() - startTime;
  Serial.println("STREAM END %dfrm %0.2ffps\n", res, 1000.0 * res / duration);
}

void setup() {
  Serial.begin(115200);  
  Serial.println();
  
  {
    using namespace esp32cam;

    Config cfg;
    cfg.setPins(pins::AiThinker);
    cfg.setResolution(hiRes);
    cfg.setBufferCount(2);
    cfg.setJpeg(80);

    bool ok = Camera.begin(cfg);
    Serial.println(ok ? "CAMERA OK!!" : "CAMERA FAIL!!");
  }
  
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
  }

  Serial.print("http://");
  Serial.println(WiFi.localIP());
  Serial.println("  /cam.bmp");
  Serial.println("  /cam-lo.jpg");
  Serial.println("  /cam-hi.jpg");
  Serial.println("  /cam.mjpeg");

  server.begin();
}

void loop() {
  
  server.handleClient();
}
