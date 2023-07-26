/* ======================================== Including the libraries. */
#include "esp_camera.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include <WiFi.h>
#include "esp_http_server.h"
/* ======================================== */

/* ======================================== Select camera model */
//#define CAMERA_MODEL_WROVER_KIT
//#define CAMERA_MODEL_ESP_EYE
//#define CAMERA_MODEL_M5STACK_PSRAM
//#define CAMERA_MODEL_M5STACK_WITHOUT_PSRAM
//#define CAMERA_MODEL_M5STACK_WITHOUT_PSRAM
#define CAMERA_MODEL_AI_THINKER
/* ======================================== */

/* ======================================== GPIO of camera models */
#if defined(CAMERA_MODEL_WROVER_KIT)
  #define PWDN_GPIO_NUM    -1
  #define RESET_GPIO_NUM   -1
  #define XCLK_GPIO_NUM    21
  #define SIOD_GPIO_NUM    26
  #define SIOC_GPIO_NUM    27
  
  #define Y9_GPIO_NUM      35
  #define Y8_GPIO_NUM      34
  #define Y7_GPIO_NUM      39
  #define Y6_GPIO_NUM      36
  #define Y5_GPIO_NUM      19
  #define Y4_GPIO_NUM      18
  #define Y3_GPIO_NUM       5
  #define Y2_GPIO_NUM       4
  #define VSYNC_GPIO_NUM   25
  #define HREF_GPIO_NUM    23
  #define PCLK_GPIO_NUM    22

#elif defined(CAMERA_MODEL_M5STACK_PSRAM)
  #define PWDN_GPIO_NUM     -1
  #define RESET_GPIO_NUM    15
  #define XCLK_GPIO_NUM     27
  #define SIOD_GPIO_NUM     25
  #define SIOC_GPIO_NUM     23
  
  #define Y9_GPIO_NUM       19
  #define Y8_GPIO_NUM       36
  #define Y7_GPIO_NUM       18
  #define Y6_GPIO_NUM       39
  #define Y5_GPIO_NUM        5
  #define Y4_GPIO_NUM       34
  #define Y3_GPIO_NUM       35
  #define Y2_GPIO_NUM       32
  #define VSYNC_GPIO_NUM    22
  #define HREF_GPIO_NUM     26
  #define PCLK_GPIO_NUM     21

#elif defined(CAMERA_MODEL_M5STACK_PSRAM_B)
  #define PWDN_GPIO_NUM     -1
  #define RESET_GPIO_NUM    15
  #define XCLK_GPIO_NUM     27
  #define SIOD_GPIO_NUM     22
  #define SIOC_GPIO_NUM     23
  
  #define Y9_GPIO_NUM       19
  #define Y8_GPIO_NUM       36
  #define Y7_GPIO_NUM       18
  #define Y6_GPIO_NUM       39
  #define Y5_GPIO_NUM        5
  #define Y4_GPIO_NUM       34
  #define Y3_GPIO_NUM       35
  #define Y2_GPIO_NUM       32
  #define VSYNC_GPIO_NUM    25
  #define HREF_GPIO_NUM     26
  #define PCLK_GPIO_NUM     21

#elif defined(CAMERA_MODEL_M5STACK_WITHOUT_PSRAM)
  #define PWDN_GPIO_NUM     -1
  #define RESET_GPIO_NUM    15
  #define XCLK_GPIO_NUM     27
  #define SIOD_GPIO_NUM     25
  #define SIOC_GPIO_NUM     23
  
  #define Y9_GPIO_NUM       19
  #define Y8_GPIO_NUM       36
  #define Y7_GPIO_NUM       18
  #define Y6_GPIO_NUM       39
  #define Y5_GPIO_NUM        5
  #define Y4_GPIO_NUM       34
  #define Y3_GPIO_NUM       35
  #define Y2_GPIO_NUM       17
  #define VSYNC_GPIO_NUM    22
  #define HREF_GPIO_NUM     26
  #define PCLK_GPIO_NUM     21

