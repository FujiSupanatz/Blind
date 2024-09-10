#include <WiFi.h>
// รอ AII 
// WiFi cred
const char* ssid = "Office_02";
const char* password = "acr@1963";

// Server details
const char* serverIP = "192.168.88.121";
const int serverPort = 80;

void setup() {
  Serial.begin(115200);
  connectToWiFi();
}

void loop() {
  sendDataToServer();
  delay(5000); // Wait 5 seconds before sending another message
}

// Function to connect to WiFi
void connectToWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".");
  }

  Serial.println("Connected to WiFi");
}

// Function to send data to the server
void sendDataToServer() {
  WiFiClient client;

  if (client.connect(serverIP, serverPort)) {
    client.println("car");
    // Wait for the server response
    while (client.available()) {
      String response = client.readStringUntil('\n');
      Serial.println("Response from server: " + response);
    }
    client.stop();
  } else {
    Serial.println("Connection to server failed");
  }
