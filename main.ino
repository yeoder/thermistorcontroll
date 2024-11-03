#include <WiFi.h>
#include <Arduino.h>
#include <WebServer.h>
#include <WiFiManager.h>
#include "html.h"           // Includes HTML code for web page
#include "sensor.h"         // Includes sensor functions
#include "calculation.h"    // Includes temperature conversion functions
#include "potentiometer.h"  // Includes potentiometer control functions
#include "ohmTablePage.h"   // New Ohm Table page

WebServer server(80);

void handleRoot() {
    server.send_P(200, "text/html", htmlPage);  // Serve the HTML page
}

void handleGetTemp() {
    float temperature = readTemperature();  // Fetch current temperature
    int resistance = getCurrentResistance();
    String jsonResponse = "{\"temperature\": " + String(temperature) + ", \"resistance\": " + String(resistance) + "}";
    server.send(200, "application/json", jsonResponse);  // Send temperature and resistance in JSON format
}

void handleOhmTablePage() {
    server.send_P(200, "text/html", ohmTablePage); // Serve Ohm Table page
}

void handleSetResistance() {
    if (server.hasArg("value")) {
        int targetResistance = server.arg("value").toInt();
        setDigitalPotentiometer(targetResistance);
        server.send(200, "text/plain", "Resistance set to: " + String(targetResistance));
    } else {
        server.send(400, "text/plain", "Resistance parameter missing.");
    }
}

void handleCalculateResistance() {
    if (server.hasArg("temperature") && server.hasArg("R25") && server.hasArg("B_value")) {
        float temperature = server.arg("temperature").toFloat();
        float R25 = server.arg("R25").toFloat();
        float B_value = server.arg("B_value").toFloat();

        float calculatedOhm = calculateOhmFromTemperature(temperature, R25, B_value);
        String response = "{\"calculatedOhm\": " + String(calculatedOhm) + "}";
        server.send(200, "application/json", response);
    } else {
        // Skicka JSON med ett felmeddelande istället för text/plain
        String errorResponse = "{\"error\": \"Missing parameters\"}";
        server.send(400, "application/json", errorResponse);
    }
}



void sendToPotentiometer(float resistance) {
    int targetValue = (int)resistance; // Conversion as required for potentiometer
    // Call function to adjust potentiometer to `targetValue`
    setDigitalPotentiometer(targetValue);
}

void setup() {
    Serial.begin(115200);
    WiFiManager wifiManager;
    if (!wifiManager.autoConnect("ESP32_Config")) {
        Serial.println("Failed to connect to WiFi.");
    } else {
        Serial.println("Connected to WiFi!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
    }

    server.on("/", HTTP_GET, handleRoot);
    server.on("/ohmTablePage", handleOhmTablePage);  // Ohm Table page route
    server.on("/getTemp", HTTP_GET, handleGetTemp);
    server.on("/setResistance", HTTP_GET, handleSetResistance);
    server.on("/calculateResistance", HTTP_GET, handleCalculateResistance); 

    setupPotentiometer();  // Initialize potentiometer
    server.begin();
}

void loop() {
    server.handleClient(); // Handle incoming web server requests

    static unsigned long lastLogTime = 0;  // Track last time a full log occurred
    unsigned long currentMillis = millis(); // Get the current time

    if (currentMillis - lastLogTime >= 10000) { // Every 10 seconds
        // 1. Read temperature from sensor
        float temperature = readTemperature();
        Serial.print("Calculated Temperature from Sensor: ");
        Serial.print(temperature);
        Serial.println(" °C");

        // 2. Calculate ohm value based on temperature (example uses B-value calculation)
        float calculatedOhm = calculateOhmFromTemperature(temperature, R25, B_value); // Use an appropriate function
        Serial.print("Calculated Ohm Value: ");
        Serial.print(calculatedOhm);
        Serial.println(" Ω");

        // 3. Send the calculated ohm to the potentiometer
        sendToPotentiometer(calculatedOhm); // Assume function adjusts potentiometer
        Serial.print("Sent to Potentiometer: ");
        Serial.print(calculatedOhm);
        Serial.println(" Ω");

        // Update the last log time
        lastLogTime = currentMillis;
    }
}
