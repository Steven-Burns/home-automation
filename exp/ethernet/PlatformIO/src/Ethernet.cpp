#include <Arduino.h>
#include "WiznetW5100PicoHat.hpp"

// The rate at which we emit serial data depends on the wire and the board, so parameterize it.
const unsigned DEBUG_BIT_RATE = 115200;



void crash()
{
  Serial.println("CRASH!");
  for (;;)
  {
    delay(1000);
  }
}

void setup()
{
  Serial.begin(DEBUG_BIT_RATE);
  pinMode(LED_BUILTIN, OUTPUT);

  delay(2000);

  Wiznet5100PicoHat::Setup();

  // Start the Ethernet port -- this will retry connecting forever.
  if (!Wiznet5100PicoHat::BeginAndConnect())
  {
    crash();
  }
}

const char *host = "djxmmx.net";
const uint16_t port = 17;

static uint32_t loopCount = 0;
void loop()
{
  loopCount++;
  // Serial.print("loopCount "); Serial.println(loopCount);

  static bool wait = false;

  Serial.print("connecting to ");
  Serial.print(host);
  Serial.print(':');
  Serial.println(port);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(host, port))
  {
    Serial.println("connection failed");
    delay(5000);
    return;
  }

  // This will send a string to the server
  Serial.println("sending data to server");
  if (client.connected())
  {
    client.println("hello from RP2040");
  }

  // wait for data to be available
  unsigned long timeout = millis();
  while (client.available() == 0)
  {
    if (millis() - timeout > 5000)
    {
      Serial.println(">>> Client Timeout !");
      client.stop();
      delay(60000);
      return;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  Serial.println("receiving from remote server");
  // not testing 'client.connected()' since we do not need to send data here
  while (client.available())
  {
    char ch = static_cast<char>(client.read());
    Serial.print(ch);
  }
  Serial.println();

  Serial.print("packets recd: "); Serial.println(Wiznet5100PicoHat::PacketsRecevied());
  Serial.print("packets sent: "); Serial.println(Wiznet5100PicoHat::PacketsSent());

  // Close the connection
  Serial.println();
  Serial.println("closing connection");
  client.stop();

  if (wait)
  {
    delay(150000); // don't flood remote service
  }
  wait = true;
}