#elif defined(CAMERA_MODEL_AI_THINKER)
  #define PWDN_GPIO_NUM     32
  #define RESET_GPIO_NUM    -1
  #define XCLK_GPIO_NUM      0
  #define SIOD_GPIO_NUM     26
  #define SIOC_GPIO_NUM     27
  
  #define Y9_GPIO_NUM       35
  #define Y8_GPIO_NUM       34
  #define Y7_GPIO_NUM       39
  #define Y6_GPIO_NUM       36
  #define Y5_GPIO_NUM       21
  #define Y4_GPIO_NUM       19
  #define Y3_GPIO_NUM       18
  #define Y2_GPIO_NUM        5
  #define VSYNC_GPIO_NUM    25
  #define HREF_GPIO_NUM     23
  #define PCLK_GPIO_NUM     22
#else
  #error "Camera model not selected"
#endif
/* ======================================== */

// LEDs GPIO
#define LED_OnBoard 33
#define LED_Flash 4

/* ======================================== Variables declaration */
// setting PWM properties
const int PWM_freq = 5000;
const int ledChannel = 5;
const int PWM_resolution = 8;

/* ======================================== */

/* ======================================== Access Point Declaration and Configuration */
const char* ssid = "ESP32-CAM-ROBOT-CLUB";  //--> access point name
const char* password = "robotclub22"; //--> access point password

IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
/* ======================================== */

/* ======================================== */
#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";
/* ======================================== */

/* ======================================== Empty handle to esp_http_server */
httpd_handle_t index_httpd = NULL;
httpd_handle_t stream_httpd = NULL;
/* ======================================== */

