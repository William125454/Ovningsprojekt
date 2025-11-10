  /*
    * Name: övningsprojekt
    * Author: William Nilsson
    * Date: 2025-11-10
    * Description: This project uses a ds3231 to measure time and displays the time to an 1306 oled display, 
    * Further, it measures temprature with ds3231 and displays a mapped value to a 9g-servo-motor.
  */

  // Include Libraries
  #include <RTClib.h>
  #include <Wire.h>
  #include <U8glib.h>
  #include <Servo.h>
  #include <Adafruit_NeoPixel.h>

  U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK); // OLED DISPLAY

  int servoPin = 10;
  Servo tempServo;

  const int buzzerPin = 9;
  int pos = 0;

  // Construct objects
  RTC_DS3231 rtc;

  #define LED_PIN 11
  #define NUM_LEDS 32
  Adafruit_NeoPixel ring = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

  void setup() {
    // init communication
    Serial.begin(9600);
    Wire.begin();
    rtc.begin();

    // Settings
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Set rtc time
    tempServo.attach(servoPin);
    u8g.setFont(u8g_font_6x10);

    pinMode(buzzerPin, OUTPUT);
  }

  void loop() { // Loop that calls all functions
    float temp = getTemp();

    servoWrite(temp); // moves servo based on temp
    Serial.println(getTime());
    Serial.println(temp);
    
    oledWrite(getTime() + "  " + String(temp, 1) + "C"); // shot temp + time on OLED display


    delay(1000);
  }

  //This function reads time from an ds3231 module and package the time as a String
  String getTime() {
    DateTime now = rtc.now();
    return String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
  }

  //This function takes temprature from ds3231 and returns as a float
  float getTemp() {
    return rtc.getTemperature();
  }

  // Display text to OLED
  void oledWrite(String text) {
    u8g.firstPage();
    do {
      u8g.drawStr(0, 20, text.c_str());
    } while (u8g.nextPage());
  }

  //Move servo according to temperature
  void servoWrite(float temp) {
    if (temp < 15) temp = 15;
    if (temp > 30) temp = 30;

    int servoPos = (int)((temp - 15) * 12); // Calculating servo pos
    tempServo.write(servoPos);

    Serial.print("temp");
    Serial.print(temp);
    Serial.print(" °C -> Servo: ");
    Serial.println(servoPos);

    if (temp > 25) { // Sound buzzer if temp > 25
      tone(buzzerPin, 1000); // Melody start
      delay(200);
      noTone(buzzerPin);
      delay(100);
      tone(buzzerPin, 2000);
      noTone(buzzerPin);
    } else {
      noTone(buzzerPin);  // Turn off buzzer
    }
  }
