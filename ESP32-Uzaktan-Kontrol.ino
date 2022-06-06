#include "esp_camera.h"
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#define CAMERA_MODEL_AI_THINKER // Has PSRAM
#include "camera_pins.h"

const char* ssid = "VODAFONE_9D53";   // Wi-Fi Ağının İsmi
const char* password = "fc1f1fff";    // Wi-Fi Ağının Şifresi
unsigned long previousMillis2 = 0;

void startCameraServer();
char c = 0;
bool start = true;
unsigned long time_0 = 0;
unsigned long time_1 = 0;
unsigned long time_2 = 0;
unsigned long time_3 = 0;
unsigned long time_4 = 0;
unsigned long time_5 = 0;
String formattedDate;
String dayStamp;
String timeStamp;
bool flag1 = false;
bool flag2 = false;
bool flag_sayac = true;

WiFiServer wifiServer(10001);

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

void setup() {
  // Piston kontrolü için
  pinMode(12, OUTPUT); //YV0
  pinMode(13, OUTPUT); //YV1
  pinMode(14, OUTPUT); //YV2
  pinMode(15, OUTPUT); //YV3

  // Piston sensörleri için (S1, S2 anahtarları)
  pinMode(1, INPUT);  //S1
  digitalWrite(1, LOW);  //S1
  pinMode(3, INPUT);  //S2
  digitalWrite(3, LOW);  //S2
  pinMode(2, INPUT);  //S3
  digitalWrite(2, LOW);  //S3
  pinMode(4, INPUT); //S4
  digitalWrite(4, LOW);  //S4

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
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    //Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1); // flip it back
    s->set_brightness(s, 1); // up the brightness just a bit
    s->set_saturation(s, -2); // lower the saturation
  }
  // drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_QVGA);

#if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  startCameraServer();
  wifiServer.begin();

  // Initialize a NTPClient to get time
  timeClient.begin();
  timeClient.setTimeOffset(10800);
}

String timestamp(){
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  formattedDate = timeClient.getFormattedDate();

  // Extract date
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);time
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1) + " ";
  return timeStamp;
}