/* ======================================== HTML code for index / main page */
static const char PROGMEM INDEX_HTML[] = R"rawliteral(
<html>

  <head>
  
    <title>ESP32-CAM Pan and Tilt Control</title>
    
    <meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no">
    
    <style>
    
      body { font-family: Arial; text-align: center; margin:0px auto; padding-top: 10px;}

      /* ----------------------------------- Slider */
      .slidecontainer {
        width: 100%;
      }

      .slider {
        -webkit-appearance: none;
        width: 50%;
        height: 10px;
        border-radius: 5px;
        background: #d3d3d3;
        outline: none;
        opacity: 0.7;
        -webkit-transition: .2s;
        transition: opacity .2s;
      }

      .slider:hover {
        opacity: 1;
      }

      .slider::-webkit-slider-thumb {
        -webkit-appearance: none;
        appearance: none;
        width: 20px;
        height: 20px;
        border-radius: 50%;
        background: #04AA6D;
        cursor: pointer;
      }

      .slider::-moz-range-thumb {
        width: 20px;
        height: 20px;
        border-radius: 50%;
        background: #04AA6D;
        cursor: pointer;
      }
      /* ----------------------------------- */
      
      /* ----------------------------------- Button */
      .button {
        display: inline-block;
        padding: 10px;
        font-size: 12px;
        cursor: pointer;
        text-align: center;
        text-decoration: none;
        outline: none;
        color: #fff;
        background-color: #4CAF50;
        border: none;
        border-radius: 15px;
        box-shadow: 0 6px #999;
        -webkit-touch-callout: none;
        -webkit-user-select: none;
        -khtml-user-select: none;
        -moz-user-select: none;
        -ms-user-select: none;
        user-select: none;
        width: 15%
      }

      .button:hover {background-color: #3e8e41}

      .button:active {
        background-color: #3e8e41;
        box-shadow: 0 2px #666;
        transform: translateY(4px);
      }
      /* ----------------------------------- */
      
      /* ----------------------------------- */
      .space {
        width: 15%;
        height: auto;
        display: inline-block;
      }
      /* ----------------------------------- */

      /* ----------------------------------- Stream Viewer */
      img {
        width: auto ;
        max-width: 100% ;
        height: auto ; 
      }
      /* ----------------------------------- */
      
    </style>
    
  </head>
  
  <body>
  
    <h2>Rakuten Technology Conference 2022</h2>
    <h3>Robot Club</h3>
    <h3>Humaoid Robot</h3>
    
    <img src="" id="vdstream">
    
    <br><br>
    
    <div class="slidecontainer">
      <span style="font-size:15;">LED Flash &nbsp;</span>
      <input type="range" min="0" max="20" value="0" class="slider" id="mySlider">
    </div>

    <br>

    <div>
      <!-- "T" suffix in "start_timer_TC" (example : start_timer_TC('UT')) and -->
      <!-- "M" suffix in "start_timer_MP" (example : start_timer_MP('UM')) to differentiate between touch screen input and mouse pointer input on button. -->
      <button class="button" ontouchstart="start_timer_TC('HT')" ontouchend="stop_timer()" onmousedown="start_timer_MP('HM')" onmouseup="stop_timer()" onmouseleave="stop_timer()"><b>HI</b></button>
      <div class="space"></div>
      <button class="button" ontouchstart="start_timer_TC('BT')" ontouchend="stop_timer()" onmousedown="start_timer_MP('BM')" onmouseup="stop_timer()" onmouseleave="stop_timer()"><b>BOW</b></button>
      <div class="space"></div>
      <button class="button" ontouchstart="start_timer_TC('DT')" ontouchend="stop_timer()" onmousedown="start_timer_MP('DM')" onmouseup="stop_timer()" onmouseleave="stop_timer()"><b>DANCE</b></button>
      <br><br>
      <button class="button" ontouchstart="start_timer_TC('WT')" ontouchend="stop_timer()" onmousedown="start_timer_MP('WM')" onmouseup="stop_timer()" onmouseleave="stop_timer()"><b>WALK</b></button>
      <div class="space"></div>
      <button class="button" ontouchstart="start_timer_TC('ST')" ontouchend="stop_timer()" onmousedown="start_timer_MP('SM')" onmouseup="stop_timer()" onmouseleave="stop_timer()"><b>STOP</b></button>
    </div>
    
    <script>
      /* ----------------------------------- Calls the video stream link and displays it. */ 
      window.onload = document.getElementById("vdstream").src = window.location.href.slice(0, -1) + ":81/stream";
      /* ----------------------------------- */
      
      /* ----------------------------------- */
      var slider = document.getElementById("mySlider");
      /* ----------------------------------- */
      
      /* ----------------------------------- Variable declarations for timers and their intervals. */
      var myTmr;
      var myTmr_Interval = 250;
      /* ----------------------------------- */

      // Variable declarations to differentiate the sending of button commands when the button is held down or just a single press.
      var onmousedown_stat = 0;

      /* ----------------------------------- Variable declaration for sending commands to ESP32 Cam from index/web page. */
      let btn_commands = "";
      let btn_commands_rslt = "";
      /* ----------------------------------- */

      // Variable declarations to distinguish between button presses using the mouse or using the touch screen (on a mobile browser or in a browser on a touch screen device).
      var join_TM = 0;

      /* :::::::::::::::::::::::::::::::::::::::::::::::: Update the current slider value (each time you drag the slider handle) */
      slider.oninput = function() {
        let slider_pwm_val = "S," + slider.value;
        send_cmd(slider_pwm_val);
      }
      /* :::::::::::::::::::::::::::::::::::::::::::::::: */

      /* :::::::::::::::::::::::::::::::::::::::::::::::: Start and stop the timer */
      function start_timer_MP(cmds) {
        btn_commands = cmds;
        btn_commands_rslt = cmds.substring(0, 1);
        if (join_TM == 0) {
          myTmr = setInterval(myTimer, myTmr_Interval);
          if (onmousedown_stat == 0) send_btn_cmd(btn_commands_rslt);
          onmousedown_stat = 1;
        }
      }

      function start_timer_TC(cmds) {
        join_TM = 1;
        btn_commands_rslt = cmds.substring(0, 1);
        myTmr = setInterval(myTimer, myTmr_Interval);
        btn_commands = cmds;
        if (onmousedown_stat == 0) send_btn_cmd(btn_commands_rslt);
        onmousedown_stat = 1;
      }
      
      function stop_timer() {
        clearInterval(myTmr);
        onmousedown_stat = 0;
      }
      /* :::::::::::::::::::::::::::::::::::::::::::::::: */

      /* :::::::::::::::::::::::::::::::::::::::::::::::: Timer to send data or commands from index page/web server to ESP32 Cam. */
      function myTimer() {
        send_btn_cmd(btn_commands_rslt);
      }
      /* :::::::::::::::::::::::::::::::::::::::::::::::: */

      /* :::::::::::::::::::::::::::::::::::::::::::::::: Function to send commands to the ESP32 Cam whenever the button is clicked. */
      function send_btn_cmd(cmds) {
        let btn_cmd = "B," + cmds;
        send_cmd(btn_cmd);
      }
      /* :::::::::::::::::::::::::::::::::::::::::::::::: */
      
      /* :::::::::::::::::::::::::::::::::::::::::::::::: Function for sending commands */
      function send_cmd(cmds) {
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "/action?go=" + cmds, true);
        xhr.send();
      }
      /* :::::::::::::::::::::::::::::::::::::::::::::::: */
    </script>
  
  </body>
  
