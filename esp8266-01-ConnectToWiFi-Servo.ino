#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

// Nastavenie Wi-Fi pripojenia
const char* ssid = "Your-WiFi-SSID-HERE";                 // Názov existujúcej Wi-Fi siete
const char* password = "Your-WiFi-Password-HERE"; // Heslo do Wi-Fi siete

// Vytvorenie servera na porte 80
ESP8266WebServer server(80);

// Pin pre servo motor
const int servoPin = 2;
Servo myServo;

const int minPulseWidth = 500;    // Minimálny pulz v mikrosekundách (nastavte podľa možností vášho serva)
const int maxPulseWidth = 2400;   // Maximálny pulz v mikrosekundách (nastavte podľa možností vášho serva)

int servoPos = 0; // Počiatočná pozícia servo motora (v stupňoch 0 - 180)

void setup() {
  Serial.begin(115200);

  myServo.attach(servoPin, minPulseWidth, maxPulseWidth);
  myServo.write(servoPos); // Nastavenie serva na počiatočnú pozíciu 0 stupňov

  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");

  // Čakanie na pripojenie a logovanie IP adresy
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  
  // Testovanie bez statickej IP adresy, DHCP pridelí IP
  Serial.print("ESP8266 IP Address: ");
  Serial.println(WiFi.localIP());

  // Nastavenie webových handlerov
  server.on("/", handleRoot);
  server.on("/setPos", handleSetPos);
  server.on("/status", handleStatus);
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  String html = "<!DOCTYPE html><html lang='en'><head><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<style>input[type=range] {width: 100%; margin: 20px 0;} ";
  html += "body {text-align: center; font-family: Arial, sans-serif;}</style></head><body>";
  html += "<h1>Donkov Ovladac Serva</h1>";
  html += "<p>Moja aktualna pozicia: <span id='position'>" + String(servoPos) + "</span> stupnov</p>";
  html += "<input type='range' min='0' max='180' value='" + String(servoPos) + "' class='slider' id='servoSlider'>";
  html += "<script>var slider = document.getElementById('servoSlider');";
  html += "slider.oninput = function() { fetch('/setPos?value=' + this.value); };";
  html += "setInterval(function() { fetch('/status').then(response => response.text()).then(data => {";
  html += "document.getElementById('position').innerText = data; slider.value = data; }); }, 1000);</script>";
  
  html += "<h2>Checkout GUIDE Link here</h2>";
  html += "<h3>https://github.com/Fattcat/esp8266-01-ServoTests</h3>";
  html += "<h3>Thank You For using this code :D</h3>";
  
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleSetPos() {
  if (server.hasArg("value")) {
    servoPos = server.arg("value").toInt();
    if (servoPos >= 0 && servoPos <= 180) {
      myServo.write(servoPos);
    }
  }
  server.send(204); // Žiadna odpoveď (No Content)
}

void handleStatus() {
  server.send(200, "text/plain", String(servoPos));
}