void loop() {
WiFiClient client = wifiServer.available();


  if (client) { // bağlı bir client varsa
 
    while (client.connected()) {  // bağlantı devam ettiği sürece

      if(client.available()>0) {  // gönderilen data olduğunda
        c = client.read();  // datayı oku.
        
        if(c == 'q') // ######################################## Deney 3. ########################################
        {
          digitalWrite(12, HIGH);
          digitalWrite(13, LOW);
          time_5 = millis();  // Sayacı başlat.
          flag_sayac = true;
          flag1 = true;
            while(true){
              
              if(flag1 == false && digitalRead(1) == HIGH)  //S1 anahtarı tetiklendiyse YV0 aktif
                {
                  digitalWrite(12, HIGH);
                  digitalWrite(13, LOW);
                  flag1 = true;
                  client.println(timestamp() + "S1 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV0 Aktif");
                  time_5 = millis();  // Sayacı başlat.
                  flag_sayac = true;
                }
              if(flag_sayac == true && millis() >= time_5 + 50)   //0.05 saniye sonra tüm çıkışları kapat.
                {
                  digitalWrite(12, LOW);
                  digitalWrite(13, LOW);
                  flag_sayac = false;
                }
              if(flag1 == true && digitalRead(3) == HIGH)   //S2 anahtarı tetiklendiyse YV1 aktif
                {
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  flag1 = false;
                  client.println(timestamp() + "S2 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV1 Aktif");
                  time_5 = millis();  // Sayacı başlat.
                  flag_sayac = true;
                }
              
              if(client.available()>0)
                {
                  c = client.read();  // datayı oku.
                }
              if(c == 'z')
                {
                  client.println(timestamp() + "Deney Durduruldu.");
                  digitalWrite(12, LOW);  //YV1 aktif
                  digitalWrite(13, HIGH);
                  delay(50);
                  digitalWrite(12, LOW);
                  digitalWrite(13, LOW);
                  break;
                }
            }
          }
        
        else if(c == 'Q') // ######################################## Deney 4. ########################################
        {
          digitalWrite(12, HIGH); //YV0 açık
          digitalWrite(13, LOW);
          //client.println(timestamp() + "Q");
          time_2 = millis();
          time_5 = millis();
          flag1 = true;
          flag_sayac = true;
            while(true){
              
              if(flag1 == false && digitalRead(1) == HIGH)  //S1 anahtarı tetiklendiyse YV0 aktif
                {
                  digitalWrite(12, HIGH);
                  digitalWrite(13, LOW);
                  time_5 = millis();
                  flag_sayac = true;
                  flag1 = true;
                  client.println(timestamp() + "S1 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV0 Aktif");
                }
              if(flag_sayac == true && millis() >= time_5 + 50)   //0.05 saniye sonra tüm çıkışları kapat.
                {
                  digitalWrite(12, LOW);
                  digitalWrite(13, LOW);
                  flag_sayac = false;
                }

              if(flag1 == true && digitalRead(3) == HIGH)   //S2 Anahtarı Aktif
                {
                  time_2 = millis();  // start timer
                  flag1 = false;
                  client.println(timestamp() + "S2 Sınır Anahtarı Tetiklendi.");
                }
              
              if(millis() >= time_2 + 1000)   //3 saniye bekledikten sonra dönüyor
                {
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  time_5 = millis();
                  flag_sayac = true;
                  time_2 = time_2 + 9999999;  //stop timer
                  client.println(timestamp() + "YV1 Aktif");
                }
                
              if(client.available()>0)
                {
                  c = client.read();  // datayı oku.
                }
              if(c == 'z')
                {
                  c = 0; //yeni data için sıfırlama.
                  client.println(timestamp() + "Deney Durduruldu.");
                  digitalWrite(12, LOW);  //YV1 aktif
                  digitalWrite(13, HIGH);
                  delay(50);
                  digitalWrite(12, LOW);
                  digitalWrite(13, LOW);
                  break;
                }
            }
        }

        else if(c == 'w') // ######################################## Deney 5. ########################################
        {
          digitalWrite(12, HIGH); //YV0 açık
          digitalWrite(13, LOW);
          time_5 = millis();  // Sayacı başlat.
          flag_sayac = true;
          flag1 = true;
          
            while(true)
            {
              if(flag1 == false && digitalRead(1) == HIGH)  // S1 Anahtarı Aktif
                {
                  time_1 = millis();  // start timer
                  flag1 = true;
                  //client.println(timestamp() + "S1 Sınır Anahtarı Tetiklendi.");
                }
              if(millis() >= time_1 + 4000)  //4 saniye bekledikten sonra YV0 aktif
                {
                  digitalWrite(12, HIGH);
                  digitalWrite(13, LOW);
                  
                  time_5 = millis();
                  flag_sayac = true;
                  time_1 = time_1 + 9999999;  // stop timer
                  //client.println(timestamp() + "YV0 Aktif.");
                }
              if(flag_sayac == true && millis() >= time_5 + 50)   //0.05 saniye sonra tüm çıkışları kapat.
                {
                  digitalWrite(12, LOW);
                  digitalWrite(13, LOW);
                  flag_sayac = false;
                }
              if(flag1 == true && digitalRead(3) == HIGH)   //S2 Anahtarı Aktif
                {
                  time_2 = millis();  // start timer
                  flag1 = false;
                  //client.println(timestamp() + "S2 Sınır Anahtarı Tetiklendi.");
                }
              if(millis() >= time_2 + 3000)   //3 saniye bekledikten sonra YV1 aktif
                {
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  time_5 = millis();
                  flag_sayac = true;
                  time_2 = time_2 + 9999999;  //stop timer
                  //client.println(timestamp() + "YV1 Aktif");
                }

              if(client.available()>0)
                {
                  c = client.read();  // datayı oku.
                }
              if(c == 'z')
                {
                  c = 0; //yeni data için sıfırlama.
                  //client.println(timestamp() + "Deney Durduruldu.");
                  break;
                }
            }
            digitalWrite(12, LOW);  //YV1 aktif
            digitalWrite(13, HIGH);
            delay(50);
            digitalWrite(12, LOW);
            digitalWrite(13, LOW);
        }

        else if(c == 'W') // ######################################## Deney 6. ########################################
        {
          int i = 0;
          flag1 = true;
          time_5 = millis();  // Sayacı başlat.
          flag_sayac = true;
          digitalWrite(12, HIGH); //YV0 Aktif.
          digitalWrite(13, LOW);
          while(i<3)
          {
            
            if(flag1 == false && digitalRead(1) == HIGH)  //S1 anahtarı tetiklendiyse YV0 aktif
                {
                  digitalWrite(12, HIGH);
                  digitalWrite(13, LOW);
                  time_5 = millis();
                  flag_sayac = true;
                  //client.println(timestamp() + "S1 Sınır Anahtarı Tetiklendi.");
                  //client.println(timestamp() + "YV0 Aktif");
                  flag1 = true;
                }
              if(flag_sayac == true && millis() >= time_5 + 50)   //0.05 saniye sonra tüm çıkışları kapat.
                {
                  digitalWrite(12, LOW);
                  digitalWrite(13, LOW);
                  flag_sayac = false;
                }
              if(flag1 == true && digitalRead(3) == HIGH)   //S2 anahtarı tetiklendiyse YV1 aktif
                {
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  time_5 = millis();
                  flag_sayac = true;
                  //client.println(timestamp() + "S2 Sınır Anahtarı Tetiklendi.");
                  //client.println(timestamp() + "YV1 Aktif");
                  i++;
                  flag1 = false;
                }

              if(client.available()>0)
                {
                  c = client.read();  // datayı oku.
                }
              if(c == 'z')
                {
                  c = 0; //yeni data için sıfırlama.
                  //client.println(timestamp() + "Deney Durduruldu.");
                  break;
                }
          }
            digitalWrite(12, LOW);  //YV1 aktif
            digitalWrite(13, HIGH);
            delay(50);
            digitalWrite(12, LOW);
            digitalWrite(13, LOW);
        }


        else if(c == 'e') // ######################################## Deney 7. ########################################
        {
          int i = 0;
          flag1 = true;
          digitalWrite(12, HIGH); //YV0 Aktif.
          digitalWrite(13, LOW);
          while(i<3)
          {
            
              if(flag1 == false && digitalRead(1) == HIGH)  //S1 anahtarı tetiklendiyse YV0 aktif
                {
                  digitalWrite(12, HIGH);
                  digitalWrite(13, LOW);
                  client.println(timestamp() + "S1 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV0 Aktif");
                  flag1 = true;
                }
              if(flag1 == true && digitalRead(3) == HIGH)   //S2 anahtarı tetiklendiyse YV1 aktif
                {
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  client.println(timestamp() + "S2 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV1 Aktif");
                  i++;
                  flag1 = false;
                }

              if(client.available()>0)
                {
                  c = client.read();  // datayı oku.
                }
              if(c == 'z')
                {
                  c = 0; //yeni data için sıfırlama.
                  client.println(timestamp() + "Deney Durduruldu.");
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  delay(5);
                  digitalWrite(13, LOW);
                  
                  digitalWrite(14, LOW);
                  digitalWrite(15, HIGH);
                  delay(5);
                  digitalWrite(15, LOW);
                  break;
                }
          }
          while(true){

              if(flag1 == false && digitalRead(1) == HIGH)  //S1 anahtarı tetiklendiyse YV0 aktif
                {
                  digitalWrite(12, HIGH);
                  digitalWrite(13, LOW);
                  flag1 = true;
                  client.println(timestamp() + "S1 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV0 Aktif");
                }

              if(flag1 == true && digitalRead(3) == HIGH)   //S2 Anahtarı Aktif
                {
                  time_2 = millis();  // start timer
                  flag1 = false;
                  client.println(timestamp() + "S2 Sınır Anahtarı Tetiklendi.");
                }
              
              if(millis() >= time_2 + 3000)   //YV1 Aktif, 3 saniye bekledikten sonra dönüyor
                {
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  time_2 = time_2 + 9999999;  //stop timer
                  client.println(timestamp() + "YV1 Aktif");
                }

              if(client.available()>0)
                {
                  c = client.read();  // datayı oku.
                }
              if(c == 'z')
                {
                  c = 0; //yeni data için sıfırlama.
                  client.println(timestamp() + "Deney Durduruldu.");
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  delay(5);
                  digitalWrite(13, LOW);
                  
                  digitalWrite(14, LOW);
                  digitalWrite(15, HIGH);
                  delay(5);
                  digitalWrite(15, LOW);
                  break;
                }
          }
            digitalWrite(12, LOW);  //YV1 aktif
            digitalWrite(13, HIGH);
        }

        else if(c == 'E') // ######################################## Deney 8. ########################################
        {
          digitalWrite(12, HIGH); //YV0 açık
          digitalWrite(13, LOW);
          flag1 = true;
          int i = 0;
            while(i<3)
            {
              if(flag1 == false && digitalRead(1) == HIGH)  // S1 Anahtarı Aktif
                {
                  time_1 = millis();  // start timer
                  flag1 = true;
                  client.println(timestamp() + "S1 Sınır Anahtarı Tetiklendi.");
                }
              if(millis() >= time_1 + 3000)  //3 saniye bekledikten sonra YV0 aktif
                {
                  digitalWrite(12, HIGH);
                  digitalWrite(13, LOW);
                  
                  time_1 = time_1 + 9999999;  // stop timer
                  client.println(timestamp() + "YV0 Aktif");
                }
              if(flag1 == true && digitalRead(3) == HIGH)   //S2 Anahtarı Aktif
                {
                  time_2 = millis();  // start timer
                  flag1 = false;
                  client.println(timestamp() + "S2 Sınır Anahtarı Tetiklendi.");
                }
              if(millis() >= time_2 + 4000)   //YV1 Aktif, 4 saniye bekledikten sonra YV1 aktif
                {
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  time_2 = time_2 + 9999999;  //stop timer
                  client.println(timestamp() + "YV1 Aktif");
                  i++;
                }

              if(client.available()>0)
                {
                  c = client.read();  // datayı oku.
                }
              if(c == 'z')
                {
                  c = 0; //yeni data için sıfırlama.
                  client.println(timestamp() + "Deney Durduruldu.");
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  delay(5);
                  digitalWrite(13, LOW);
                  
                  digitalWrite(14, LOW);
                  digitalWrite(15, HIGH);
                  delay(5);
                  digitalWrite(15, LOW);
                  break;
                }
            }
            digitalWrite(12, LOW);  //YV1 aktif
            digitalWrite(13, HIGH);
        }

        else if(c == 'r') // ######################################## Deney 9. ########################################
        {
          digitalWrite(12, HIGH); //YV0 aktif
          digitalWrite(13, LOW);
          digitalWrite(14, HIGH); //YV1 aktif
          digitalWrite(15, LOW);
          flag1 = true;
          flag2 = true;
            while(true){
              
              if(flag1 == false && digitalRead(1) == HIGH)  //S1 anahtarı tetiklendiyse YV0 aktif
                {
                  digitalWrite(12, HIGH); //YV0 aktif
                  digitalWrite(13, LOW);
                  flag1 = true;
                  client.println(timestamp() + "S1 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV0 Aktif");
                }
              if(flag1 == true && digitalRead(3) == HIGH)   //S2 anahtarı tetiklendiyse YV1 aktif
                {
                  digitalWrite(12, LOW);  //YV1 aktif
                  digitalWrite(13, HIGH);
                  flag1 = false;
                  client.println(timestamp() + "S2 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV1 Aktif");
                }
              if(flag2 == false && digitalRead(2) == HIGH)  //S3 anahtarı tetiklendiyse YV2 aktif
                {
                  digitalWrite(14, HIGH); //YV2 aktif
                  digitalWrite(15, LOW);
                  flag2 = true;
                  client.println(timestamp() + "S3 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV2 Aktif");
                }
              if(flag2 == true && digitalRead(4) == HIGH)   //S4 anahtarı tetiklendiyse YV3 aktif
                {
                  digitalWrite(14, LOW);  //YV3 aktif
                  digitalWrite(15, HIGH);
                  flag2 = false;
                  client.println(timestamp() + "S4 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV3 Aktif");
                }

              if(client.available()>0)
                {
                  c = client.read();  // datayı oku.
                }
              if(c == 'z')
                {
                  c = 0; //yeni data için sıfırlama.
                  client.println(timestamp() + "Deney Durduruldu.");
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  delay(5);
                  digitalWrite(13, LOW);
                  
                  digitalWrite(14, LOW);
                  digitalWrite(15, HIGH);
                  delay(5);
                  digitalWrite(15, LOW);
                  break;
                }
            }
            digitalWrite(12, LOW);  //YV1 aktif
            digitalWrite(13, HIGH);
            digitalWrite(14, LOW); //YV3 aktif
            digitalWrite(15, HIGH);
          }

        else if(c == 'R') // ######################################## Deney 10. ########################################
        {
          digitalWrite(12, HIGH); //YV0 aktif
          digitalWrite(13, LOW);
          digitalWrite(14, LOW); //YV3 aktif
          digitalWrite(15, HIGH);
          flag1 = false;
            while(true){
              if(flag1 == false && digitalRead(3) == HIGH && digitalRead(2) == HIGH)  //S2 ve S3 anahtarı tetiklendiyse YV0 aktif
                {
                  digitalWrite(12, LOW);  //YV1 aktif
                  digitalWrite(13, HIGH);
                  digitalWrite(14, HIGH); //YV2 aktif
                  digitalWrite(15, LOW);
                  flag1 = true;
                  client.println(timestamp() + "S2 ve S3 Sınır Anahtarları Tetiklendi.");
                  client.println(timestamp() + "YV1 ve YV2 aktif.");
                }
              if(flag1 == true && digitalRead(1) == HIGH && digitalRead(4) == HIGH)   //S1 ve S4 anahtarı tetiklendiyse YV1 aktif
                {
                  digitalWrite(12, HIGH); //YV0 aktif
                  digitalWrite(13, LOW);
                  digitalWrite(14, LOW); //YV3 aktif
                  digitalWrite(15, HIGH);
                  flag1 = false;
                  client.println(timestamp() + "S1 ve S4 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV0 ve YV3 Aktif.");
                }

              if(client.available()>0)
                {
                  c = client.read();  // datayı oku.
                }
              if(c == 'z')
                {
                  c = 0; //yeni data için sıfırlama.
                  client.println(timestamp() + "Deney Durduruldu.");
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  delay(5);
                  digitalWrite(13, LOW);
                  
                  digitalWrite(14, LOW);
                  digitalWrite(15, HIGH);
                  delay(5);
                  digitalWrite(15, LOW);
                  break;
                }
            }
            digitalWrite(12, LOW);  //YV1 aktif
            digitalWrite(13, HIGH);
            digitalWrite(14, LOW); //YV3 aktif
            digitalWrite(15, HIGH);
          }

        else if(c == 't') // ######################################## Deney 11. ########################################
        {
          digitalWrite(12, HIGH); //YV0 aktif
          digitalWrite(13, LOW);
          digitalWrite(14, HIGH); //YV2 aktif
          digitalWrite(15, LOW);
          flag1 = true;
          flag2 = true;
          
            while(true){
              // ----- 1. Piston ileri geri döngüsü yapıyor -----
              if(flag1 == false && digitalRead(1) == HIGH)  //S1 anahtarı tetiklendiyse YV0 aktif
                {
                  digitalWrite(12, HIGH); //YV0 aktif
                  digitalWrite(13, LOW);
                  flag1 = true;
                  client.println(timestamp() + "S1 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV0 Aktif.");
                }
              if(flag1 == true && digitalRead(3) == HIGH)   //S2 anahtarı tetiklendiyse YV1 aktif
                {
                  digitalWrite(12, LOW);  //YV1 aktif
                  digitalWrite(13, HIGH);
                  flag1 = false;
                  client.println(timestamp() + "S2 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV1 Aktif.");
                }
              
              // ----- 2. Piston ileri gidip 3sn bekleyip geri dönüyor -----
              if(flag2 == false && digitalRead(2) == HIGH)  //S3 anahtarı tetiklendiyse YV2 aktif
                {
                  digitalWrite(14, HIGH);
                  digitalWrite(15, LOW);
                  flag2 = true;
                  client.println(timestamp() + "S3 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV2 Aktif.");
                }

              if(flag2 == true && digitalRead(4) == HIGH)   //S4 Anahtarı Aktif
                {
                  time_2 = millis();  // start timer
                  flag2 = false;
                  client.println(timestamp() + "S4 Sınır Anahtarı Tetiklendi.");
                }
              
              if(millis() >= time_2 + 3000)   //3 saniye bekledikten sonra dönüyor
                {
                  digitalWrite(14, LOW);  //YV3 Aktif
                  digitalWrite(15, HIGH);
                  time_2 = time_2 + 9999999;  //stop timer
                  client.println(timestamp() + "YV3 Aktif.");
                }

              if(client.available()>0)
                {
                  c = client.read();  // datayı oku.
                }
              if(c == 'z')
                {
                  c = 0; //yeni data için sıfırlama.
                  client.println(timestamp() + "Deney Durduruldu.");
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  delay(5);
                  digitalWrite(13, LOW);
                  
                  digitalWrite(14, LOW);
                  digitalWrite(15, HIGH);
                  delay(5);
                  digitalWrite(15, LOW);
                  break;
                }
            }
            digitalWrite(12, LOW);  //YV1 aktif
            digitalWrite(13, HIGH);
            digitalWrite(14, LOW); //YV3 aktif
            digitalWrite(15, HIGH);
          }

        else if(c == 'T') // ######################################## Deney 12. ########################################
        {
          digitalWrite(12, HIGH); //YV0 aktif
          digitalWrite(13, LOW);
          digitalWrite(14, HIGH); //YV2 aktif
          digitalWrite(15, LOW);
          flag1 = true;
          flag2 = true;
          
            while(true){
              // ----- 1. Piston ileri geri döngüsü yapıyor -----
              if(flag1 == false && digitalRead(1) == HIGH)  //S1 anahtarı tetiklendiyse YV0 aktif
                {
                  digitalWrite(12, HIGH); //YV0 aktif
                  digitalWrite(13, LOW);
                  flag1 = true;
                  client.println(timestamp() + "S1 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV0 Aktif.");
                }
              if(flag1 == true && digitalRead(3) == HIGH)   //S2 anahtarı tetiklendiyse YV1 aktif
                {
                  digitalWrite(12, LOW);  //YV1 aktif
                  digitalWrite(13, HIGH);
                  flag1 = false;
                  client.println(timestamp() + "S2 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV1 Aktif.");
                }
              
              // ----- 2. Piston ileri gidip 3sn bekleyip geri dönüp 4 sn bekliyor -----
              
              if(flag2 == false && digitalRead(2) == HIGH)  // S3 Anahtarı Aktif
                {
                  time_1 = millis();  // start timer
                  flag2 = true;
                  client.println(timestamp() + "S3 Sınır Anahtarı Tetiklendi.");
                }
              if(millis() >= time_1 + 4000)  //4 saniye bekledikten sonra YV2 aktif
                {
                  digitalWrite(14, HIGH); // YV2 Aktif
                  digitalWrite(15, LOW);
                  
                  time_1 = time_1 + 9999999;  // stop timer
                  client.println(timestamp() + "YV2 Aktif.");
                }
              if(flag2 == true && digitalRead(4) == HIGH)   //S4 Anahtarı Aktif
                {
                  time_2 = millis();  // start timer
                  flag2 = false;
                  client.println(timestamp() + "S4 Sınır Anahtarı Tetiklendi.");
                }
              if(millis() >= time_2 + 3000)   //3 saniye bekledikten sonra YV3 aktif
                {
                  digitalWrite(14, LOW);  //YV3 Aktif
                  digitalWrite(15, HIGH);
                  time_2 = time_2 + 9999999;  //stop timer
                  client.println(timestamp() + "YV3 Aktif.");
                }

              if(client.available()>0)
                {
                  c = client.read();  // datayı oku.
                }
              if(c == 'z')
                {
                  c = 0; //yeni data için sıfırlama.
                  client.println(timestamp() + "Deney Durduruldu.");
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  delay(5);
                  digitalWrite(13, LOW);
                  
                  digitalWrite(14, LOW);
                  digitalWrite(15, HIGH);
                  delay(5);
                  digitalWrite(15, LOW);
                  break;
                }
              
            }
            digitalWrite(12, LOW);  //YV1 aktif
            digitalWrite(13, HIGH);
            digitalWrite(14, LOW); //YV3 aktif
            digitalWrite(15, HIGH);
          }

        else if(c == 'y') // ######################################## Deney 13. ########################################
        {
          digitalWrite(12, HIGH); //YV0 aktif
          digitalWrite(13, LOW);
          digitalWrite(14, HIGH); //YV2 aktif
          digitalWrite(15, LOW);
          flag1 = true;
          flag2 = true;
          
            while(true){
              // ----- 1. Piston ileri gidip 3 sn bekleyip geri dönüyor -----
              if(flag1 == false && digitalRead(1) == HIGH)  //S1 anahtarı tetiklendiyse YV0 aktif
                {
                  digitalWrite(12, HIGH);
                  digitalWrite(13, LOW);
                  flag1 = true;
                  client.println(timestamp() + "S1 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV0 Aktif.");
                }

              if(flag1 == true && digitalRead(3) == HIGH)   //S2 Anahtarı Aktif
                {
                  time_1 = millis();  // start timer
                  flag1 = false;
                  client.println(timestamp() + "S2 Sınır Anahtarı Tetiklendi.");
                }
              
              if(millis() >= time_1 + 3000)   //3 saniye bekledikten sonra dönüyor
                {
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  time_1 = time_1 + 9999999;  //stop timer
                  client.println(timestamp() + "YV1 Aktif.");
                }
              
              // ----- 2. Piston ileri gidip 2sn bekleyip geri dönüyor -----
              
              if(flag2 == false && digitalRead(2) == HIGH)  //S3 anahtarı tetiklendiyse YV2 aktif
                {
                  digitalWrite(14, HIGH); //YV2 aktif
                  digitalWrite(15, LOW);
                  flag2 = true;
                  client.println(timestamp() + "S3 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV2 Aktif.");
                }

              if(flag2 == true && digitalRead(4) == HIGH)   //S4 Anahtarı Aktif
                {
                  time_2 = millis();  // start timer
                  flag2 = false;
                  client.println(timestamp() + "S4 Sınır Anahtarı Tetiklendi.");
                }
              
              if(millis() >= time_2 + 2000)   //2 saniye bekledikten sonra dönüyor
                {
                  digitalWrite(14, LOW);  //YV3 aktif
                  digitalWrite(15, HIGH);
                  time_2 = time_2 + 9999999;  //stop timer
                  client.println(timestamp() + "YV3 Aktif.");
                }

              if(client.available()>0)
                {
                  c = client.read();  // datayı oku.
                }
              if(c == 'z')
                {
                  c = 0; //yeni data için sıfırlama.
                  client.println(timestamp() + "Deney Durduruldu.");
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  delay(5);
                  digitalWrite(13, LOW);
                  
                  digitalWrite(14, LOW);
                  digitalWrite(15, HIGH);
                  delay(5);
                  digitalWrite(15, LOW);
                  break;
                }
            }
            digitalWrite(12, LOW);  //YV1 aktif
            digitalWrite(13, HIGH);
            digitalWrite(14, LOW); //YV3 aktif
            digitalWrite(15, HIGH);
          }

        else if(c == 'Y') // ######################################## Deney 14. ########################################
        {
          digitalWrite(12, HIGH); //YV0 aktif, Piston1 ileri
          digitalWrite(13, LOW);
          digitalWrite(14, HIGH); //YV2 aktif, Piston2 ileri
          digitalWrite(15, LOW);
          flag1 = true;
          flag2 = false;
          
            while(true){
              // ----- 1. Piston ileri gidip 3 sn bekleyip geri dönüyor -----
              if(flag1 == false && digitalRead(1) == HIGH)  //S1 anahtarı tetiklendiyse YV0 aktif
                {
                  digitalWrite(12, HIGH);
                  digitalWrite(13, LOW);
                  flag1 = true;
                  client.println(timestamp() + "S1 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV0 Aktif.");
                }

              if(flag1 == true && digitalRead(3) == HIGH)   //S2 Anahtarı Aktif
                {
                  time_1 = millis();  // start timer
                  flag1 = false;
                  client.println(timestamp() + "S2 Sınır Anahtarı Tetiklendi.");
                }
              
              if(millis() >= time_1 + 3000)   //3 saniye bekledikten sonra dönüyor
                {
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  time_1 = time_1 + 9999999;  //stop timer
                  client.println(timestamp() + "YV1 Aktif.");
                }
              
              // ----- 2. Piston ileri gidip(-->) geri dönüp(<--), ileri gidip(-->) 2sn bekleyip geri dönüyor(<--) -----
              
              if(flag2 == false && digitalRead(4) == HIGH)  //S4 anahtarı tetiklendiyse YV3 aktif
                {
                  digitalWrite(14, LOW); //YV3 aktif
                  digitalWrite(15, HIGH);
                  flag2 = true;
                  client.println(timestamp() + "S4 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV3 Aktif.");
                }

              if(flag2 == true && digitalRead(2) == HIGH)   //S3 Anahtarı Aktif
                {
                  time_2 = millis();  // start timer
                  flag2 = false;
                  client.println(timestamp() + "S3 Sınır Anahtarı Tetiklendi.");
                }
              
              if(millis() >= time_2 + 3000)   //3 saniye bekledikten sonra ileri gidiyor
                {
                  digitalWrite(14, HIGH);  //YV2 aktif
                  digitalWrite(15, LOW);
                  time_2 = time_2 + 9999999;  //stop timer
                  client.println(timestamp() + "YV2 Aktif.");
                }

              if(client.available()>0)
                {
                  c = client.read();  // datayı oku.
                }
              if(c == 'z')
                {
                  c = 0; //yeni data için sıfırlama.
                  client.println(timestamp() + "Deney Durduruldu.");
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  delay(5);
                  digitalWrite(13, LOW);
                  
                  digitalWrite(14, LOW);
                  digitalWrite(15, HIGH);
                  delay(5);
                  digitalWrite(15, LOW);
                  break;
                }
            }
            digitalWrite(12, LOW);  //YV1 aktif
            digitalWrite(13, HIGH);
            digitalWrite(14, LOW); //YV3 aktif
            digitalWrite(15, HIGH);
          }

        else if(c == 'p') // ######################################## Deney 15. ########################################
        {
          digitalWrite(12, HIGH); //YV0 aktif, Piston1 ileri
          digitalWrite(13, LOW);
          digitalWrite(14, HIGH); //YV2 aktif, Piston2 ileri
          digitalWrite(15, LOW);
          flag1 = true;
          flag2 = true;
          
            while(true){
              // ----- 1. Piston ileri gidip 3 sn bekleyip geri dönüp 3 sn bekliyor -----
              if(flag1 == false && digitalRead(1) == HIGH)  // S1 Anahtarı Aktif
                {
                  time_1 = millis();  // start timer
                  client.println(timestamp() + "S1 Sınır Anahtarı Tetiklendi.");
                  flag1 = true;
                }
              if(millis() >= time_1 + 2000)  //2 saniye bekledikten sonra YV0 aktif
                {
                  digitalWrite(12, HIGH);
                  digitalWrite(13, LOW);
                  
                  time_1 = time_1 + 9999999;  // stop timer
                  client.println(timestamp() + "YV0 Aktif.");
                }
              if(flag1 == true && digitalRead(3) == HIGH)   //S2 Anahtarı Aktif
                {
                  time_2 = millis();  // start timer
                  client.println(timestamp() + "S2 Sınır Anahtarı Tetiklendi.");
                  flag1 = false;
                }
              if(millis() >= time_2 + 3000)   //3 saniye bekledikten sonra YV1 aktif
                {
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  time_2 = time_2 + 9999999;  //stop timer
                  client.println(timestamp() + "YV1 Aktif.");
                }
              
              // ----- 2. Piston ileri gidip 3 sn bekleyip geri dönüp 3 sn bekliyor -----
              if(flag2 == false && digitalRead(2) == HIGH)  // S3 Anahtarı Aktif
                {
                  time_3 = millis();  // start timer
                  client.println(timestamp() + "S3 Sınır Anahtarı Tetiklendi.");
                  flag2 = true;
                }
              if(millis() >= time_3 + 3000)  //3 saniye bekledikten sonra YV0 aktif
                {
                  digitalWrite(14, HIGH);
                  digitalWrite(15, LOW);
                  
                  time_3 = time_3 + 9999999;  // stop timer
                  client.println(timestamp() + "YV3 Aktif.");
                }
              if(flag2 == true && digitalRead(4) == HIGH)   //S4 Anahtarı Aktif
                {
                  time_4 = millis();  // start timer
                  client.println(timestamp() + "S4 Sınır Anahtarı Tetiklendi.");
                  flag2 = false;
                }
              if(millis() >= time_4 + 2000)   //2 saniye bekledikten sonra YV1 aktif
                {
                  digitalWrite(14, LOW);
                  digitalWrite(15, HIGH);
                  time_4 = time_4 + 9999999;  //stop timer
                  client.println(timestamp() + "YV4 Aktif.");
                }    

              if(client.available()>0)
                {
                  c = client.read();  // datayı oku.
                }
              if(c == 'z')
                {
                  c = 0; //yeni data için sıfırlama.
                  client.println(timestamp() + "Deney Durduruldu.");
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  delay(5);
                  digitalWrite(13, LOW);
                  
                  digitalWrite(14, LOW);
                  digitalWrite(15, HIGH);
                  delay(5);
                  digitalWrite(15, LOW);
                  break;
                }
            }
            digitalWrite(12, LOW);  //YV1 aktif
            digitalWrite(13, HIGH);
            digitalWrite(14, LOW); //YV3 aktif
            digitalWrite(15, HIGH);
          }


        else if(c == 'P') // ######################################## Deney 16. ########################################
        {
          digitalWrite(12, HIGH); //YV0 açık
          digitalWrite(13, LOW);
          digitalWrite(14, HIGH); //YV2 açık
          digitalWrite(15, LOW);
          flag1 = true;
          flag2 = true;
          int i = 0;
          int j = 0;
            while(i<3 || j<3)
            {
              // 1. Piston 3 defa ileri geri döngüsünü yapıyor
              if(flag1 == false && digitalRead(1) == HIGH && i<3)  //S1 anahtarı tetiklendiyse YV0 aktif
                {
                  digitalWrite(12, HIGH);
                  digitalWrite(13, LOW);
                  client.println(timestamp() + "S1 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV0 Aktif.");
                  flag1 = true;
                }
              if(flag1 == true && digitalRead(3) == HIGH && i<3)   //S2 anahtarı tetiklendiyse YV1 aktif
                {
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  client.println(timestamp() + "S2 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV1 Aktif.");
                  i++;
                  flag1 = false;
                }

              // 2. Piston 3 defa ileri geri döngüsünü yapıyor
              if(flag2 == false && digitalRead(2) == HIGH && j<3)  //S3 anahtarı tetiklendiyse YV2 aktif
                {
                  digitalWrite(14, HIGH);
                  digitalWrite(15, LOW);
                  client.println(timestamp() + "S3 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV2 Aktif.");
                  flag2 = true;
                }
              if(flag2 == true && digitalRead(4) == HIGH && j<3)   //S4 anahtarı tetiklendiyse YV3 aktif
                {
                  digitalWrite(14, LOW);
                  digitalWrite(15, HIGH);
                  client.println(timestamp() + "S4 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV3 Aktif.");
                  j++;
                  flag2 = false;
                }

              if(client.available()>0)
                {
                  c = client.read();  // datayı oku.
                }
              if(c == 'z')
                {
                  c = 0; //yeni data için sıfırlama.
                  client.println(timestamp() + "Deney Durduruldu.");
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  delay(5);
                  digitalWrite(13, LOW);
                  
                  digitalWrite(14, LOW);
                  digitalWrite(15, HIGH);
                  delay(5);
                  digitalWrite(15, LOW);
                  break;
                }
            }
            digitalWrite(12, LOW);  //YV1 aktif
            digitalWrite(13, HIGH);
            digitalWrite(14, LOW);  //YV3 aktif
            digitalWrite(15, HIGH);
        }

        else if(c == 'a') // ######################################## Deney 17. ########################################
        {
          digitalWrite(12, HIGH); //YV0 açık
          digitalWrite(13, LOW);
          digitalWrite(14, HIGH); //YV2 açık
          digitalWrite(15, LOW);
          flag1 = true;
          flag2 = true;
          int j = 0;
            while(true){
              // 1. Piston ileri geri döngüsünü yapıyor.
              if(flag1 == false && digitalRead(1) == HIGH)  //S1 anahtarı tetiklendiyse YV0 aktif
                {
                  digitalWrite(12, HIGH);
                  digitalWrite(13, LOW);
                  flag1 = true;
                  client.println(timestamp() + "S1 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV0 Aktif.");
                }
              if(flag1 == true && digitalRead(3) == HIGH)   //S2 anahtarı tetiklendiyse YV1 aktif
                {
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  flag1 = false;
                  client.println(timestamp() + "S2 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV1 Aktif.");
                }

              // 2. Piston sadece 3 defa ileri geri döngüsünü yapıyor.
              if(flag2 == false && digitalRead(2) == HIGH && j<3)  //S3 anahtarı tetiklendiyse YV2 aktif
                {
                  digitalWrite(14, HIGH);
                  digitalWrite(15, LOW);
                  flag2 = true;
                  client.println(timestamp() + "S3 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV2 Aktif.");
                }
              if(flag2 == true && digitalRead(4) == HIGH)   //S4 anahtarı tetiklendiyse YV3 aktif
                {
                  digitalWrite(14, LOW);
                  digitalWrite(15, HIGH);
                  flag2 = false;
                  j++;
                  client.println(timestamp() + "S4 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV3 Aktif.");
                }    

              if(client.available()>0)
                {
                  c = client.read();  // datayı oku.
                }
              if(c == 'z')
                {
                  c = 0; //yeni data için sıfırlama.
                  client.println(timestamp() + "Deney Durduruldu.");
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  delay(5);
                  digitalWrite(13, LOW);
                  
                  digitalWrite(14, LOW);
                  digitalWrite(15, HIGH);
                  delay(5);
                  digitalWrite(15, LOW);
                  break;
                }
            }
            digitalWrite(12, LOW);  //YV1 aktif
            digitalWrite(13, HIGH);
            digitalWrite(14, LOW);  //YV3 aktif
            digitalWrite(15, HIGH);
        }

        else if(c == 'A') // ######################################## Deney 18. ########################################
        {
          digitalWrite(12, HIGH); //YV0 açık
          digitalWrite(13, LOW);
          digitalWrite(14, HIGH); //YV2 açık
          digitalWrite(15, LOW);
          flag1 = true;
          flag2 = true;
          int j = 0;
            while(true){
              // 1. Piston ileri geri döngüsünü yapıyor, 2.Piston 3 defa ileri geri yaptığında 1.Piston duruyor.
              if(flag1 == false && digitalRead(1) == HIGH && j<3)  //S1 anahtarı tetiklendiyse YV0 aktif
                {
                  digitalWrite(12, HIGH);
                  digitalWrite(13, LOW);
                  flag1 = true;
                  client.println(timestamp() + "S1 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV0 Aktif.");
                }
              if(flag1 == true && digitalRead(3) == HIGH && j<3)   //S2 anahtarı tetiklendiyse YV1 aktif
                {
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  flag1 = false;
                  client.println(timestamp() + "S2 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV1 Aktif.");
                }

              // 2. Piston ileri geri döngüsünü yapıyor.
              if(flag2 == false && digitalRead(2) == HIGH)  //S3 anahtarı tetiklendiyse YV2 aktif
                {
                  digitalWrite(14, HIGH);
                  digitalWrite(15, LOW);
                  flag2 = true;
                  client.println(timestamp() + "S3 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV2 Aktif.");
                }
              if(flag2 == true && digitalRead(4) == HIGH)   //S4 anahtarı tetiklendiyse YV3 aktif
                {
                  digitalWrite(14, LOW);
                  digitalWrite(15, HIGH);
                  flag2 = false;
                  j++;
                  client.println(timestamp() + "S4 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV3 Aktif.");
                } 

             if(client.available()>0)
                {
                  c = client.read();  // datayı oku.
                }
              if(c == 'z')
                {
                  c = 0; //yeni data için sıfırlama.
                  client.println(timestamp() + "Deney Durduruldu.");
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  delay(5);
                  digitalWrite(13, LOW);
                  
                  digitalWrite(14, LOW);
                  digitalWrite(15, HIGH);
                  delay(5);
                  digitalWrite(15, LOW);
                  break;
                }
            }
            digitalWrite(12, LOW);  //YV1 aktif
            digitalWrite(13, HIGH);
            digitalWrite(14, LOW);  //YV3 aktif
            digitalWrite(15, HIGH);
        }

        else if(c == 's') // ######################################## Deney 19. ########################################
        {
          digitalWrite(12, HIGH); //YV0 açık
          digitalWrite(13, LOW);
          digitalWrite(14, HIGH); //YV2 açık
          digitalWrite(15, LOW);
          flag1 = true;
          flag2 = true;
          int i = 0;
          int j = 0;
            while(i<2 || j<3)
            {
              // 1. Piston 2 defa ileri geri döngüsünü yapıyor
              if(flag1 == false && digitalRead(1) == HIGH && i<2)  //S1 anahtarı tetiklendiyse YV0 aktif
                {
                  digitalWrite(12, HIGH);
                  digitalWrite(13, LOW);
                  client.println(timestamp() + "S1 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV0 Aktif.");
                  flag1 = true;
                }
              if(flag1 == true && digitalRead(3) == HIGH && i<2)   //S2 anahtarı tetiklendiyse YV1 aktif
                {
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  client.println(timestamp() + "S2 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV1 Aktif.");
                  i++;
                  flag1 = false;
                }

              // 2. Piston 3 defa ileri geri döngüsünü yapıyor
              if(flag2 == false && digitalRead(2) == HIGH && j<3)  //S3 anahtarı tetiklendiyse YV2 aktif
                {
                  digitalWrite(14, HIGH);
                  digitalWrite(15, LOW);
                  client.println(timestamp() + "S3 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV2 Aktif.");
                  flag2 = true;
                }
              if(flag2 == true && digitalRead(4) == HIGH && j<3)   //S4 anahtarı tetiklendiyse YV3 aktif
                {
                  digitalWrite(14, LOW);
                  digitalWrite(15, HIGH);
                  client.println(timestamp() + "S4 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV3 Aktif.");
                  j++;
                  flag2 = false;
                }

              if(client.available()>0)
                {
                  c = client.read();  // datayı oku.
                }
              if(c == 'z')
                {
                  c = 0; //yeni data için sıfırlama.
                  client.println(timestamp() + "Deney Durduruldu.");
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  delay(5);
                  digitalWrite(13, LOW);
                  
                  digitalWrite(14, LOW);
                  digitalWrite(15, HIGH);
                  delay(5);
                  digitalWrite(15, LOW);
                  break;
                }
            }
            digitalWrite(12, LOW);  //YV1 aktif
            digitalWrite(13, HIGH);
            digitalWrite(14, LOW);  //YV3 aktif
            digitalWrite(15, HIGH);
        }

        else if(c == 'S') // ######################################## Deney 20. ########################################
        {
          digitalWrite(12, HIGH); //YV0 açık
          digitalWrite(13, LOW);
          digitalWrite(14, HIGH); //YV2 açık
          digitalWrite(15, LOW);
          flag1 = true;
          flag2 = true;
          int j = 0;
            while(true){
              // 1. Piston ileri geri döngüsünü yapıyor.
              if(flag1 == false && digitalRead(1) == HIGH)  //S1 anahtarı tetiklendiyse YV0 aktif
                {
                  digitalWrite(12, HIGH);
                  digitalWrite(13, LOW);
                  flag1 = true;
                  client.println(timestamp() + "S1 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV0 Aktif.");
                }
              if(flag1 == true && digitalRead(3) == HIGH)   //S2 anahtarı tetiklendiyse YV1 aktif
                {
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  flag1 = false;
                  client.println(timestamp() + "S2 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV1 Aktif.");
                }

              // 2. Piston 3 defa ileri gidip 3 sn bekleyip geri dönüyor.
              if(flag2 == false && digitalRead(2) == HIGH && j<3)  //S3 anahtarı tetiklendiyse YV2 aktif
                {
                  digitalWrite(14, HIGH); //YV2 aktif
                  digitalWrite(15, LOW);
                  flag2 = true;
                  client.println(timestamp() + "S3 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV2 Aktif.");
                }

              if(flag2 == true && digitalRead(4) == HIGH && j<3)   //S4 Anahtarı Aktif
                {
                  time_2 = millis();  // start timer
                  flag2 = false;
                  client.println(timestamp() + "S4 Sınır Anahtarı Tetiklendi.");
                }
              
              if(millis() >= time_2 + 3000 && j<3)   //3 saniye bekledikten sonra dönüyor
                {
                  digitalWrite(14, LOW);  //YV3 aktif
                  digitalWrite(15, HIGH);
                  j++;
                  time_2 = time_2 + 9999999;  //stop timer
                  client.println(timestamp() + "YV3 Aktif.");
                }

              if(client.available()>0)
                {
                  c = client.read();  // datayı oku.
                }
              if(c == 'z')
                {
                  c = 0; //yeni data için sıfırlama.
                  client.println(timestamp() + "Deney Durduruldu.");
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  delay(5);
                  digitalWrite(13, LOW);
                  
                  digitalWrite(14, LOW);
                  digitalWrite(15, HIGH);
                  delay(5);
                  digitalWrite(15, LOW);
                  break;
                }
            }
            digitalWrite(12, LOW);  //YV1 aktif
            digitalWrite(13, HIGH);
            digitalWrite(14, LOW);  //YV3 aktif
            digitalWrite(15, HIGH);
        }

        else if(c == 'd') // ######################################## Deney 21. ########################################
        {
          digitalWrite(12, HIGH); //YV0 açık
          digitalWrite(13, LOW);
          digitalWrite(14, HIGH); //YV2 açık
          digitalWrite(15, LOW);
          flag1 = true;
          flag2 = true;
          int j = 0;
            while(true){
              // 1. Piston ileri geri döngüsünü yapıyor.
              if(flag1 == false && digitalRead(1) == HIGH)  //S1 anahtarı tetiklendiyse YV0 aktif
                {
                  digitalWrite(12, HIGH);
                  digitalWrite(13, LOW);
                  flag1 = true;
                  client.println(timestamp() + "S1 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV0 Aktif.");
                }
              if(flag1 == true && digitalRead(3) == HIGH)   //S2 anahtarı tetiklendiyse YV1 aktif
                {
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  flag1 = false;
                  client.println(timestamp() + "S2 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV1 Aktif.");
                }

              // 2. Piston 3 defa ileri 3sn bekliyor geri dönüp 2sn bekliyor
              if(flag2 == false && digitalRead(2) == HIGH && j<3)  // S3 Anahtarı Aktif
                {
                  time_3 = millis();  // start timer
                  flag2 = true;
                  client.println(timestamp() + "S3 Sınır Anahtarı Tetiklendi.");
                }
              if(millis() >= time_3 + 2000 && j<3)  //3 saniye bekledikten sonra YV2 aktif
                {
                  digitalWrite(14, HIGH);
                  digitalWrite(15, LOW);
                  
                  time_3 = time_3 + 9999999;  // stop timer
                  client.println(timestamp() + "YV2 Aktif.");
                }
              if(flag2 == true && digitalRead(4) == HIGH && j<3)   //S4 Anahtarı Aktif
                {
                  time_4 = millis();  // start timer
                  flag2 = false;
                  client.println(timestamp() + "S4 Sınır Anahtarı Tetiklendi.");
                }
              if(millis() >= time_4 + 3000 && j<3)   //2 saniye bekledikten sonra YV3 aktif
                {
                  digitalWrite(14, LOW);
                  digitalWrite(15, HIGH);
                  time_4 = time_4 + 9999999;  //stop timer
                  j++;
                  client.println(timestamp() + "YV3 Aktif.");
                }

              if(client.available()>0)
                {
                  c = client.read();  // datayı oku.
                }
              if(c == 'z')
                {
                  c = 0; //yeni data için sıfırlama.
                  client.println(timestamp() + "Deney Durduruldu.");
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  delay(5);
                  digitalWrite(13, LOW);
                  
                  digitalWrite(14, LOW);
                  digitalWrite(15, HIGH);
                  delay(5);
                  digitalWrite(15, LOW);
                  break;
                }
            }
            digitalWrite(12, LOW);  //YV1 aktif
            digitalWrite(13, HIGH);
            digitalWrite(14, LOW);  //YV3 aktif
            digitalWrite(15, HIGH);
        }

        else if(c == 'D') // ######################################## Deney 22. ########################################
        {
          digitalWrite(12, HIGH); //YV0 açık
          digitalWrite(13, LOW);
          digitalWrite(14, HIGH); //YV2 açık
          digitalWrite(15, LOW);
          flag1 = true;
          flag2 = true;
          int i = 0;
          int j = 0;
            while(i<3 || j<2){
              // 1. Piston 3 defa ileri gidip 3sn bekleyip dönüyor.
              if(flag1 == false && digitalRead(1) == HIGH && i<3)  //S1 anahtarı tetiklendiyse YV0 aktif
                {
                  digitalWrite(12, HIGH);
                  digitalWrite(13, LOW);
                  flag1 = true;
                  client.println(timestamp() + "S1 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV0 Aktif.");
                }

              if(flag1 == true && digitalRead(3) == HIGH && i<3)   //S2 Anahtarı Aktif
                {
                  time_2 = millis();  // start timer
                  flag1 = false;
                  client.println(timestamp() + "S2 Sınır Anahtarı Tetiklendi.");
                }
              
              if(millis() >= time_2 + 3000 && i<3)   //3 saniye bekledikten sonra dönüyor
                {
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  i++;
                  time_2 = time_2 + 9999999;  //stop timer
                  client.println(timestamp() + "YV1 Aktif.");
                }

              // 2. Piston 2 defa ileri gidip 2 sn bekleyip geri dönüyor.
              if(flag2 == false && digitalRead(2) == HIGH && j<2)  //S3 anahtarı tetiklendiyse YV2 aktif
                {
                  digitalWrite(14, HIGH); //YV2 aktif
                  digitalWrite(15, LOW);
                  flag2 = true;
                  client.println(timestamp() + "S3 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV2 Aktif.");
                }

              if(flag2 == true && digitalRead(4) == HIGH && j<2)   //S4 Anahtarı Aktif
                {
                  time_4 = millis();  // start timer
                  flag2 = false;
                  client.println(timestamp() + "S4 Sınır Anahtarı Tetiklendi.");
                }
              
              if(millis() >= time_4 + 2000 && j<2)   //3 saniye bekledikten sonra dönüyor
                {
                  digitalWrite(14, LOW);  //YV3 aktif
                  digitalWrite(15, HIGH);
                  j++;
                  time_4 = time_4 + 9999999;  //stop timer
                  client.println(timestamp() + "YV3 Aktif.");
                }

              if(client.available()>0)
                {
                  c = client.read();  // datayı oku.
                }
              if(c == 'z')
                {
                  c = 0; //yeni data için sıfırlama.
                  client.println(timestamp() + "Deney Durduruldu.");
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  delay(5);
                  digitalWrite(13, LOW);
                  
                  digitalWrite(14, LOW);
                  digitalWrite(15, HIGH);
                  delay(5);
                  digitalWrite(15, LOW);
                  break;
                }
            }
            digitalWrite(12, LOW);  //YV1 aktif
            digitalWrite(13, HIGH);
            digitalWrite(14, LOW);  //YV3 aktif
            digitalWrite(15, HIGH);
        }
     

          else if(c == 'f') // ######################################## Deney 23. ########################################
        {
          digitalWrite(12, HIGH); //YV0 aktif, Piston1 ileri
          digitalWrite(13, LOW);
          digitalWrite(14, HIGH); //YV2 aktif, Piston2 ileri
          digitalWrite(15, LOW);
          flag1 = true;
          flag2 = true;
          int i = 0;
          int j = 0;
          
            while(i<3 || j<3){
              // ----- 1. Piston ileri gidip 3 sn bekleyip geri dönüp 2 sn bekliyor -----
              if(flag1 == false && digitalRead(1) == HIGH && i<3)  // S1 Anahtarı Aktif
                {
                  time_1 = millis();  // start timer
                  flag1 = true;
                  client.println(timestamp() + "S1 Sınır Anahtarı Tetiklendi.");
                }
              if(millis() >= time_1 + 2000 && i<3)  //2 saniye bekledikten sonra YV0 aktif
                {
                  digitalWrite(12, HIGH);
                  digitalWrite(13, LOW);
                  
                  time_1 = time_1 + 9999999;  // stop timer
                  client.println(timestamp() + "YV0 Aktif.");
                }
              if(flag1 == true && digitalRead(3) == HIGH && i<3)   //S2 Anahtarı Aktif
                {
                  time_2 = millis();  // start timer
                  flag1 = false;
                  client.println(timestamp() + "S2 Sınır Anahtarı Tetiklendi.");
                }
              if(millis() >= time_2 + 3000 && i<3)   //3 saniye bekledikten sonra YV1 aktif
                {
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  time_2 = time_2 + 9999999;  //stop timer
                  i++;
                  client.println(timestamp() + "YV1 Aktif.");
                }
              
              // ----- 2. Piston ileri gidip 2 sn bekleyip geri dönüp 3 sn bekliyor -----
              if(flag2 == false && digitalRead(2) == HIGH && j<3)  // S3 Anahtarı Aktif
                {
                  time_3 = millis();  // start timer
                  flag2 = true;
                  client.println(timestamp() + "S3 Sınır Anahtarı Tetiklendi.");
                }
              if(millis() >= time_3 + 3000 && j<3)  //2 saniye bekledikten sonra YV0 aktif
                {
                  digitalWrite(14, HIGH);
                  digitalWrite(15, LOW);
                  
                  time_3 = time_3 + 9999999;  // stop timer
                  client.println(timestamp() + "YV0 Aktif.");
                }
              if(flag2 == true && digitalRead(4) == HIGH && j<3)   //S4 Anahtarı Aktif
                {
                  time_4 = millis();  // start timer
                  flag2 = false;
                  client.println(timestamp() + "S4 Sınır Anahtarı Tetiklendi.");
                }
              if(millis() >= time_4 + 2000 && j<3)   //4 saniye bekledikten sonra YV1 aktif
                {
                  digitalWrite(14, LOW);
                  digitalWrite(15, HIGH);
                  time_4 = time_4 + 9999999;  //stop timer
                  j++;
                  client.println(timestamp() + "YV3 Aktif.");
                }

              if(client.available()>0)
                {
                  c = client.read();  // datayı oku.
                }
              if(c == 'z')
                {
                  c = 0; //yeni data için sıfırlama.
                  client.println(timestamp() + "Deney Durduruldu.");
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  delay(5);
                  digitalWrite(13, LOW);
                  
                  digitalWrite(14, LOW);
                  digitalWrite(15, HIGH);
                  delay(5);
                  digitalWrite(15, LOW);
                  break;
                }
            }
            digitalWrite(12, LOW);  //YV1 aktif
            digitalWrite(13, HIGH);
            digitalWrite(14, LOW); //YV3 aktif
            digitalWrite(15, HIGH);
          }

          else if(c == 'F') // ######################################## Deney 24. ########################################
        {
          digitalWrite(12, HIGH); //YV0 aktif, Piston1 ileri
          digitalWrite(13, LOW);
          digitalWrite(14, HIGH); //YV2 aktif, Piston2 ileri
          digitalWrite(15, LOW);
          flag1 = true;
          flag2 = true;
          int i = 0;
          int j = 0;
          
            while(i<3 || j<2){
              // ----- 1. Piston 3 defa ileri gidip 3 sn bekleyip geri dönüp 2 sn bekliyor -----
              if(flag1 == false && digitalRead(1) == HIGH && i<3)  // S1 Anahtarı Aktif
                {
                  time_1 = millis();  // start timer
                  flag1 = true;
                  client.println(timestamp() + "S1 Sınır Anahtarı Tetiklendi.");
                }
              if(millis() >= time_1 + 2000 && i<3)  //3 saniye bekledikten sonra YV0 aktif
                {
                  digitalWrite(12, HIGH);
                  digitalWrite(13, LOW);
                  
                  time_1 = time_1 + 9999999;  // stop timer
                  client.println(timestamp() + "YV0 Aktif.");
                }
              if(flag1 == true && digitalRead(3) == HIGH && i<3)   //S2 Anahtarı Aktif
                {
                  time_2 = millis();  // start timer
                  flag1 = false;
                  client.println(timestamp() + "S2 Sınır Anahtarı Tetiklendi.");
                }
              if(millis() >= time_2 + 3000 && i<3)   //2 saniye bekledikten sonra YV1 aktif
                {
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  time_2 = time_2 + 9999999;  //stop timer
                  i++;
                  client.println(timestamp() + "YV1 Aktif.");
                }
              
              // ----- 2. Piston 2 defa ileri gidip 2 sn bekleyip geri dönüp 3 sn bekliyor -----
              if(flag2 == false && digitalRead(2) == HIGH && j<2)  // S3 Anahtarı Aktif
                {
                  time_3 = millis();  // start timer
                  flag2 = true;
                  client.println(timestamp() + "S3 Sınır Anahtarı Tetiklendi.");
                }
              if(millis() >= time_3 + 3000 && j<2)  //2 saniye bekledikten sonra YV2 aktif
                {
                  digitalWrite(14, HIGH);
                  digitalWrite(15, LOW);
                  
                  time_3 = time_3 + 9999999;  // stop timer
                  client.println(timestamp() + "YV2 Aktif.");
                }
              if(flag2 == true && digitalRead(4) == HIGH && j<2)   //S4 Anahtarı Aktif
                {
                  time_4 = millis();  // start timer
                  flag2 = false;
                  client.println(timestamp() + "S4 Sınır Anahtarı Tetiklendi.");
                }
              if(millis() >= time_4 + 2000 && j<2)   //4 saniye bekledikten sonra YV3 aktif
                {
                  digitalWrite(14, LOW);
                  digitalWrite(15, HIGH);
                  time_4 = time_4 + 9999999;  //stop timer
                  j++;
                  client.println(timestamp() + "YV3 Aktif.");
                }

              if(client.available()>0)
                {
                  c = client.read();  // datayı oku.
                }
              if(c == 'z')
                {
                  c = 0; //yeni data için sıfırlama.
                  client.println(timestamp() + "Deney Durduruldu.");
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  delay(5);
                  digitalWrite(13, LOW);
                  
                  digitalWrite(14, LOW);
                  digitalWrite(15, HIGH);
                  delay(5);
                  digitalWrite(15, LOW);
                  break;
                }
            }
            digitalWrite(12, LOW);  //YV1 aktif
            digitalWrite(13, HIGH);
            digitalWrite(14, LOW); //YV3 aktif
            digitalWrite(15, HIGH);
          }

          

        else if(c == 'g') // ######################################## Deney 25. ########################################
        {
          digitalWrite(12, HIGH); //YV0 açık
          digitalWrite(13, LOW);
          digitalWrite(14, HIGH); //YV2 açık
          digitalWrite(15, LOW);
          flag1 = true;
          flag2 = true;
          int i = 0;
          int j = 0;
            while(i<3 || j<3)
            {
              // 1. Piston 3 defa ileri geri döngüsünü yapıyor
              if(flag1 == false && digitalRead(1) == HIGH && i<3)  //S1 anahtarı tetiklendiyse YV0 aktif
                {
                  digitalWrite(12, HIGH);
                  digitalWrite(13, LOW);
                  client.println(timestamp() + "S1 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV0 Aktif.");
                  flag1 = true;
                }
              if(flag1 == true && digitalRead(3) == HIGH && i<3)   //S2 anahtarı tetiklendiyse YV1 aktif
                {
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  client.println(timestamp() + "S1 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV1 Aktif.");
                  i++;
                  flag1 = false;
                }

              // 2. Piston 3 defa ileri geri döngüsünü yapıyor
              if(flag2 == false && digitalRead(2) == HIGH && j<3)  //S3 anahtarı tetiklendiyse YV2 aktif
                {
                  digitalWrite(14, HIGH);
                  digitalWrite(15, LOW);
                  client.println(timestamp() + "S3 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV2 Aktif.");
                  flag2 = true;
                }
              if(flag2 == true && digitalRead(4) == HIGH && j<3)   //S4 anahtarı tetiklendiyse YV3 aktif
                {
                  digitalWrite(14, LOW);
                  digitalWrite(15, HIGH);
                  client.println(timestamp() + "S4 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV3 Aktif.");
                  j++;
                  flag2 = false;
                }

              if(client.available()>0)
                {
                  c = client.read();  // datayı oku.
                }
              if(c == 'z')
                {
                  c = 0; //yeni data için sıfırlama.
                  client.println(timestamp() + "Deney Durduruldu.");
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  delay(5);
                  digitalWrite(13, LOW);
                  
                  digitalWrite(14, LOW);
                  digitalWrite(15, HIGH);
                  delay(5);
                  digitalWrite(15, LOW);
                  break;
                }
            }
            flag1 = true;
            flag2 = true;
            i = 0;
            i = 0;
            while(true){
              // ----- 1. Piston ileri gidip 3 sn bekleyip geri dönüyor -----
              if(flag1 == false && digitalRead(1) == HIGH)  //S1 anahtarı tetiklendiyse YV0 aktif
                {
                  digitalWrite(12, HIGH);
                  digitalWrite(13, LOW);
                  flag1 = true;
                  client.println(timestamp() + "S1 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV0 Aktif.");
                }

              if(flag1 == true && digitalRead(3) == HIGH)   //S2 Anahtarı Aktif
                {
                  time_1 = millis();  // start timer
                  flag1 = false;
                  client.println(timestamp() + "S1 Sınır Anahtarı Tetiklendi.");
                }
              
              if(millis() >= time_1 + 3000)   //3 saniye bekledikten sonra dönüyor
                {
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  time_1 = time_1 + 9999999;  //stop timer
                  client.println(timestamp() + "YV1 Aktif.");
                }
              
              // ----- 2. Piston ileri gidip 3sn bekleyip geri dönüyor -----
              
              if(flag2 == false && digitalRead(2) == HIGH)  //S3 anahtarı tetiklendiyse YV2 aktif
                {
                  digitalWrite(14, HIGH); //YV2 aktif
                  digitalWrite(15, LOW);
                  flag2 = true;
                  client.println(timestamp() + "S3 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV2 Aktif.");
                }

              if(flag2 == true && digitalRead(4) == HIGH)   //S4 Anahtarı Aktif
                {
                  time_2 = millis();  // start timer
                  flag2 = false;
                  client.println(timestamp() + "S4 Sınır Anahtarı Tetiklendi.");
                }
              
              if(millis() >= time_2 + 3000)   //2 saniye bekledikten sonra dönüyor
                {
                  digitalWrite(14, LOW);  //YV3 aktif
                  digitalWrite(15, HIGH);
                  time_2 = time_2 + 9999999;  //stop timer
                  client.println(timestamp() + "YV3 Aktif.");
                }

              if(client.available()>0)
                {
                  c = client.read();  // datayı oku.
                }
              if(c == 'z')
                {
                  c = 0; //yeni data için sıfırlama.
                  client.println(timestamp() + "Deney Durduruldu.");
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  delay(5);
                  digitalWrite(13, LOW);
                  
                  digitalWrite(14, LOW);
                  digitalWrite(15, HIGH);
                  delay(5);
                  digitalWrite(15, LOW);
                  break;
                }
            }
            digitalWrite(12, LOW);  //YV1 aktif
            digitalWrite(13, HIGH);
            digitalWrite(14, LOW);  //YV3 aktif
            digitalWrite(15, HIGH);
        }

        else if(c == 'G') // ######################################## Deney 26. ########################################
        {
          digitalWrite(12, HIGH); //YV0 açık
          digitalWrite(13, LOW);
          digitalWrite(14, HIGH); //YV2 açık
          digitalWrite(15, LOW);
          flag1 = true;
          flag2 = true;
          int i = 0;
          int j = 0;
            while(i<3 || j<3)
            {
              // 1. Piston 3 defa ileri geri döngüsünü yapıyor
              if(flag1 == false && digitalRead(1) == HIGH && i<3)  //S1 anahtarı tetiklendiyse YV0 aktif
                {
                  digitalWrite(12, HIGH);
                  digitalWrite(13, LOW);
                  client.println(timestamp() + "S1 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV0 Aktif.");
                  flag1 = true;
                }
              if(flag1 == true && digitalRead(3) == HIGH && i<3)   //S2 anahtarı tetiklendiyse YV1 aktif
                {
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  client.println(timestamp() + "S2 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV1 Aktif.");
                  i++;
                  flag1 = false;
                }

              // 2. Piston 3 defa ileri geri döngüsünü yapıyor
              if(flag2 == false && digitalRead(2) == HIGH && j<3)  //S3 anahtarı tetiklendiyse YV2 aktif
                {
                  digitalWrite(14, HIGH);
                  digitalWrite(15, LOW);
                  client.println(timestamp() + "S3 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV2 Aktif.");
                  flag2 = true;
                }
              if(flag2 == true && digitalRead(4) == HIGH && j<3)   //S4 anahtarı tetiklendiyse YV3 aktif
                {
                  digitalWrite(14, LOW);
                  digitalWrite(15, HIGH);
                  client.println(timestamp() + "S4 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV3 Aktif.");
                  j++;
                  flag2 = false;
                }

              if(client.available()>0)
                {
                  c = client.read();  // datayı oku.
                }
              if(c == 'z')
                {
                  c = 0; //yeni data için sıfırlama.
                  client.println(timestamp() + "Deney Durduruldu.");
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  delay(5);
                  digitalWrite(13, LOW);
                  
                  digitalWrite(14, LOW);
                  digitalWrite(15, HIGH);
                  delay(5);
                  digitalWrite(15, LOW);
                  break;
                }
            }
            flag1 = true;
            flag2 = true;
            i = 0;
            i = 0;
            while(true){
              // ----- 1. Piston ileri gidip 3 sn bekleyip geri dönüp 2 sn bekliyor -----
              if(flag1 == false && digitalRead(1) == HIGH)  // S1 Anahtarı Aktif
                {
                  time_1 = millis();  // start timer
                  flag1 = true;
                  client.println(timestamp() + "S1 Sınır Anahtarı Tetiklendi.");
                }
              if(millis() >= time_1 + 3000)  //3 saniye bekledikten sonra YV0 aktif
                {
                  digitalWrite(12, HIGH);
                  digitalWrite(13, LOW);
                  
                  time_1 = time_1 + 9999999;  // stop timer
                  client.println(timestamp() + "YV0 Aktif.");
                }
              if(flag1 == true && digitalRead(3) == HIGH)   //S2 Anahtarı Aktif
                {
                  time_2 = millis();  // start timer
                  flag1 = false;
                  client.println(timestamp() + "S2 Sınır Anahtarı Tetiklendi.");
                }
              if(millis() >= time_2 + 2000)   //2 saniye bekledikten sonra YV1 aktif
                {
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  time_2 = time_2 + 9999999;  //stop timer
                  client.println(timestamp() + "YV1 Aktif.");
                }
              
              // ----- 2. Piston ileri gidip 3 sn bekleyip geri dönüp 3 sn bekliyor -----
              if(flag2 == false && digitalRead(2) == HIGH)  // S3 Anahtarı Aktif
                {
                  time_3 = millis();  // start timer
                  flag2 = true;
                  client.println(timestamp() + "S3 Sınır Anahtarı Tetiklendi.");
                }
              if(millis() >= time_3 + 3000)  //3 saniye bekledikten sonra YV0 aktif
                {
                  digitalWrite(14, HIGH);
                  digitalWrite(15, LOW);
                  
                  time_3 = time_3 + 9999999;  // stop timer
                  client.println(timestamp() + "YV2 Aktif.");
                }
              if(flag2 == true && digitalRead(4) == HIGH)   //S4 Anahtarı Aktif
                {
                  time_4 = millis();  // start timer
                  flag2 = false;
                  client.println(timestamp() + "S4 Sınır Anahtarı Tetiklendi.");
                }
              if(millis() >= time_4 + 2000)   //2 saniye bekledikten sonra YV1 aktif
                {
                  digitalWrite(14, LOW);
                  digitalWrite(15, HIGH);
                  time_4 = time_4 + 9999999;  //stop timer
                  client.println(timestamp() + "YV3 Aktif.");
                }

              if(client.available()>0)
                {
                  c = client.read();  // datayı oku.
                }
              if(c == 'z')
                {
                  c = 0; //yeni data için sıfırlama.
                  client.println(timestamp() + "Deney Durduruldu.");
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  delay(5);
                  digitalWrite(13, LOW);
                  
                  digitalWrite(14, LOW);
                  digitalWrite(15, HIGH);
                  delay(5);
                  digitalWrite(15, LOW);
                  break;
                }
            }
            digitalWrite(12, LOW);  //YV1 aktif
            digitalWrite(13, HIGH);
            digitalWrite(14, LOW);  //YV3 aktif
            digitalWrite(15, HIGH);
        }

        else if(c == 'h') // ######################################## Deney 27. ########################################
        { //SONRA TEKRAR KONTROL ET 3 DEFA DÖNGÜSÜNÜ
          digitalWrite(12, HIGH); //YV0 aktif
          digitalWrite(13, LOW);
          digitalWrite(14, HIGH); //YV1 aktif
          digitalWrite(15, LOW);
          flag1 = true;
          flag2 = true;
          int i = 0;
            while(true){
              // 1. Piston ileri geri döngüsünü yapıyor
              if(flag1 == false && digitalRead(1) == HIGH)  //S1 anahtarı tetiklendiyse YV0 aktif
                {
                  digitalWrite(12, HIGH); //YV0 aktif
                  digitalWrite(13, LOW);
                  flag1 = true;
                  client.println(timestamp() + "S1 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV0 Aktif.");
                }
              if(flag1 == true && digitalRead(3) == HIGH)   //S2 anahtarı tetiklendiyse YV1 aktif
                {
                  digitalWrite(12, LOW);  //YV1 aktif
                  digitalWrite(13, HIGH);
                  flag1 = false;
                  i++;
                  if(i>4)
                  {
                    i = 0;
                  }
                  client.println(timestamp() + "S2 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV1 Aktif.");
                }
              // 2. Piston ileri geri döngüsünü yapıyor
              if(flag2 == false && digitalRead(2) == HIGH && i<4)  //S3 anahtarı tetiklendiyse YV2 aktif
                {
                  digitalWrite(14, HIGH); //YV2 aktif
                  digitalWrite(15, LOW);
                  flag2 = true;
                  client.println(timestamp() + "S3 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV2 Aktif.");
                }
              if(flag2 == true && digitalRead(4) == HIGH && i<4)   //S4 anahtarı tetiklendiyse YV3 aktif
                {
                  digitalWrite(14, LOW);  //YV3 aktif
                  digitalWrite(15, HIGH);
                  flag2 = false;
                  client.println(timestamp() + "S4 Sınır Anahtarı Tetiklendi.");
                  client.println(timestamp() + "YV3 Aktif.");
                }

              if(client.available()>0)
                {
                  c = client.read();  // datayı oku.
                }
              if(c == 'z')
                {
                  c = 0; //yeni data için sıfırlama.
                  client.println(timestamp() + "Deney Durduruldu.");
                  digitalWrite(12, LOW);
                  digitalWrite(13, HIGH);
                  delay(5);
                  digitalWrite(13, LOW);
                  
                  digitalWrite(14, LOW);
                  digitalWrite(15, HIGH);
                  delay(5);
                  digitalWrite(15, LOW);
                  break;
                }
            }
            digitalWrite(12, LOW);  //YV1 aktif
            digitalWrite(13, HIGH);
            digitalWrite(14, LOW); //YV3 aktif
            digitalWrite(15, HIGH);
          }









        


        else if(c == 'm')
        {
          client.println(timestamp() + timestamp() + "12 HIGH, 13 LOW");
          client.println(timestamp() + timestamp() + "12 HIGH, 13 LOW");
          digitalWrite(12, HIGH);
          digitalWrite(13, LOW);
          
          time_1 = millis();
          time_2 = millis() + 1000;
          c = 0; //yeni data için sıfırlama.
          
          while(client.connected())
          {
            if(millis() >= time_1 + 1000){
                client.println(timestamp() + timestamp() + "12 LOW, 13 HIGH");
                client.println(timestamp() + timestamp() + "12 LOW, 13 HIGH");
                digitalWrite(12, LOW);
                digitalWrite(13, HIGH);
                time_1 +=2000;
            }
            if(millis() >= time_2 + 1000){
                client.println(timestamp() + timestamp() + "12 HIGH, 13 LOW");
                client.println(timestamp() + timestamp() + "12 HIGH, 13 LOW");
                digitalWrite(12, HIGH);
                digitalWrite(13, LOW);
                time_2 +=2000;
            }
            //client.println(timestamp() + "test1");
            
            if(client.available()>0)
            {
              c = client.read();  // datayı oku.
            }
            if(c == 'z')
            {
              c = 0; //yeni data için sıfırlama.
              client.println(timestamp() + "Deney Durduruldu.");
              digitalWrite(12, LOW);
              digitalWrite(13, HIGH);
              delay(5);
              digitalWrite(13, LOW);
              
              digitalWrite(14, LOW);
              digitalWrite(15, HIGH);
              delay(5);
              digitalWrite(15, LOW);
              break;
            }
            //client.println(timestamp() + "test2");
          }
        }

        else{
          c = 0;
        }
      }
 
      delay(10);
    }
 
    //client.stop();
    client.println(timestamp() + "Client disconnected");
 
  }
  
  

}
