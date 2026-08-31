#include <WiFi.h>

const char* ssid = "JANE";
const char* password = "0649733742";

#define PIEZO_PIN 1

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
  Serial.print("IP: http://");
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
            int val = analogRead(PIEZO_PIN);

            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");
            client.println();

            client.println("<!DOCTYPE html><html><head>");
            client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<meta http-equiv=\"refresh\" content=\"1\">");
            client.println("<title>Piezo Sensor</title>");
            client.println("</head><body>");
            
            client.println("<h1>Piezo Sensor</h1>");
            client.print("<p>Value: ");
            client.print(val);
            client.println("</p>");

            if (val > 500) {
              client.println("<p style=\"color:red;\">Knock Detected!</p>");
            } else {
              client.println("<p style=\"color:gray;\">Normal</p>");
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
    client.stop();
  }
}