</html>
)rawliteral";
/* ======================================== */

/* ________________________________________________________________________________ Index handler function to be called during GET or uri request */
static esp_err_t index_handler(httpd_req_t *req){
  httpd_resp_set_type(req, "text/html");
  return httpd_resp_send(req, (const char *)INDEX_HTML, strlen(INDEX_HTML));
}
/* ________________________________________________________________________________ */

/* ________________________________________________________________________________ stream handler function to be called during GET or uri request. */
static esp_err_t stream_handler(httpd_req_t *req){
  camera_fb_t * fb = NULL;
  esp_err_t res = ESP_OK;
  size_t _jpg_buf_len = 0;
  uint8_t * _jpg_buf = NULL;
  char * part_buf[64];

  res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
  if(res != ESP_OK){
    return res;
  }

  /* ---------------------------------------- Loop to show streaming video from ESP32 Cam camera. */
  while(true){
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed (stream_handler)");
      res = ESP_FAIL;
    } else {
      if(fb->width > 400){
        if(fb->format != PIXFORMAT_JPEG){
          bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
          esp_camera_fb_return(fb);
          fb = NULL;
          if(!jpeg_converted){
            Serial.println("JPEG compression failed");
            res = ESP_FAIL;
          }
        } else {
          _jpg_buf_len = fb->len;
          _jpg_buf = fb->buf;
        }
      }
    }
    if(res == ESP_OK){
      size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _jpg_buf_len);
      res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
    }
    if(res == ESP_OK){
      res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
    }
    if(res == ESP_OK){
      res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
    }
    if(fb){
      esp_camera_fb_return(fb);
      fb = NULL;
      _jpg_buf = NULL;
    } else if(_jpg_buf){
      free(_jpg_buf);
      _jpg_buf = NULL;
    }
    if(res != ESP_OK){
      break;
    }
    sensor_t * s = esp_camera_sensor_get();  // collect camera sensor data
    s->set_vflip(s, 1);       // vertical flip camera
    //s->set_hmirror(s, 1);   // horizontal mirror image
  }
  /* ---------------------------------------- */
  return res;
}
/* ________________________________________________________________________________ */

