/**
  ******************************************************************************
  * @file    wl_definitions.h
  * @author  Wi6labs
  * @brief   WiFi definitions.
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

#ifndef WL_DEFINITIONS_H_
#define WL_DEFINITIONS_H_

#define WL_SSID_MAX_LENGTH 32          // Maximum size of a SSID
#define WL_WPA_KEY_MAX_LENGTH 63       // Length of passphrase. Valid lengths are 8-63.
#define WL_WEP_KEY_MAX_LENGTH 13       // Length of key in bytes. Valid values are 5 and 13.
#define WL_MAC_ADDR_LENGTH 6           // Size of a MAC-address or BSSID
#define WL_IPV4_LENGTH 4               // Size of a IPV4
#define WL_NETWORKS_LIST_MAXNUM 10     // Maximum size of a SSID list
#define MAX_SOCK_NUM  4                // Maximum number of socket
#define SOCK_NOT_AVAIL  255            // Socket not available constant
#define NA_STATE -1                    // Default state value for Wi-Fi state field
#define WL_MAX_ATTEMPT_CONNECTION 10 //Maximum number of attempts to establish Wi-Fi connection
#define WIFI_TIMEOUT  5000U            // Defines the Wi-Fi request timeout in milliseconds

typedef enum {
  WL_NO_SHIELD = 255,
  WL_IDLE_STATUS = 0,
  WL_NO_SSID_AVAIL,
  WL_SCAN_COMPLETED,
  WL_CONNECTED,
  WL_CONNECT_FAILED,
  WL_CONNECTION_LOST,
  WL_DISCONNECTED
} wl_status_t;

#endif /* WL_DEFINITIONS_H_ */
