#include <WiFi.h>
#include <DHT.h>

// --- ตั้งค่า WiFi Hotspot / Router ---
const char* ssid     = "S24feK";
const char* password = "xqcz1045";

// --- ตั้งค่า DHT Sensor ---
#define DHTPIN 4       // ต่อสาย Data เข้า GPIO 4
#define DHTTYPE DHT11  // หากใช้ DHT22 ให้เปลี่ยนเป็น DHT22

DHT dht(DHTPIN, DHTTYPE);
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(1000);

  // เริ่มทำงานเซนเซอร์ DHT
  dht.begin();

  // เชื่อมต่อ Wi-Fi Network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.println("Connected!");
  Serial.print(">> เข้าดูหน้าเว็บผ่าน IP Address: http://");
  Serial.println(WiFi.localIP()); 
  
  server.begin();
}

void loop() {
  WiFiClient client = server.available();   // รอรับการเชื่อมต่อจาก Client (เบราว์เซอร์)

  if (client) {
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (currentLine.length() == 0) {
            
            // อ่านค่าอุณหภูมิและความชื้น
            float humidity = dht.readHumidity();
            float temperature = dht.readTemperature();

            // ส่ง HTTP Header
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html; charset=utf-8");
            client.println("Connection: close");
            client.println();
            
            // สร้างหน้าเว็บ HTML แสดงผลค่าเซนเซอร์
            client.println("<!DOCTYPE html><html>");
            client.println("<head>");
            client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<meta http-equiv=\"refresh\" content=\"3\">"); // รีเฟรชหน้าอัตโนมัติทุกๆ 3 วินาที
            client.println("<title>ESP32-C3 Dashboard</title>");
            client.println("<style>");
            client.println("html { font-family: Arial; text-align: center; background-color: #f0f2f5; }");
            client.println(".card { background: white; padding: 20px; margin: 15px auto; border-radius: 12px; width: 80%; max-width: 300px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); }");
            client.println("h1 { color: #333; }");
            client.println("p { font-size: 1.8rem; font-weight: bold; margin: 10px 0; }");
            client.println(".temp { color: #ff5722; }");
            client.println(".hum { color: #00bcd4; }");
            client.println("</style>");
            client.println("code>");
            client.println("<body>");
            client.println("<h1>ESP32-C3 Sensor Dashboard</h1>");
            
            // แสดงผลการอ่านค่า
            if (isnan(humidity) || isnan(temperature)) {
              client.println("<div class=\"card\"><p style=\"color:red;\">Error: อ่านค่าเซนเซอร์ไม่ได้</p></div>");
            } else {
              client.println("<div class=\"card\"><h3>อุณหภูมิ</h3><p class=\"temp\">" + String(temperature, 1) + " &deg;C</p></div>");
              client.println("<div class=\"card\"><h3>ความชื้น</h3><p class=\"hum\">" + String(humidity, 1) + " %</p></div>");
            }
            
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
    client.stop(); // ปิดการเชื่อมต่อ
  }
}