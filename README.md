# Description
An ESP32-CAM and PIR sensor birdwatching system that detects motion and sends photos to a Telegram bot
# Features
* Motion detection via PIR sensor
* Sends photos to Telegram bot
* Sleep mode for power saving
* Adjustable image resolution and quality
# Hardware Used
* ESP32-CAM
* PIR motion sensor
* Breadboard power supply
* USB power bank
* Breadboard
# Software/Libraries
* Arduino IDE
* WiFi.h
* WiFiClientSecure.h
* esp_camera.h
# Wiring
![Wiring Setup](https://github.com/Jack-Tack/Birdwatcher/blob/main/images/wiring.png "Wiring")
# Telegram Setup
1. To obtain your ID, create a Telegram account and find a GetID bot, there are plenty available. When you start a chat it will give you your ID.
2. To create your own bot, start a chat with "BotFather" and type /newbot. After entering a display and username you will be given your bot token.
3. Begin a chat with the bot you've created before uploading code or there will be an error and you will receive no images.
# Code Setup
1. Insert WiFi details and Telegram bot information.
2. Plug ESP32-CAM into Arduino and ground the IO0 pin to the GND pin near it.
3. Upload and press the reset button on ESP32-CAM when "Connecting..." message appears.
4. After upload remove the grounding on IO0 and press the reset button again.
# Usage
# Troubleshooting
