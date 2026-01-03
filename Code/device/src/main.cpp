#define DEBUG_ESP_PORT Serial

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_PN532.h>
#include <Servo.h>
#include <ESP8266WebServer.h>

#define BUZZER 0

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_PN532 nfc(-1, -1); // Set up the NFC reader with the correct pins


#define BUZZER_PIN D4
#define BLUE_LED_PIN D0
#define GREEN_LED_PIN D5
#define RED_LED_PIN D8
#define SERVO_PIN D7
#define SERVO_PIN2 D6


Servo myServo;
Servo myServo2;

const char* ssid = "Vodafone-277D"; 
const char* password = "ueWBtjqw6w8fupAx";
const char* serverName = "https://ipcamguineapig.ddnss.de/receive";

WiFiClientSecure wifiClient;
ESP8266WebServer server(80);

struct User {
  uint8_t uid[7];        // UID of the NFC card
  String name;           // User's name
  String medicine;       // Medicine name and dosage
  int servoAngle;        // Servo motor angle rotation
};

// Example user data
User users[] = {
  { {0x4, 0xE8, 0xDF, 0x61, 0xB5, 0x2A, 0x81}, "Alice", "Aspirin 10mg", 0 },
  { {0x4, 0xD9, 0xC8, 0x62, 0xB5, 0x2A, 0x81}, "Bob", "Fentanyl 20mg", 45 },
  { {0x4, 0xEA, 0x86, 0x62, 0xB5, 0x2A, 0x81}, "Charlie", "Insulin 30mg", 135 }
};
const int numUsers = sizeof(users) / sizeof(users[0]);

uint8_t lastUID[7];
uint8_t lastUIDLength = 0;

const long interval = 5000;  // wait duration per user (milliseconds)
const long startMillis = 10000;  // start of program
int currentIndex = 0;
unsigned long lastMillis = startMillis;

void buzz() {
  if (BUZZER) {
    tone(BUZZER_PIN, 1000, 1000);
    delay(1000);
    noTone(BUZZER_PIN);
  }  
}

void sendRequest(String message, bool success) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String success_string = success ? "success" : "fail";
    String url = serverName + String("?message=" + success_string + "/" + message);
    wifiClient.setInsecure();
    wifiClient.connect(url, 443);
    http.begin(wifiClient, url);
    
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      Serial.println("Sent slack message successfully");
    } else {
      String intStr = String(httpResponseCode);
      Serial.println("Error on HTTP request " + intStr);
    }

    http.end();
  } else {
    Serial.println("Not connected to WiFi");
  }
}

void setColor(int redValue, int greenValue, int blueValue) {
  analogWrite(RED_LED_PIN, redValue);
  analogWrite(GREEN_LED_PIN, greenValue);
  analogWrite(BLUE_LED_PIN, blueValue);
}


void dispense(User user) {
  // Display user information
      display.println("User: " + user.name);
      display.println("Medicine: " + user.medicine);
      display.println("Servo Angle: " + String(user.servoAngle));
      display.display();

      setColor(0, 255, 0); // Green Color
      delay(1000);
      setColor(0, 0, 0); // Reset

      buzz();

      // Move the servo to the specified angle slowly
      int currentAngle = myServo.read();
      int targetAngle = user.servoAngle;
      if (targetAngle > currentAngle) {
        for (int angle = currentAngle; angle <= targetAngle; angle++) {
          myServo.write(angle);
          delay(15); // Adjust the speed of movement
        }
      } else {
        for (int angle = currentAngle; angle >= targetAngle; angle--) {
          myServo.write(angle);
          delay(15); // Adjust the speed of movement
        }
      }

      //Move second motor to open hole / close hole
      delay(4000);
      myServo2.write(90);
      delay(4000);
      myServo2.write(0);
      lastMillis += 8000;
      sendRequest(user.name, true);

}

void handleGetRequest() {
  if (server.hasArg("arg")) {
    String message = server.arg("arg");
    User user;
    String responseMessage;
    // Clear the display
    display.clearDisplay();
    display.setCursor(0, 0); 
    // Convert message to integer
    int index = message.toInt();
    if (index >= 0 && index < numUsers) {
      user = users[index];
      dispense(user);
      responseMessage = "Success";
    } else {
      Serial.println("Invalid argument received");
      responseMessage = "Fail";
    }
    server.send(200, "text/plain", responseMessage);
  } else {
    server.send(200, "text/plain", "No argument received");
  }
}

void dose_missed(User user) {
  display.println(user.name + " missed dose!" );
  display.display();

  setColor(255, 0, 0); // Red Color
  delay(1000);
  setColor(0, 0, 0); // Reset

  buzz();

  // Move the servo to the specified angle slowly
  int currentAngle = myServo.read();
  int targetAngle = user.servoAngle;
  if (targetAngle > currentAngle) {
    for (int angle = currentAngle; angle <= targetAngle; angle++) {
      myServo.write(angle);
      delay(15); // Adjust the speed of movement
    }
  } else {
    for (int angle = currentAngle; angle >= targetAngle; angle--) {
      myServo.write(angle);
      delay(15); // Adjust the speed of movement
    }
  }

  sendRequest(user.name, false);
}

void setup(void) {
  Serial.begin(9600);
  Serial.println("Hello!");

  // Initialize WiFi
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  // Initialize the server
  server.on("/get", handleGetRequest);
  server.begin();
  Serial.println("Server started");

  // Initialize the NFC reader
  Serial.println("Initializing NFC reader...");
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("Didn't find PN53x board");
    while (1); // halt
  }
  nfc.SAMConfig();
  Serial.println("NFC reader initialized. Waiting for an NFC tag...");

  // Initialize the OLED display
  Serial.println("Initializing OLED display...");
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  // Clear the buffer
  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.print(F("OLED initialized"));
  display.display();
  delay(2000);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  pinMode(BLUE_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);

  // Initialize the Servo
  myServo.attach(SERVO_PIN,500,2500);
  myServo.write(0); // Initialize to 0 degrees

  myServo2.attach(SERVO_PIN2,500,2500);
  myServo2.write(0); // Initialize to 0 degrees
  delay(2000);

}



void loop(void) {
  server.handleClient();
  unsigned long currentMillis = millis();
  if (currentMillis <= startMillis) {
    return;
  }
  if (currentMillis - lastMillis <= interval) {
    return;
  }
  lastMillis = currentMillis;
  // Buffer to store the returned UID
  uint8_t uid[7] = {0};
  uint8_t uidLength = 0;
  bool success;

  User user = users[currentIndex];

  success = false;
  
  display.clearDisplay();
  display.setCursor(0, 0);    
  display.println(user.name + " dose time" );
  display.display();

  setColor(0, 0, 255); // Blue Color
  delay(1000);
  setColor(0, 0, 0); // Reset

  buzz();

  while (!success && ((millis() - currentMillis) <= interval)) 
  {
      if(nfc.startPassiveTargetIDDetection(PN532_MIFARE_ISO14443A)) {
      Serial.println("Card detected");

      nfc.readDetectedPassiveTargetID(uid, &uidLength);

      if ((memcmp(uid, user.uid, uidLength) == 0) && ((millis() - currentMillis) <= interval))
      {
        success = true;
      } 
      }
  }

  if(success)
  {
    dispense(user);
  }
  else 
  {
    dose_missed(user);
  }

  currentIndex = (currentIndex + 1) % numUsers;
  //exit(0);

}



