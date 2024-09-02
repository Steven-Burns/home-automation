#include <Arduino.h>

// The rate at which we emit serial data depends on the wire and the board, so parameterize it.
const unsigned DEBUG_BIT_RATE = 115200;

#include <W5100lwIP.h>
Wiznet5100lwIP eth(17, SPI, 21); // use the interupt mode of the wiz chip.

void setup()
{
  // These are the pins for the wiznet 5100 pico hat, and also the w5100-evb-pico
  // https://arduino-pico.readthedocs.io/en/latest/ethernet.html#using-the-wiznet-w5100s-evb-pico
  SPI.setMISO(16);
  SPI.setCS(17);
  SPI.setSCK(18);
  SPI.setMOSI(19);

  Serial.begin(DEBUG_BIT_RATE);
  pinMode(LED_BUILTIN, OUTPUT);

  delay(2000);

  // Start the Ethernet port
  if (!eth.begin())
  {
    Serial.println("No wired Ethernet hardware detected. Check pinouts, wiring.");
    while (1)
    {
      delay(1000);
    }
  }

  while (!eth.connected())
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println();

  Serial.print("IP address: "); Serial.println(eth.localIP());
  Serial.print("Gateway   : "); Serial.println(eth.gatewayIP());
  Serial.print("DNS       : "); Serial.println(eth.dnsIP());
  Serial.print("Subnet    : "); Serial.println(eth.subnetMask());
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

  Serial.print("packets recd: "); Serial.println(eth.packetsReceived());
  Serial.print("packets sent: "); Serial.println(eth.packetsSent());

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
