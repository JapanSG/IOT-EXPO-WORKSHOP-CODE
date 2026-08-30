#include <WiFi.h>

const char* ssid     = "JAPAN-DELL-PC 3175";
const char* password = "9q[18L14";

#define PIEZO_PIN 1  // ขา Analog อ่านค่าการเคาะ (GPIO 1)

WiFiServer server(80);

void setup() {
  Serial.begin(9600);
  delay(1000);

  pinMode(PIEZO_PIN, INPUT);

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
            
            // อ่านค่าแรงดันการสั่นสะเทือน (0 - 4095)
            int val = analogRead(PIEZO_PIN);

            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html; charset=utf-8");
            client.println("Connection: close");
            client.println();
            
            client.println("<!DOCTYPE html><html>");
            client.println("<head>");
            client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<meta http-equiv=\"refresh\" content=\"0.5\">"); 
            client.println("<title>ESP32-C3 Piezo Sensor</title>");
            client.println("<style>");
            client.println("html { font-family: Arial; text-align: center; background-color: #f0f2f5; }");
            client.println(".card { background: white; padding: 25px; margin: 15px auto; border-radius: 12px; width: 80%; max-width: 320px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); }");
            client.println("h1 { color: #333; }");
            client.println(".val { font-size: 2.2rem; font-weight: bold; color: #e91e63; margin: 10px 0; }");
            client.println("</style>");
            client.println("</head>");
            client.println("<body>");
            client.println("<h1>Piezo Vibration Sensor</h1>");
            
            client.println("<div class=\"card\">");
            client.println("<h3>ระดับแรงเคาะ / สั่นสะเทือน</h3>");
            client.println("<p class=\"val\">" + String(val) + "</p>");
            if(val > 500) {
              client.println("<p style=\"color:red;\">● ตรวจพบการเคาะ!</p>");
            } else {
              client.println("<p style=\"color:gray;\">สถานะปกติ</p>");
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