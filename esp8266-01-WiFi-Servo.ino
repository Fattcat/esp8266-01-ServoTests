#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

// Nastavenie Wi-Fi
const char* ssid = "ServoControll";  // Servo WiFi SSID
const char* password = "Servo001";   // Servo PASSWORD

// Vytvorenie servera na porte 80
ESP8266WebServer server(80);

// Pin pre servo motor
const int servoPin = 2;
Servo myServo;

// Nastavenie rozsahu pulzov pre maximálny rozsah otáčania serva
const int minPulseWidth = 500;    // Minimálny pulz v mikrosekundách (nastavte podľa možností vášho serva)
const int maxPulseWidth = 2400;   // Maximálny pulz v mikrosekundách (nastavte podľa možností vášho serva)

int servoPos = 0; // Počiatočná pozícia servo motora (v stupňoch 0 - 180)

void setup() {
  // Inicializácia sériového monitoru
  Serial.begin(115200);

  // Pripojenie serva k definovanému pinu a nastavenie rozsahu pulzov
  myServo.attach(servoPin, minPulseWidth, maxPulseWidth);
  myServo.write(servoPos); // Nastavenie serva na počiatočnú pozíciu 0 stupňov

  // Inicializácia Wi-Fi
  WiFi.softAP(ssid, password);
  Serial.println("Access point started");

  // IP adresa ESP8266
  Serial.println(WiFi.softAPIP());

  // Nastavenie webových handlerov
  server.on("/", handleRoot);
  server.on("/setPos", handleSetPos);
  server.on("/status", handleStatus);
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient(); // Obsluha prichádzajúcich požiadaviek
}

// Funkcia na obsluhu koreňovej stránky
void handleRoot() {
  String html = "<!DOCTYPE html><html lang='sk'><head><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<style>input[type=range] {width: 100%; margin: 20px 0;} ";
  html += "body {text-align: center; font-family: Arial, sans-serif;}</style></head><body>";
  html += "<h1>Ovládanie servo motora</h1>";
  html += "<p>Aktuálna pozícia: <span id='position'>" + String(servoPos) + "</span>°</p>";
  html += "<input type='range' min='0' max='180' value='" + String(servoPos) + "' class='slider' id='servoSlider'>";
  html += "<script>var slider = document.getElementById('servoSlider');";
  html += "slider.oninput = function() { fetch('/setPos?value=' + this.value); };";
  html += "setInterval(function() { fetch('/status').then(response => response.text()).then(data => {";
  html += "document.getElementById('position').innerText = data; slider.value = data; }); }, 1000);</script>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

// Funkcia na nastavenie pozície serva
void handleSetPos() {
  if (server.hasArg("value")) {
    servoPos = server.arg("value").toInt();
    if (servoPos >= 0 && servoPos <= 180) {
      myServo.write(servoPos);
    }
  }
  server.send(204); // Žiadna odpoveď (No Content)
}

// Funkcia na zobrazenie aktuálnej pozície serva
void handleStatus() {
  server.send(200, "text/plain", String(servoPos));
}
