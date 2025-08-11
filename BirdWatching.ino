#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "esp_camera.h"

// Define all ESP32-CAM pins
#define PWDN_GPIO_NUM  32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM  0
#define SIOD_GPIO_NUM  26
#define SIOC_GPIO_NUM  27
#define Y9_GPIO_NUM    35
#define Y8_GPIO_NUM    34
#define Y7_GPIO_NUM    39
#define Y6_GPIO_NUM    36
#define Y5_GPIO_NUM    21
#define Y4_GPIO_NUM    19
#define Y3_GPIO_NUM    18
#define Y2_GPIO_NUM    5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM  23
#define PCLK_GPIO_NUM  22

// Define the PIR sensor pin
#define PIR_PIN 13

// Wi-Fi Credentials and Telegram
const char ssid[] = "*";
const char pass[] = "*";
const char ID[] = "*";
const char token[] = "*";

void setup() {
  // Basic setup
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT);

  // Connect to WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  Serial.print("Connecting to WiFi ...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Success!");

  // Initialize ESP32-CAM
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
  config.frame_size = FRAMESIZE_VGA;
  config.jpeg_quality = 32;
  config.fb_count = 1;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.println("Camera init failed");
    delay(2000);
    for (int i = 0; i < 3; i++) {
      Serial.println("Retrying init...");
      err = esp_camera_init(&config);
      if (err == ESP_OK) break;
      delay(2000);
    }
    
    if (err != ESP_OK) {
      Serial.println("Camera failed, pausing...");
      while (true) {
        delay(1000);
      }
    }
  }
}

void loop() {
  // No motion, sleep
  if (digitalRead(PIR_PIN) == LOW) {
    esp_sleep_enable_ext0_wakeup((gpio_num_t)PIR_PIN, 1); // Wake on signal
    Serial.println("Entering light sleep...");
    delay(100);
    esp_light_sleep_start();
  }

  // Code resumes
  Serial.println("Motion detected!");
  delay(1000);
  sendPhoto();
  delay(10000);
}

// Send images to Telegram
void sendPhoto() {
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Failed");
    return;
  }

  Serial.println("Photo captured. Sending...");

  // Check that WiFi is still connected
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("No connection, searching ..");
    delay(5000);
  }

  WiFiClientSecure client;
  client.setInsecure();

  if (!client.connect("api.telegram.org", 443)) {
    Serial.println("Connect to Telegram failed");
    esp_camera_fb_return(fb);
    return;
  }

  String boundary = "------------------------abcdef123456";
  String startRequest = "--" + boundary + "\r\n";
  startRequest += "Content-Disposition: form-data; name=\"chat_id\"\r\n\r\n";
  startRequest += String(ID) + "\r\n";
  startRequest += "--" + boundary + "\r\n";
  startRequest += "Content-Disposition: form-data; name=\"photo\"; filename=\"photo.jpg\"\r\n";
  startRequest += "Content-Type: image/jpeg\r\n\r\n";

  String endRequest = "\r\n--" + boundary + "--\r\n";

  String requestHead = 
    "POST /bot" + String(token) + "/sendPhoto HTTP/1.1\r\n" +
    "Host: api.telegram.org\r\n" +
    "Content-Type: multipart/form-data; boundary=" + boundary + "\r\n" +
    "Content-Length: " + String(startRequest.length() + fb->len + endRequest.length()) + "\r\n" +
    "Connection: close\r\n\r\n";

  client.print(requestHead);
  client.print(startRequest);

  // Send image buffer
  client.write(fb->buf, fb->len);
  client.print(endRequest);

  // Wait for server response
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") break;
  }

  // Get response
  String response = client.readString();
  Serial.println("Telegram response:");
  Serial.println(response);

  esp_camera_fb_return(fb);
}
