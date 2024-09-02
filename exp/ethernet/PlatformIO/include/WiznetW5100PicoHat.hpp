#pragma once

// encapsulate the basics of bringing up the Wiznet W5100 Pico Hat wired ethernet
// module with the Earle Philwhatever core, see
// https://arduino-pico.readthedocs.io/en/latest/ethernet.html

#include <W5100lwIP.h>

class Wiznet5100PicoHat
{
private:
    static Wiznet5100lwIP eth;

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

    static bool Begin()
    {
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

Wiznet5100lwIP Wiznet5100PicoHat::eth(17, SPI, 21); // use the interupt mode of the wiz chip.
