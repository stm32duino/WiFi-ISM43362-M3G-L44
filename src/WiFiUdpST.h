/**
  ******************************************************************************
  * @file    WiFiUdpST.h
  * @author  Wi6labs
  * @brief   Header for Udp functions
  ******************************************************************************
  * Copyright (c) <2017>, <Wi6labs>
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted provided that the following conditions are met:
  *     * Redistributions of source code must retain the above copyright
  *       notice, this list of conditions and the following disclaimer.
  *     * Redistributions in binary form must reproduce the above copyright
  *       notice, this list of conditions and the following disclaimer in the
  *       documentation and/or other materials provided with the distribution.
  *     * Neither the name of the wi6labs nor the
  *       names of its contributors may be used to endorse or promote products
  *       derived from this software without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL WI6LABS BE LIABLE FOR ANY
  * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *****************************************************************************
*/

#ifndef WIFIUDP_H
#define WIFIUDP_H

#include <Udp.h>

#define UDP_TX_PACKET_MAX_SIZE 24

class WiFiUDP : public UDP {
  private:
    uint8_t _sock;  // socket ID
    uint16_t _port; // local port to listen on

  public:
    WiFiUDP();
    virtual uint8_t begin(uint16_t);
    virtual void stop();
    virtual int beginPacket(IPAddress ip, uint16_t port);
    virtual int beginPacket(const char *host, uint16_t port);
    virtual int endPacket();
    virtual size_t write(uint8_t);
    virtual size_t write(const uint8_t *buffer, size_t size);
    using Print::write;
    virtual int parsePacket();
    virtual int available();
    virtual int read();
    virtual int read(unsigned char *buffer, size_t len);
    virtual int read(char *buffer, size_t len)
    {
      return read((unsigned char *)buffer, len);
    };
    virtual int peek();
    virtual void flush();
    virtual IPAddress remoteIP();
    virtual uint16_t remotePort();
};

#endif  //WIFIUDP_H

/******************* (C) COPYRIGHT 2017 Wi6labs ***********END OF FILE********/
