#include <WiFi.h>

// --- ตั้งค่า WiFi Hotspot / Router ---
const char* ssid     = "S24feK";
const char* password = "xqcz1045";

// --- ตั้งค่า PIR Motion Sensor ---
#define PIR_PIN 1  // ต่อขา OUT ของ PIR เข้าขา GPIO 1

WiFiServer server(80);
unsigned long lastMotionTime = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(PIR_PIN, INPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.println("Connected!");
  Serial.print(">> IP Address: http://");
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
            
            // อ่านค่าการตรวจจับความเคลื่อนไหว (HIGH = เจอ / LOW = ไม่เจอ)
            int pirState = digitalRead(PIR_PIN);
            
            if (pirState == HIGH) {
              lastMotionTime = millis();
            }

            String statusText = (pirState == HIGH) ? "MOTION DETECTED!" : "Clear (No Motion)";
            String statusColor = (pirState == HIGH) ? "#dc3545" : "#28a745";

            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html; charset=utf-8");
            client.println("Connection: close");
            client.println();
            
            client.println("<!DOCTYPE html><html>");
            client.println("<head>");
            client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<meta http-equiv=\"refresh\" content=\"1\">"); // อัปเดตทุก 1 วินาที
            client.println("<title>ESP32-C3 PIR Sensor</title>");
            client.println("<style>");
            client.println("html { font-family: Arial; text-align: center; background-color: #f0f2f5; }");
            client.println(".card { background: white; padding: 25px; margin: 20px auto; border-radius: 12px; width: 80%; max-width: 320px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); }");
            client.println("h1 { color: #333; }");
            client.println(".status { font-size: 1.4rem; font-weight: bold; color: white; padding: 12px; border-radius: 8px; display: block; margin-top: 15px; }");
            client.println("</style>");
            client.println("</head>");
            client.println("<body>");
            client.println("<h1>ESP32-C3 Motion Monitor</h1>");
            
            client.println("<div class=\"card\">");
            client.println("<h3>สถานะความเคลื่อนไหว</h3>");
            client.println("<span class=\"status\" style=\"background-color:" + statusColor + ";\">" + statusText + "</span>");
            
            if (lastMotionTime > 0) {
              long secondsAgo = (millis() - lastMotionTime) / 1000;
              client.println("<p style=\"color:#6c757d; margin-top:15px;\">พบการเคลื่อนไหวล่าสุด: " + String(secondsAgo) + " วินาทีที่แล้ว</p>");
            }
            
            client.println("</div>");
            client.println("</body></html>");
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