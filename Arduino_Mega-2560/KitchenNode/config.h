/**
 * \file config.h
 * \brief KitchenNode configuration file.
 */

#ifndef _CONFIG
#define _CONFIG

//! Max6675 thermocouple sensor SPI Serial Output pin.
#define PIN_MAX6675_SO 8
//! Max6675 thermocouple sensor SPI Chip Select pin.
#define PIN_MAX6675_CS 9
//! Max6675 thermocouple sensor SPI CLocK pin.
#define PIN_MAX6675_CLK 10

//! Control pin for the gas valve
#define PIN_RELAY_GAS 22
//! Control pin for the cooking plate light
#define PIN_RELAY_LIGHT 23
//! Control pin for the cooking plate fume vacuum
#define PIN_RELAY_VACUUM 24

// Load cell weight sensor pins
#define PIN_HX711_DOUT 4
#define PIN_HX711_CLK 5

// Alarm signals
#define PIN_WATER_LEVEL_ALARM_LED 31
#define PIN_GAS_ALARM_LED 32
#define PIN_TEMPERATURE_ALARM_LED 33

// Activity signals
#define PIN_GAS_LED 34
#define PIN_WATER_LED 35
#define PIN_PRESENCE_LED 36
#define PIN_COOKING_LED 37
#define PIN_CALIBRATE_TARE_LED 38
#define PIN_CALIBRATE_FULL_LED 39

//! Manual control of the water pump
#define IRQ_MANUAL_PUMP_BTN 2
//! Tare calibration button
#define IRQ_SET_TARE_BTN 3
//! Full bowl calibration button
#define IRQ_SET_FULL_BTN 20
//! Save the configuration on EEPROM
#define IRQ_SAVE_CONFIG_BTN 21
//! Manual reset an alarm condition
#define ALARM_RESET_BTN 28

// Water pump L298 DC Motor controller pins
#define MOTOR_ENABLE_PUMP 11
#define MOTOR_CONTROL_PUMP1 26
#define MOTOR_CONTROL_PUMP2 27

//! Analog input used to calibrate the pump
//! motor speed
#define PUMP_SPEED_TRIMMER A0
//! Analog input connected to the gas sensor
#define GAS_READ_PPM A1
//! Digital trigger for gas presence
#define GAS_TRIGGER 25

//! Ultrasound human presence detector based on HC-SR04
//! Note that the trigger emits a frequence so needs a PWM pin
#define HCSR04_PRESENCE_TRIG 40
#define HCSR04_PRESENCE_ECHO 41

//! Ultrasound cooking presence detector based on HC-SR04
#define HCSR04_COOKING_TRIG 42
#define HCSR04_COOKING_ECHO 43

#define EEPROM_ADDRESS 0X00  ///< EEPROM address to read/write the configuration variables

//! Max human presence distance (cm) to detect someone near the
//! cooking plate
#define MAX_HUMAN_DISTANCE 90
//! Max cooking distance (cm) to detect something over the
//! cooking plate
#define MAX_COOKING_DISTANCE 25
//! Define absolute max ppm the gas sensor is able to detect
#define GAS_SENSOR_MAX 10000
//! Define absolute min ppm the gas sensor is able to detect
#define GAS_SENSOR_MIN 200
//! Min alert gas ppm
#define GAS_SENSOR_ALERT 800
//! Max temperature of the cooking plate before checking for presence
//! of something, powering on the vapor vacuum or raising an alarm.
#define GAS_RISK_TEMPERATURE 100

/**
 * Configuration parameters, saved as single object to the EEPROM
 */
struct PersistentConfiguration {
  float emptyBowl;  ///< Weight of the empy bowl (can not be 0)
  float fullBowl;   ///< Weight of the full bowl (can not be 0)
};

/**
 * This structure defines all the parameters controlled by the
 * node. At any moment it is possible to know the status of the
 * kitchen environment, alarms and working conditions.
 */
struct KitchenStatus {
    //! General alarm condition. Associated to one
    //! or more alarm status flags.
    boolean isAlarm = false;
    //! Water pump motor speed.
    int pumpSpeed = 0;
    //! Water pump status
    boolean isWaterPumpRunning = false;
    //! Gas valve status
    boolean isGasOpen = false;
    //! Vapor vacuum status
    boolean isVacuum = false;
    //! cooking plate light
    boolean isLight = false;
    //! Water level in grams
    int waterLevel = 0;
    //! Cooking light status
    boolean isCoookingLightOn = false;
    //! Cooking plate temperature (Celsius)
    int tempCookingPlate;
    //! Gas detection flag
    boolean isGas = false;
    //! Gas ppm
    float gasPPM = 0;
    //! Status of the cooking plate (with or without pans)
    boolean isCookingPlateEmpty = true;
    //! Status of humans in front of the cooking plate
    boolean isChefPresent = false;
    //! Flag indicating when a button ISR is running to logically stop
    //! any other IRQ activity
    boolean isISR_Running = false;
};

#endif