/* ________________________________________________________________________________ cmd handler function to be called during GET or uri request. */
static esp_err_t cmd_handler(httpd_req_t *req){
  char*  buf;
  size_t buf_len;
  char variable[32] = {0,};
   
  buf_len = httpd_req_get_url_query_len(req) + 1;
  if (buf_len > 1) {
    buf = (char*)malloc(buf_len);
    if(!buf){
      httpd_resp_send_500(req);
      return ESP_FAIL;
    }
    if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
      if (httpd_query_key_value(buf, "go", variable, sizeof(variable)) == ESP_OK) {
      } else {
        free(buf);
        httpd_resp_send_404(req);
        return ESP_FAIL;
      }
    } else {
      free(buf);
      httpd_resp_send_404(req);
      return ESP_FAIL;
    }
    free(buf);
  } else {
    httpd_resp_send_404(req);
    return ESP_FAIL;
  }
 
  int res = 0;

  //Serial.print("Incoming command : ");
  //Serial.println(variable);
  //Serial.println();
  String getData = String(variable);
  String resultData = getValue(getData, ',', 0);

  /* ---------------------------------------- Controlling the LEDs on the ESP32 Cam board with PWM. */
  // Example :
  // Incoming command = S,10
  // S = Slider
  // 10 = slider value
  // I set the slider value range from 0 to 20.
  // Then the slider value is changed from 0 - 20 or vice versa to 0 - 255 or vice versa.
  if (resultData == "S") {
    resultData = getValue(getData, ',', 1);
    int pwm = map(resultData.toInt(), 0, 20, 0, 255);
    ledcSetup(ledChannel, PWM_freq, PWM_resolution);
    ledcAttachPin(LED_Flash, ledChannel);
    ledcWrite(ledChannel, pwm);
    //Serial.print("PWM LED On Board : ");
    //Serial.println(pwm);
  }
  /* ---------------------------------------- */

  /* ---------------------------------------- Processes and executes commands received from the index page/web server. */
  // Example :
  // Incoming Command = B,U
  // B = Button
  // U = Command to drive the robot.
  
  if (resultData == "B") {
    resultData = getValue(getData, ',', 1);
    
    if (resultData == "H") {
      //Serial.println("HI");
      Serial.println("#1P1427#2P1505#3P1573#4P1505#5P1505#6P1550#7P1505#8P1427#9P1505#10P1505#11P1622#12P1505#13P1505T500");
      delay(500);
    }

    if (resultData == "B") {
      //Serial.println("BOW");
      Serial.println("#1P1427#2P1505#3P1573#4P1505#5P1505#6P1505#7P1480#8P1427#9P1505#10P1505#11P1622#12P1505#13P1505T1000");
      delay(500);
      Serial.println("#1P1427#2P700#3P1720#4P2500#5P2300#6P1378#7P500#8P900#9P1505#10P1505#11P2000#12P1505#13P1505T1000");
      delay(1000);
      Serial.println("#1P1427#2P1505#3P1573#4P1505#5P1505#6P1505#7P1480#8P1427#9P1505#10P1505#11P1622#12P1505#13P1505T1000");
      delay(500);
    }

    if (resultData == "D") {
      //Serial.println("DANCE");
      Serial.println("#1P1427#2P1505#3P1573#4P1505#5P1505#6P1505#7P1480#8P1427#9P1505#10P1505#11P1622#12P1505#13P1505T500");
      delay(500);
      Serial.println("#1P1427#2P1505#3P1573#4P1505#5P1505#6P1550#7P1480#8P1427#9P1505#10P1720#11P1622#12P1505#13P1720T500");
      delay(500);
      Serial.println("#1P1427#2P1505#3P500#4P1100#5P1505#6P2500#7P2000#8P1427#9P1505#10P1300#11P1622#12P1505#13P1720T500");
      delay(500);
      Serial.println("#1P1427#2P1505#3P1100#4P2500#5P1505#6P2000#7P500#8P600#9P500#10P1505#11P1622#12P1505#13P1720T500");
      delay(500);
      Serial.println("#1P1427#2P1505#3P500#4P1100#5P1505#6P2500#7P2000#8P1427#9P1505#10P1300#11P1622#12P1505#13P1720T500");
      delay(500);
      Serial.println("#1P1427#2P1505#3P1100#4P2500#5P1505#6P2000#7P500#8P600#9P500#10P1505#11P1622#12P1505#13P1720T500");
      delay(500);
      Serial.println("#1P1427#2P1505#3P1573#4P1505#5P1505#6P1505#7P1480#8P1427#9P1505#10P1505#11P1622#12P1505#13P1505T500");
      delay(500);
      Serial.println("#1P1427#2P1505#3P1573#4P1505#5P1505#6P1505#7P1480#8P1427#9P1505#10P1280#11P1622#12P1505#13P1280T500");
      delay(500);
      Serial.println("#1P1427#2P1505#3P500#4P1100#5P1505#6P2500#7P1900#8P1427#9P1505#10P1280#11P1622#12P1505#13P1720T500");
      delay(500);
      Serial.println("#1P1427#2P1505#3P1100#4P2500#5P1505#6P2000#7P500#8P1427#9P1505#10P1280#11P2300#12P2500#13P1505T500");
      delay(500);
      Serial.println("#1P1427#2P1505#3P500#4P1100#5P1505#6P2500#7P1900#8P1427#9P1505#10P1280#11P1622#12P1505#13P1720T500");
      delay(500);
      Serial.println("#1P1427#2P1505#3P1100#4P2500#5P1505#6P2000#7P500#8P1427#9P1505#10P1280#11P2300#12P2500#13P1505T500");
      delay(500);
      Serial.println("#1P1427#2P1505#3P500#4P1100#5P1505#6P2500#7P1900#8P1427#9P1505#10P1280#11P1622#12P1505#13P1720T500");
      delay(500);
      Serial.println("#1P1427#2P1505#3P1573#4P1505#5P1505#6P1505#7P1480#8P1427#9P1505#10P1505#11P1622#12P1505#13P1505T500");
      delay(500);
    }

    if (resultData == "W") {
      //Serial.println("Walk");
      for (int i = 0; 1 < 3; i++){
        Serial.println("#1P1427#2P1505#3P1573#4P1505#5P1505#6P1505#7P1427#8P1232#9P1476#10P1280#11P1915#12P1817#13P1232T500");
        delay(500);
        Serial.println("#1P1134#2P1105#3P1573#4P1505#5P1505#6P1505#7P1427#8P1280#9P1476#10P1329#11P1915#12P1671#13P1378T500");
        delay(500);
        Serial.println("#1P1427#2P1505#3P1573#4P1505#5P1505#6P1505#7P1427#8P1280#9P1476#10P1768#11P1915#12P1671#13P1622T500");
        delay(500);
        Serial.println("#1P1720#2P1505#3P1573#4P1505#5P1905#6P1505#7P1427#8P890#9P1280#10P1505#11P1915#12P1671#13P1622T500");
        delay(500);
      }
      Serial.println("#1P1427#2P1505#3P1573#4P1505#5P1505#6P1550#7P1505#8P1427#9P1505#10P1505#11P1622#12P1505#13P1505T500D500");
      delay(500);
    }
    
    if (resultData == "S") {
      //Serial.println("STOP");
      Serial.println("#1P1427#2P1505#3P1573#4P1505#5P1505#6P1550#7P1505#8P1427#9P1505#10P1505#11P1622#12P1505#13P1505T500D500");
    }
  }
  /* ---------------------------------------- */
  
  if(res){
    return httpd_resp_send_500(req);
  }
 
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, NULL, 0);
}
/* ________________________________________________________________________________ */

