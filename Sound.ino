#include <WiFi.h>

// --- ตั้งค่า WiFi Hotspot / Router ---
const char* ssid     = "S24feK";
const char* password = "xqcz1045";

// --- ตั้งค่า Sound Sensor ---
#define SOUND_PIN 1  // ต่อขา AO ของโมดูลเสียงเข้าขา GPIO 1 (A1)

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(SOUND_PIN, INPUT);

  // เชื่อมต่อ Wi-Fi Network
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
            
            // อ่านค่าความดังเสียง Analog (0 - 4095)
            int soundRaw = analogRead(SOUND_PIN);
            int soundPercent = map(soundRaw, 0, 4095, 0, 100);

            // ส่ง HTTP Header
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html; charset=utf-8");
            client.println("Connection: close");
            client.println();
            
            client.println("<!DOCTYPE html><html>");
            client.println("<head>");
            client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<meta http-equiv=\"refresh\" content=\"0.5\">"); // อัปเดตทุก 0.5 วินาทีเพื่อความเรียลไทม์
            client.println("<title>ESP32-C3 Sound Detector</title>");
            client.println("<style>");
            client.println("html { font-family: Arial; text-align: center; background-color: #f0f2f5; }");
            client.println(".card { background: white; padding: 25px; margin: 15px auto; border-radius: 12px; width: 80%; max-width: 320px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); }");
            client.println("h1 { color: #333; }");
            client.println(".sound-val { font-size: 2.5rem; font-weight: bold; color: #ff9800; margin: 10px 0; }");
            client.println("</style>");
            client.println("</head>");
            client.println("<body>");
            client.println("<h1>ESP32-C3 Sound Sensor</h1>");
            
            client.println("<div class=\"card\">");
            client.println("<h3>ระดับความดังเสียง</h3>");
            client.println("<p class=\"sound-val\">" + String(soundPercent) + " %</p>");
            client.println("<small style=\"color:#888;\">Raw ADC: " + String(soundRaw) + " / 4095</small>");
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