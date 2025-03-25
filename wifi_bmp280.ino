#include <WiFi.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>

const char* ssid = "KhMrMo";
const char* password = "M.r.k.h.1113.";

Adafruit_BMP280 bmp;

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); 

  if (!bmp.begin(0x76)) { 
    Serial.print("BMP280 Error!");
    while (1); 
  }

  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,    
                  Adafruit_BMP280::SAMPLING_X2,    
                  Adafruit_BMP280::SAMPLING_X16,    
                  Adafruit_BMP280::FILTER_X16,      
                  Adafruit_BMP280::STANDBY_MS_500);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nConnected to Wi-Fi!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  server.begin();                

}


void loop() {
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  Serial.println("New Client Connected!");
  String request = client.readStringUntil('\r');  
  Serial.println(request);
  client.flush();



  if (request.indexOf("GET / ") != -1) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println("Connection: close");
    client.println();
    
    client.println("<!DOCTYPE html><html><head>");
    client.println("<title>ESP32 Sensor Dashboard</title>");
    client.println("<style>");
    client.println("body { font-family: Arial, sans-serif; text-align: center; background-color: #f4f4f4; }");
    client.println("h2 { color: #333; }");
    client.println("p { font-size: 20px; }");
    client.println("span { font-weight: bold; color: #007bff; }");
    client.println("</style>");

    client.println("<script>");
    client.println("setInterval(() => {");
    client.println(" fetch('/sensor')");  
    client.println(" .then(response => response.text())");
    client.println(" .then(data => {");
    client.println("   let values = data.split(',');");
    client.println("   document.getElementById('temp').innerText = values[0] + ' °C';");
    client.println("   document.getElementById('press').innerText = values[1] + ' hPa';");
    client.println("   document.getElementById('alt').innerText = values[2] + ' m';");
    client.println(" });");
    client.println("}, 1000);");  
    client.println("</script></head><body>");

    client.println("<h2>ESP32 Sensor Dashboard</h2>");
    client.println("<p>Temperature: <span id='temp'>Loading...</span></p>");
    client.println("<p>Pressure: <span id='press'>Loading...</span></p>");
    client.println("<p>Altitude: <span id='alt'>Loading...</span></p>");
    
    client.println("</body></html>");
    client.println();
  }

  
  if (request.indexOf("GET /sensor") != -1) {
    float temp = bmp.readTemperature();
    float press = bmp.readPressure() / 100.0;
    float alt = bmp.readAltitude(1013.25);

    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/plain");
    client.println("Connection: close");
    client.println();
    client.print(temp);
    client.print(",");
    client.print(press);
    client.print(",");
    client.print(alt);
    client.println();
  }

  client.stop();

}
