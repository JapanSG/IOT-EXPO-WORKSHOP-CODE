#include <WiFi.h>

const char* ssid = "JANE";
const char* password = "0649733742";

#define SOUND_PIN 1  // ย้ายจาก GPIO 0 มาใช้ GPIO 1 เพื่อหลีกเลี่ยงขา Boot

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  pinMode(SOUND_PIN, INPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  int soundRaw = analogRead(SOUND_PIN);

  WiFiClient client = server.available();
  if (client) {
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n");
            client.println("<!DOCTYPE html><html><head><meta http-equiv=\"refresh\" content=\"1\">");
            client.println("<style>body{font-family:sans-serif;background:#0f172a;color:#fff;display:grid;place-content:center;height:100vh;margin:0;}");
            client.println(".card{background:#1e293b;padding:2rem;border-radius:1rem;text-align:center;}</style></head><body>");
            client.printf("<div class=\"card\"><h1>Sound Sensor</h1><div style=\"font-size:3.5rem;color:#38bdf8;font-weight:bold;\">%d</div></div></body></html>", soundRaw);
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
  }
  delay(50);
}