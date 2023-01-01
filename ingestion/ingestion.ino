#include <OBD2.h>
#include <ArduinoJson.h>
#include <Ethernet.h>

// Results: https://webhook.site/#!/b45d93df-9567-4db4-b0cb-bda9edb84012/83d55a4e-9a74-4e4b-85d0-8fa6807e1dae/1
const char *API_ENDPOINT = "https://webhook.site/b45d93df-9567-4db4-b0cb-bda9edb84012";
EthernetClient client;
OBD2 obd;

enum class OBDCommand
{
  RPM = "010C",
  SPEED = "010D",
  COOLANT_TEMP = "0105",
  FUEL_LEVEL = "012F"
};

struct OBDData
{
  int rpm;
  float speed;
  int coolantTemp;
  int fuelLevel;
};

void setup()
{
  Serial.begin(9600);
  Ethernet.begin(mac);
  obd.begin();
}

void loop()
{
  OBDData data;
  OBD2Response response = obd.sendCommand(static_cast<char *>(OBDCommand::RPM));
  data.rpm = response.isValid() ? response.getIntValue() : 0;
  response = obd.sendCommand(static_cast<char *>(OBDCommand::SPEED));
  data.speed = response.isValid() ? response.getFloatValue() : 0;
  response = obd.sendCommand(static_cast<char *>(OBDCommand::COOLANT_TEMP));
  data.coolantTemp = response.isValid() ? response.getIntValue() : 0;
  response = obd.sendCommand(static_cast<char *>(OBDCommand::FUEL_LEVEL));
  data.fuelLevel = response.isValid() ? response.getIntValue() : 0;

  const int BUFFER_SIZE = JSON_OBJECT_SIZE(4);
  StaticJsonDocument<BUFFER_SIZE> doc;
  doc["rpm"] = data.rpm;
  doc["speed"] = data.speed;
  doc["coolantTemp"] = data.coolantTemp;
  doc["fuelLevel"] = data.fuelLevel;

  String json;
  serializeJson(doc, json);

  if (client.connect(API_ENDPOINT, 80))
  {
    // client.println("Host: domain.com");
    // client.println("POST /endpoint HTTP/1.1");
    client.println("POST /b45d93df-9567-4db4-b0cb-bda9edb84012 HTTP/1.1");
    client.println("Host: webhook.site");
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(json.length());
    client.println();
    client.print(json);
  }
  client.stop();

  // wait 5 seconds before sending the next request:
  delay(5000);
}
