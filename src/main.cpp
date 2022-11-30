#include <DHT.h>         // Library Sensor DHT11
#include <ESP8266WiFi.h> // Library NodeMCU ESP8266

String apiKey = "QJYE1VMKO9UOCW2O"; //  API thingspeak untuk dapat mengakses data

const char *ssid = "rumahjamur"; // SSID dan pass jaringan internet
const char *pass = "25102000";
const char *server = "api.thingspeak.com"; // server thingspeak

#define DHTPIN 0 // DHT11 pin D3 / GPIO0

DHT dht(DHTPIN, DHT11);

WiFiClient client; // deklarasi variabel data yang akan dikirim ke thingspeak

void setup()
{
  Serial.begin(115200);
  delay(10);
  dht.begin();

  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Please wait...");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void loop()
{

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // kalibrasi sensor DHT11
  t = t - 2.5; // kalibrasi temperatur
  h = h - 7; // kalibrasi kelembapan

  if (isnan(h) || isnan(t))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  if (client.connect(server, 80)) //   "184.106.153.149" or api.thingspeak.com
  {

    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(t);
    postStr += "&field2=";
    postStr += String(h);
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);

    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" ℃ , Humidity: ");
    Serial.print(h);
    Serial.println(" %. Send to Thingspeak.");
  }
  client.stop();

  Serial.println("Waiting...");

  // min 30 detik untuk jeda update thingspeak
  delay(1800); // 30 menit
}