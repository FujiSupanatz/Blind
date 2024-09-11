//BlinDee
#include <WiFi.h>
#include <TimeLib.h> // เอามาเทสเฉยๆ 
#include "pitches.h"
#include <HTTPClient.h>
// ลบสิ่งที่ไม่ใช้ออกไป

#define TRIG_PIN 2 
#define ECHO_PIN 14 // set ให้ trig คือ pin 12 echo 14 
#define BUZZER_PIN 16
#define THRESHOLD_DISTANCE 155.5
#define MAX_FREQUENCY 1700 // Max frequen
#define MIN_FREQUENCY 1700
#define BUZZER_DURATION 150 // มิลิวินาที ที่มันจะร้อง
#define DELAY_MIN 250 // ยิ่งใกล้ยิ่งรัวงับ
#define DELAY_MAX 1500
// ประกาศfunction
void showdetect();
void soundBuzzer(int frequency,int duration);
void lineSetup();
void sendLineNotify(String message);
void wificlient();
void setupserver();

const char* ssid = "Office_02";
const char* password = "acr@1963";
WiFiServer server(80);
// ip เซิฟ = 172.20.10.3
String linetoken = "IPtMFTV3wkbGrWjjCWR0pklvQON6n3OvWAZRwehpeVS"; //token เอาไว้ sent 
//4cgZzRWh0Nk4RsTvTSrGQH9I9GtUTWeLexmuSWbfYAH

// ไบล์นดี IPtMFTV3wkbGrWjjCWR0pklvQON6n3OvWAZRwehpeVS
void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT); // Set the Trigger pin as an output
  pinMode(ECHO_PIN, INPUT);  // Set the Echo pin as an input
  pinMode(BUZZER_PIN, OUTPUT); // Set the Buzzer pin as an output
  digitalWrite(BUZZER_PIN, LOW); // BUZ1
  lineSetup();
  setupserver();
}
void setupserver(){
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
  Serial.println(ssid);
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void wificlient(){
      WiFiClient client = server.available();
   if (client) {
      Serial.println("New client connected");
      while (client.connected()) {
         if (client.available()) {
            String message = client.readStringUntil('\n');
            Serial.println("get: " + message);
            
            // Check if the message received is "Twenty"
            if (message.indexOf("Twenty") >= 0) {
               sendLineNotify("แบงก์ยี่สิบ");
            }
            if (message.indexOf("Fifty") >= 0) {
               sendLineNotify("แบงก์ห้าสิบ");
            }
            if (message.indexOf("Hundred") >= 0){
              sendLineNotify("แบงก์ร้อย");
            }
            if (message.indexOf("car") >= 0){
              sendLineNotify("รถสองแถว สายสีขาว ตะพงหาดแม่รำพึง");
            }
            client.println("Message received: " + message); // Send a response back to the client
         }
      }
      client.stop();
      Serial.println("Client disconnected!!");
   }
}

void lineSetup(){
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(5000);
    Serial.println("Connecting to WIfi");
  }
  Serial.println("Connected to WIFI");
  sendLineNotify("ESP32 connected successfully!");
}

void sendLineNotify(String message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    http.begin("https://notify-api.line.me/api/notify");  // LINE Notify API URL
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.addHeader("Authorization", "Bearer " + linetoken);
    
    String postData = "message=" + message;
    
    int httpResponseCode = http.POST(postData);
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }
    
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
  delay(3000);
}

float detect() { // function for ultrasonic 
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH);
    float distance = (duration * 0.0343) / 2;

    return distance;
}


void showdetect()/*showdtect ให้ผู้ใช้รู้ว่าจะชน*/ {
  float dis = detect(); // dis หมายถึง distance nakub
  
  if (dis <= THRESHOLD_DISTANCE) {
    // Display current time and distance on serial monitor
    Serial.print(" Distance: ");
    Serial.println(dis);
    Serial.println("-------------------------------");
    

    int buzzerFrequency = map(dis, 0, THRESHOLD_DISTANCE, MAX_FREQUENCY, MIN_FREQUENCY);
    buzzerFrequency = constrain(buzzerFrequency, MIN_FREQUENCY, MAX_FREQUENCY);
    // คำนวน
    float normalizedDistance = dis / THRESHOLD_DISTANCE;  // Normalized distance (0 to 1)
    int delayBetweenSounds = DELAY_MIN + (DELAY_MAX - DELAY_MIN) * pow(normalizedDistance, 2); // Quadratic scaling
    delayBetweenSounds = constrain(delayBetweenSounds, DELAY_MIN, DELAY_MAX);
    
    // Play sound
    soundBuzzer(buzzerFrequency, BUZZER_DURATION);
    delay(delayBetweenSounds); 

  } else {
    // Ensure buzzer is off if distance is greater than threshold
    soundBuzzer(0, 0);
    delay(1000); // Default delay when no object is near
  }
}

void soundBuzzer(int frequency, int duration) {
if (frequency > 0) {
         tone(BUZZER_PIN, frequency); // Generate a tone at the specified frequency for BUZZER1
        delay(duration); // Keep buzzers on for the duration
        noTone(BUZZER_PIN); // Turn off BUZZER1
    } else {
        noTone(BUZZER_PIN); // Ensure buzzer is off
    }
}
//ง่
void loop() {
  showdetect();
  wificlient();
  //sendLineNotify("รถสองแถว สายสีขาว ตะพงหาดแม่รำพึง");
  //delay(30000);
}