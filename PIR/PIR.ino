#include <WiFi.h>

const char* ssid     = "JANE";
const char* password = "0649733742";

#define PIR_PIN 1

WiFiServer server(80);
unsigned long lastMotionTime = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(PIR_PIN, INPUT);

  // ตั้งค่า WiFi และเปิดระบบต่ออัตโนมัติของ ESP32
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  
  int counter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    counter++;
    
    // หากผ่านไป 20 วินาทีแล้วยังเชื่อมต่อไม่ติด ให้ปิดระบบ WiFi แล้วลองเริ่มใหม่แบบสะอาด
    if (counter > 40) {
      Serial.println("\nWiFi connection timed out. Resetting...");
      WiFi.disconnect(true, true);
      delay(1000);
      WiFi.begin(ssid, password);
      counter = 0;
    }
  }

  Serial.println("\nConnected!");
  Serial.print("IP Address: http://");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (currentLine.length() == 0) {
            int pirState = digitalRead(PIR_PIN);
            if (pirState == HIGH) lastMotionTime = millis();

            client.println("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n");
            client.println("<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<meta http-equiv=\"refresh\" content=\"1\"><title>PIR Sensor</title>");
            client.println("<style>body{font-family:sans-serif;background:#0f172a;color:#fff;display:flex;justify-content:center;align-items:center;min-height:100vh;margin:0;}");
            client.println(".card{background:#1e293b;padding:2rem;border-radius:1rem;text-align:center;width:85%;max-width:320px;}");
            client.println(".status{font-weight:700;padding:0.75rem;border-radius:0.5rem;display:block;}");
            client.println(".motion{background:#ef4444;}.clear{background:#10b981;}</style></head><body>");

            client.println("<div class=\"card\"><h1>Motion Monitor</h1>");
            if (pirState == HIGH) {
              client.println("<span class=\"status motion\">MOTION DETECTED!</span>");
            } else {
              client.println("<span class=\"status clear\">Clear (No Motion)</span>");
            }

            if (lastMotionTime > 0) {
              unsigned long secondsAgo = (millis() - lastMotionTime) / 1000;
              client.printf("<p style=\"color:#64748b;\">Last motion: %lu sec ago</p>", secondsAgo);
            }

            client.println("</div></body></html>");
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
}