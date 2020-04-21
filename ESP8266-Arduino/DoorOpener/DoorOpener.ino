/**
 * @file DoorOpener.ino
 * 
 * @brief ESP8266 door opener http server
 * 

  IMPORTANT NOTES ABOUT SSL CERTIFICATES

  1. USE/GENERATE YOUR OWN CERTIFICATES
    While a sample, self-signed certificate is included in this example,
    it is ABSOLUTELY VITAL that you use your own SSL certificate in any
    real-world deployment.  Anyone with the certificate and key may be
    able to decrypt your traffic, so your own keys should be kept in a
    safe manner, not accessible on any public network.

  2. HOW TO GENERATE YOUR OWN CERTIFICATE/KEY PAIR
    A sample script, "make-self-signed-cert.sh" is provided in the
    ESP8266WiFi/examples/WiFiHTTPSServer directory.  This script can be
    modified (replace "your-name-here" with your Organization name).  Note
    that this will be a *self-signed certificate* and will *NOT* be accepted
    by default by most modern browsers.  They'll display something like,
    "This certificate is from an untrusted source," or "Your connection is
    not secure," or "Your connection is not private," and the user will
    have to manully allow the browser to continue by using the
    "Advanced/Add Exception" (FireFox) or "Advanced/Proceed" (Chrome) link.

  Credits:
  Adapted by Earle F. Philhower, III, from the HelloServer.ino example.
  This example is released into the public domain.

  Project: Super Smart Home
  Cloud IoT Challenge Element14
  Author: Enrico Miglino
  Version: 0.1
  Date: Apr 2020
*/
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServerSecure.h>
#include <ESP8266mDNS.h>
#include "networkparams.h"

const char* ssid = STASSID;     ///< The WiFi SSID
const char* password = STAPSK;  ///< The SSID password

//! OTP sent by the client to accept the next command
String clientOTP = RESET_OTP;

//! Creates the server instance
ESP8266WebServerSecure server(443);

/**
 * The certificate is stored in PMEM created with the shell command generator
 * 
 */
static const uint8_t x509[] PROGMEM = {
#include "x509.h"
};

/**
 * The key is stored in PMEM created with the shell command generator
 * 
 */
static const uint8_t rsakey[] PROGMEM = {
#include "key.h"
};

/** 
 * Server response handler when calling the server IP address without parameters
 * 
 * If called from a browser send an http 200 with the current opener version
 */
void handleRoot() {
  server.send(200, "text/plain", DOOR_VERSION);
}

/**
 * Server response handler when the calling API does not exists
 * 
 * Generates an http 404 error
 */
void handleNotFound() {

  String message = ERROR404;
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

/**
 * Server response when the calling API jas some error
 * 
 * Generates an http 404 error without details. Only the
 * error code is shown
 */
void serverError(String apiErrorCode) {

  String message = ERROR404;
  message += apiErrorCode;
  server.send(404, "text/plain", message);
}

/**
 * Receives a One Time Password from the client via GET and save it for the next
 * command. The server URI is in the format /otp?123456 and should be the 
 * only argument passed to the GET
 */
void getOTP() {
  // Method should be GET
  if(server.method() == HTTP_GET){
    // Check for the API arguments, we expect only one param
    if(server.args() != 1) {
      // Wrong number of arguments
      serverError(ERR_ARGS);
    }
    else {
      // We consider only the first argument
      if(server.argName(0).equals(APIPARM_PASS)) {
        // Get the OTP password
        clientOTP = server.arg(0);
         server.send(200, "text/plain", OPERATION_COMPLETE);
         }
      else {
        // Wrong argument name
        serverError(ERR_PARM);
      }
    }
  }
  else {
    // Wrong method call
    serverError(ERR_METHOD);
  }
}

/** 
 * Open the door setting the relay for a delay
 */
void openDoor() {
  // Method should be GET
  if(server.method() == HTTP_GET){
    // Check for the API arguments, we expect only one param
    if(server.args() != 1) {
      // Wrong number of arguments
      serverError(ERR_ARGS);
    }
    else {
      // We consider only the first argument
      if(server.argName(0).equals(APIPARM_PASS)) {
        // Check for the OTP password
        if(clientOTP.equals(server.arg(0))){
          // Activate the door opener
          digitalWrite(OPENER_PIN, LOW);
          delay(OPENED_DELAY);
          digitalWrite(OPENER_PIN, HIGH);
          clientOTP = RESET_OTP;
          server.send(200, "text/plain", DOOR_OPEN);
        } else {
          // Wrong OTP password
          serverError(ERR_OTP);
        }
      }
      else {
        // Wrong argument name
        serverError(ERR_PARM);
      }
    }
  }
  else {
    // Wrong method call
    serverError(ERR_METHOD);
  }
}

/**
 * Server initialization
 */
void setup(void) {
  // Set the relay to initially open
  pinMode(OPENER_PIN, OUTPUT);
  digitalWrite(OPENER_PIN, HIGH);

  // Configure the nework with static IP
  WiFi.begin(ssid, password);
  IPAddress ip(192,168,1,150);   
  IPAddress gateway(192,168,1,1);   
  IPAddress subnet(255,255,255,0);   
  WiFi.config(ip, gateway, subnet);
  WiFi.mode(WIFI_STA);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  // Start the MDNS
  MDNS.begin("esp8266");

  // Initialize the server and implement the callbacks
  server.getServer().setServerKeyAndCert_P(rsakey, sizeof(rsakey), x509, sizeof(x509));
  server.on(API_ROOT, handleRoot);
  server.on(API_OPENDOOR, openDoor);
  server.on(API_OTP, getOTP);
  server.onNotFound(handleNotFound);

  // Start the server
  server.begin();
}

/** 
 *  Main loop processing the incoming https server requests
 */
void loop(void) {
  server.handleClient();
  MDNS.update();
}