/* ________________________________________________________________________________ Subroutine for starting the web server / startCameraServer. */
void startCameraWebServer(){
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 80;

  httpd_uri_t index_uri = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = index_handler,
    .user_ctx  = NULL
  };

  httpd_uri_t cmd_uri = {
    .uri       = "/action",
    .method    = HTTP_GET,
    .handler   = cmd_handler,
    .user_ctx  = NULL
  };

  httpd_uri_t stream_uri = {
    .uri       = "/stream",
    .method    = HTTP_GET,
    .handler   = stream_handler,
    .user_ctx  = NULL
  };
    
  if (httpd_start(&index_httpd, &config) == ESP_OK) {
      httpd_register_uri_handler(index_httpd, &index_uri);
      httpd_register_uri_handler(index_httpd, &cmd_uri);
  }

  config.server_port += 1;
  config.ctrl_port += 1;  
  if (httpd_start(&stream_httpd, &config) == ESP_OK) {
      httpd_register_uri_handler(stream_httpd, &stream_uri);
  }

  Serial.println();
  Serial.println("Camera Server started successfully");
  Serial.print("Camera Stream Ready! Go to: http://");
  Serial.println(WiFi.localIP());
  Serial.println();
}
/* ________________________________________________________________________________ */

/* ________________________________________________________________________________ String function to process the data command */
// I got this from : https://www.electroniclinic.com/reyax-lora-based-multiple-sensors-monitoring-using-arduino/
String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;
  
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
/* ________________________________________________________________________________ */

/* ________________________________________________________________________________ VOID SETUP() */
void setup() {
  // put your setup code here, to run once:

  // Disable brownout detector.
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  /* ---------------------------------------- Init serial communication speed (baud rate). */
  Serial.begin(115200);
  Serial.setDebugOutput(false);
  Serial.println();
  /* ---------------------------------------- */

  pinMode(LED_OnBoard, OUTPUT);
  pinMode(LED_Flash, OUTPUT);
  
  delay(2000);

  /* ---------------------------------------- Camera configuration. */
  Serial.println();
  Serial.println("------------");
  Serial.println("Start configuring and initializing the camera...");
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if(psramFound()){
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    ESP.restart();
  }
  
  Serial.println("Configure and initialize the camera successfully.");
  Serial.println("------------");
  Serial.println();
  /* ---------------------------------------- */

  /* ---------------------------------------- Waiting for WiFi device. */

  WiFi.softAP(ssid, password); //--> Creating Access Points
  WiFi.softAPConfig(local_ip, gateway, subnet);

  // Start camera web server
  startCameraWebServer();

  Serial.print("Device Ready to pair...http://");
  Serial.println(local_ip);
   
}
/* ________________________________________________________________________________ */

void loop() {
  // put your main code here, to run repeatedly:
  delay(1);
}