#include <WiFi.h>

// --- ตั้งค่า WiFi Hotspot / Router ---
const char* ssid     = "S24feK";
const char* password = "xqcz1045";

// --- ตั้งค่า Ultrasonic Sensor Pins ---
#define TRIG_PIN 2  // ขา Trig ต่อเข้า GPIO 2
#define ECHO_PIN 3  // ขา Echo ต่อเข้า GPIO 3

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(1000);

  // ตั้งค่า Pin Mode สำหรับ Ultrasonic
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

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

// ฟังก์ชันอ่านระยะทางจาก Ultrasonic (หน่วย: เซนติเมตร)
float readDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // อ่านระยะเวลาที่คลื่นเดินทางกลับ (Microseconds)
  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // Timeout 30ms

  if (duration == 0) {
    return -1.0; // หากอ่านค่าไม่ได้หรือเกินระยะ
  }

  // คำนวณความเร็วเสียง (343 m/s -> 0.0343 cm/us)
  return (duration * 0.0343) / 2.0;
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
            
            // อ่านค่าระยะทาง
            float distanceCm = readDistanceCM();
            float distanceInch = distanceCm / 2.54;

            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html; charset=utf-8");
            client.println("Connection: close");
            client.println();
            
            client.println("<!DOCTYPE html><html>");
            client.println("<head>");
            client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<meta http-equiv=\"refresh\" content=\"1\">"); // รีเฟรชอัปเดตทุก 1 วินาที
            client.println("<title>ESP32-C3 Ultrasonic</title>");
            client.println("<style>");
            client.println("html { font-family: Arial; text-align: center; background-color: #f0f2f5; }");
            client.println(".card { background: white; padding: 25px; margin: 15px auto; border-radius: 12px; width: 80%; max-width: 320px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); }");
            client.println("h1 { color: #333; }");
            client.println(".dist-val { font-size: 2.2rem; font-weight: bold; color: #007bff; margin: 10px 0; }");
            client.println(".sub-val { font-size: 1.2rem; color: #6c757d; }");
            client.println("</style>");
            client.println("</head>");
            client.println("<body>");
            client.println("<h1>Ultrasonic Distance Dashboard</h1>");
            
            client.println("<div class=\"card\">");
            client.println("<h3>ระยะทางที่ตรวจจับได้</h3>");
            
            if (distanceCm < 0) {
              client.println("<p style=\"color:red; font-size:1.2rem;\">Out of Range / Error</p>");
            } else {
              client.println("<p class=\"dist-val\">" + String(distanceCm, 1) + " cm</p>");
              client.println("<p class=\"sub-val\">(" + String(distanceInch, 1) + " inches)</p>");
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