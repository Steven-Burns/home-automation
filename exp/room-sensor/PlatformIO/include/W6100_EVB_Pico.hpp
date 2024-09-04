#pragma once

// encapsulate the basics of bringing up the Wiznet W5100 Pico Hat wired ethernet
// module with the Earle Philwhatever core, see
// https://arduino-pico.readthedocs.io/en/latest/ethernet.html

#include <W6100lwIP.h>

class W6100_EVB_Pico
{
private:
    static Wiznet6100lwIP eth;

public:
    static void Setup()
    {
        // These are the pins for the wiznet 5100 pico hat, and also the w5100-evb-pico board
        // https://arduino-pico.readthedocs.io/en/latest/ethernet.html#using-the-wiznet-w5100s-evb-pico
        SPI.setMISO(16);
        SPI.setCS(17);
        SPI.setSCK(18);
        SPI.setMOSI(19);
    }

    static bool BeginAndConnect()
    {
        if (!W6100_EVB_Pico::Begin())
        {
            Serial.println("No wired Ethernet hardware detected. Check pinouts, wiring.");
            return false;
        }

        while (!W6100_EVB_Pico::Connected())
        {
            Serial.print(".");
            delay(500);
        }
        Serial.println("Wired Ethernet w/IP Connected.");

        Serial.print("IP address: "); Serial.println(W6100_EVB_Pico::LocalIP());
        Serial.print("Gateway   : "); Serial.println(W6100_EVB_Pico::GatewayIP());
        Serial.print("DNS       : "); Serial.println(W6100_EVB_Pico::DNSIP());
        Serial.print("Subnet    : "); Serial.println(W6100_EVB_Pico::SubnetMask());
        return true;
    }

    static bool Begin()
    {
        if (!eth.hostname("arkadia-pico-02"))
        {
            Serial.println("Could not set hostname");
        }
        
        // TODO: someday, play with this
        // https://arduino-pico.readthedocs.io/en/latest/ethernet.html#adjusting-spi-speed
        // eth.setSPISpeed(????);

        return eth.begin();
    }

    static bool Connected()
    {
        return eth.connected();
    }

    static IPAddress LocalIP()
    {
        return eth.localIP();
    }

    static IPAddress SubnetMask()
    {
        return eth.subnetMask();
    }

    static IPAddress DNSIP()
    {
        return eth.dnsIP();
    }

    static IPAddress GatewayIP()
    {
        return eth.gatewayIP();
    }

    static uint32_t PacketsRecevied()
    {
        return eth.packetsReceived();
    }

    static uint32_t PacketsSent()
    {
        return eth.packetsSent();
    }
};

Wiznet6100lwIP W6100_EVB_Pico::eth(17, SPI, 21); // use the interupt mode of the wiz chip.
