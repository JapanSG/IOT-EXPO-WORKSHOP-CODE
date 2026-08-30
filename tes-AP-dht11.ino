#include <WiFi.h>
#include <DHT.h>

// Set your desired Access Point name and password
const char* ssid = "ESP32C3_AP";
const char* password = "12345678";

// DHT Sensor setup
#define DHTPIN 5       // GPIO pin connected to DHT11 DATA pin
#define DHTTYPE DHT11   // DHT 11 sensor type
DHT dht(DHTPIN, DHTTYPE);

// Set web server port number to 80
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Initialize the DHT sensor
  dht.begin();

  // Configure Access Point
  Serial.println("Setting up Access Point...");
  WiFi.softAP(ssid, password);

  // Get and print the IP address
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  // Start the server
  server.begin();
  Serial.println("Server started");
}

void loop() {
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects
    Serial.println("New Client Connected.");
    String currentLine = "";                // Make a string to hold incoming data from the client
    
    while (client.connected()) {            // Loop while the client's connected
      
      if (client.available()) {             // If there's bytes to read from the client
        char c = client.read();             // Read a byte
        Serial.write(c);                    // Print it to the serial monitor
        
        if (c == '\n') {                    // If the byte is a newline character
          if (currentLine.length() == 0) {
            // Read temperature and humidity readings
            float h = dht.readHumidity();
            float t = dht.readTemperature(); // Reading temperature in Celsius
            Serial.println(t);
            Serial.println("test");
            

            // HTTP headers always start with a response code
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><title>ESP32-C3 Environment Monitor</title></head>");
            client.println("<body style=\"font-family: Arial; text-align: center; margin-top: 50px;\">");
            client.println("<h1>ESP32-C3 DHT11 Readings</h1>");
            
            // Handle failed sensor reads gracefully
            if (isnan(h) || isnan(t)) {
              client.println("<p style=\"color: red;\">Failed to read from DHT sensor!</p>");
            } else {
              client.println("<p><strong>Temperature:</strong> " + String(t, 1) + " &deg;C</p>");
              client.println("<p><strong>Humidity:</strong> " + String(h, 1) + " %</p>");
            }
            
            client.println("</body></html>");
            
            // Break out of the while loop
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += symbolOrChar(c); // Add it to the end of the currentLine
        }
      }
    }
    client.stop(); // Close the connection
    Serial.println("Client Disconnected.");
  }
}

// Helper to handle characters safely
char symbolOrChar(char c) {
  return c;
}