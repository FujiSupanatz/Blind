//เอาไว้ test kub
#include <WiFi.h>
#include <TimeLib.h> // เอามาเทสเฉยๆ 
#include "pitches.h"

#define TRIG_PIN 2 
#define ECHO_PIN 14 // set ให้ trig คือ pin 12 echo 14 
#define BUZZER_PIN 16
#define THRESHOLD_DISTANCE 155.5
#define MAX_FREQUENCY 2000 // Max frequency for the buzzer
#define MIN_FREQUENCY 2000
#define BUZZER_DURATION 200
#define DELAY_MIN 50
#define DELAY_MAX 1550
// ประกาศfunction
void showdetect();
void soundBuzzer(int frequency,int duration);

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT); // Set the Trigger pin as an output
  pinMode(ECHO_PIN, INPUT);  // Set the Echo pin as an input
  pinMode(BUZZER_PIN, OUTPUT); // Set the Buzzer pin as an output
  digitalWrite(BUZZER_PIN, LOW);
  setTime(12, 0, 0, 1, 1, 2024);
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
        tone(BUZZER_PIN, frequency); // Generate a tone at the specified frequency
        delay(duration); // Keep buzzer on for the duration
        noTone(BUZZER_PIN); // Turn off the buzzer
    } else {
        noTone(BUZZER_PIN); // Ensure buzzer is off
    }
}

void loop() {
  showdetect();
}