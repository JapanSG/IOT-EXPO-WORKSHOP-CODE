#include <WiFi.h>

// --- ตั้งค่า WiFi Hotspot / Router ---
const char* ssid     = "S24feK";
const char* password = "xqcz1045";

// --- ตั้งค่า Light Sensor (DO) ---
#define LDR_PIN 1  // ต่อขา DO ของโมดูลเข้าขา GPIO 1

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LDR_PIN, INPUT);

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
            
            // อ่านค่าสัญญาณดิจิทัล (HIGH / LOW)
            int ldrState = digitalRead(LDR_PIN);
            
            // แปลงค่าสถานะ (โมดูลส่วนใหญ่: LOW = มีแสง / HIGH = มืด)
            String statusText = "";
            String statusColor = "";
            
            if (ldrState == LOW) {
              statusText = "มีแสงสว่าง (LIGHT)";
              statusColor = "#28a745"; // สีเขียว
            } else {
              statusText = "มืด (DARK)";
              statusColor = "#dc3545"; // สีแดง
            }

            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html; charset=utf-8");
            client.println("Connection: close");
            client.println();
            
            client.println("<!DOCTYPE html><html>");
            client.println("<head>");
            client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<meta http-equiv=\"refresh\" content=\"1\">"); // อัปเดตทุก 1 วินาที
            client.println("<title>ESP32-C3 Light Switch</title>");
            client.println("<style>");
            client.println("html { font-family: Arial; text-align: center; background-color: #f0f2f5; }");
            client.println(".card { background: white; padding: 30px 20px; margin: 20px auto; border-radius: 15px; width: 80%; max-width: 320px; box-shadow: 0 4px 10px rgba(0,0,0,0.1); }");
            client.println("h1 { color: #333; }");
            client.println(".status { font-size: 1.5rem; font-weight: bold; color: white; padding: 12px; border-radius: 10px; display: block; margin-top: 15px; }");
            client.println("</style>");
            client.println("</head>");
            client.println("<body>");
            client.println("<h1>ESP32-C3 Light Detector</h1>");
            
            client.println("<div class=\"card\">");
            client.println("<h3>สถานะตรวจจับแสง</h3>");
            client.println("<span class=\"status\" style=\"background-color:" + statusColor + ";\">" + statusText + "</span>");
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