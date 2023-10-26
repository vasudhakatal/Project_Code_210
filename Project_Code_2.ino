#include <WiFiNINA.h>
#include <ThingSpeak.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// WiFi credentials
char ssid[] = "Shailly";       // Replace with your WiFi SSID
char pass[] = "arshya1997";   // Replace with your WiFi password

// Raspberry Pi's IP address and port
char raspberryPiIP[] = "192.168.1.12";  // Replace with the Raspberry Pi's IP address
int raspberryPiPort = 12345;            // Replace with the port on which Raspberry Pi is listening

// ThingSpeak settings
char thingSpeakApiKey[] = "J6UNQ2GFCDOVVXRP";  // Replace with your ThingSpeak API Key
unsigned long myChannelNumber = 2312000; // Replace with your ThingSpeak Channel Number

// Pin configuration
int sensorPin = A0;              // Analog pin for turbidity sensor
#define ONE_WIRE_BUS 2           // Pin for DS18B20 sensor
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

WiFiClient client;
WiFiClient raspberryPiClient;

void setup() {
  Serial.begin(115200);

  // Initialize DS18B20 sensor
  sensors.begin();
  pinMode(sensorPin, INPUT);

  // Connect to WiFi
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  int sensorValue = analogRead(sensorPin);
  int turbidity = map(sensorValue, 0, 750, 0, 100); // Adjust the mapping to match your sensor's specifications

  Serial.print("Turbidity: ");
  Serial.println(turbidity);

  sensors.requestTemperatures(); // Send the command to get temperatures
  float temperature = sensors.getTempCByIndex(0); // Get the temperature in Celsius
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  // Send data to ThingSpeak
  ThingSpeak.begin(client);
  ThingSpeak.setField(1, turbidity);
  ThingSpeak.setField(2, temperature);
  int response = ThingSpeak.writeFields(myChannelNumber, thingSpeakApiKey);

  if (response == 200) {
    Serial.println("Data sent to ThingSpeak successfully");
  } else {
    Serial.println("Data sending to ThingSpeak failed");
  }

  // Connect to the Raspberry Pi and send data
  if (client.connect(raspberryPiIP, raspberryPiPort)) {
    Serial.println("Connected to Raspberry Pi");
    String dataToSend = "Turbidity: " + String(turbidity) + ", Temperature: " + String(temperature);
    client.println(dataToSend);
    client.stop();
    Serial.println("Data sent to Raspberry Pi");
  } else {
    Serial.println("Data sending to Raspberry Pi failed");
  }

  delay(1500); // Send data to ThingSpeak and Raspberry Pi every 15 seconds (adjust as needed)
}