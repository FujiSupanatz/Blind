//เอาไว้ test kub
//เอาๆปทำline notI ด้วยดส้จแล้วลบออก
#include <WiFi.h>
#include <TimeLib.h> // เอามาเทสเฉยๆ 
#include "pitches.h"
#include <HTTPClient.h>

#define TRIG_PIN 2 
#define ECHO_PIN 14 // set ให้ trig คือ pin 12 echo 14 
#define BUZZER_PIN 16
#define BUZZER_PIN2 17
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

const char* ssid = " ";
const char* password = " ";

String linetoken = "4cgZzRWh0Nk4RsTvTSrGQH9I9GtUTWeLexmuSWbfYAH"; //token เอาไว้ sent 
//4cgZzRWh0Nk4RsTvTSrGQH9I9GtUTWeLexmuSWbfYAH
void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT); // Set the Trigger pin as an output
  pinMode(ECHO_PIN, INPUT);  // Set the Echo pin as an input
  pinMode(BUZZER_PIN, OUTPUT); // Set the Buzzer pin as an output
  digitalWrite(BUZZER_PIN, LOW); // BUZ1
  //BUZ2
  pinMode(BUZZER_PIN2, OUTPUT); // Set the Buzzer pin as an output
  digitalWrite(BUZZER_PIN2, LOW);
  lineSetup();
  setTime(12, 0, 0, 1, 1, 2024);
}

void lineSetup(){
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(5000);
    Serial.println("Connecting to WIfi............................");
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
    Serial.print("Current Time: ");
    Serial.print(hour());
    Serial.print(":");
    Serial.print(minute());
    Serial.print(":");
    Serial.print(second());
    Serial.print(" Distance: ");
    Serial.println(dis);
    Serial.println("-------------------------------");
    int buzzerFrequency = map(dis, 0, THRESHOLD_DISTANCE, MAX_FREQUENCY, MIN_FREQUENCY);
    buzzerFrequency = constrain(buzzerFrequency, MIN_FREQUENCY, MAX_FREQUENCY);
    // Calculate delay between buzzer sounds with a more pronounced scale
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
        tone(BUZZER_PIN2, frequency); // Generate a tone at the specified frequency for BUZZER2
        delay(duration); // Keep buzzers on for the duration
        noTone(BUZZER_PIN); // Turn off BUZZER1
        noTone(BUZZER_PIN2);
    } else {
        noTone(BUZZER_PIN); // Ensure buzzer is off
        noTone(BUZZER_PIN2);
    }
}

void loop() {
  showdetect();
  delay(10000);
  float distance = detect();
  String message = " " + String(distance) + " cm";
  sendLineNotify(message);
}
