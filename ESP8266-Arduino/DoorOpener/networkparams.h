/**
 * @file netowkrparams.h
 * 
 * Hardcoded network parameters to connect the server to the lan 
 * and other server messages
 */

#ifndef _NETWORK_PARAMS
#define _NETWORK_PARAMS

// Private WiFi access
#define STASSID "********"
#define STAPSK "********"

#define OPENER_PIN 0      ///< Pin to open the door
#define OPENED_DELAY 500  ///< ms needed to activate the solenoid to open the door

// Server messages
#define DOOR_VERSION "ESP8266 Door Opener V. 0.1.2"
#define ERROR404 "Error 404 - Door Opener\ninvalid request\n\n"
#define DOOR_OPEN "esp8266 Door is Open"
#define OPERATION_COMPLETE "Ok"

// OTP Password reset string
#define RESET_OTP "0000"

// Server error codes
#define ERR_ARGS "Error code 001\n\n"     ///< Wrong number of arguments
#define ERR_PARM "Error code 002\n\n"     ///< Wrong parameter name
#define ERR_METHOD "Error code 003\n\n"   ///< Wrong client method (should be GET)
#define ERR_OTP "Error code 004\n\n"      ///< Wrong OTP password

// Server get APIs
#define API_OPENDOOR "/opendoor"
#define API_ROOT "/"
#define API_OTP "/otp"

// Server parameter names
#define APIPARM_PASS "pass"

#endif
