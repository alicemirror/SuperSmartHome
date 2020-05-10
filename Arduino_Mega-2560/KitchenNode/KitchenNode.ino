/**
 * \file KitchenNode.ino
 * \brief Arduino Mega Kitchen Control Node and Dog Water Dispenser
 * 
 * This is the Arduino Mega sketch that controls the alarma and feedback
 * of the kitchen node. The same microcontroller includes the water automatic
 * dispenser for the dog.
 * 
 * \versione 0.1.5
 * \date May 2020
 * \author Enrico Miglino <balearicdynamics@gmail.com>
 */

#include <max6675.h>
#include <EEPROM.h>
#include <Streaming.h>
#include <HCSR04.h>
#include "config.h"
#include "waterlevel.h"

//! Initialize the thermocouple library
MAX6675 ktc(PIN_MAX6675_CLK, PIN_MAX6675_CS, PIN_MAX6675_SO);

// Undef to disable the USB to serial debug
#define _DEBUG

//! Kitchen status structure
KitchenStatus ks;

//! HC-SR04 distance sensor for human presence detection
UltraSonicDistanceSensor distanceHuman(HCSR04_PRESENCE_TRIG, HCSR04_PRESENCE_ECHO);
//! HC-SR04 distance sensor for cooking activity detection
UltraSonicDistanceSensor distanceCooking(HCSR04_COOKING_TRIG, HCSR04_COOKING_ECHO);

//! EEPROM persistent configuratin parameters
PersistentConfiguration savedConfig;

WaterLevel bowl;

/**
 * Program initialization
 */
void setup() {
#ifdef _DEBUG
    Serial.begin(9600);
#endif

    // Set the relays pins
    pinMode(PIN_RELAY_GAS, OUTPUT);
    pinMode(PIN_RELAY_LIGHT, OUTPUT);
    pinMode(PIN_RELAY_VACUUM, OUTPUT);

    // Set the DC motor controller pins    
    pinMode(MOTOR_ENABLE_PUMP, OUTPUT);
    pinMode(MOTOR_CONTROL_PUMP1, OUTPUT);
    pinMode(MOTOR_CONTROL_PUMP2, OUTPUT);

    // Set the control and configuration buttons
    pinMode(IRQ_MANUAL_PUMP_BTN, INPUT);
    pinMode(IRQ_SET_TARE_BTN, INPUT);
    pinMode(IRQ_SET_FULL_BTN, INPUT);
    pinMode(IRQ_SET_FULL_BTN, INPUT);
    pinMode(ALARM_RESET_BTN, INPUT);

    // Set the LEDs pins
    pinMode(PIN_GAS_LED, OUTPUT);
    pinMode(PIN_WATER_LED, OUTPUT);
    pinMode(PIN_WATER_LEVEL_ALARM_LED, OUTPUT);
    pinMode(PIN_PRESENCE_LED, OUTPUT);
    pinMode(PIN_COOKING_LED, OUTPUT);
    pinMode(PIN_CALIBRATE_TARE_LED, OUTPUT);
    pinMode(PIN_CALIBRATE_FULL_LED, OUTPUT);

    // Gas detection trigger
    pinMode(GAS_TRIGGER, INPUT);

    // Initialize the four relays to off
    digitalWrite(PIN_RELAY_GAS, HIGH);
    digitalWrite(PIN_RELAY_LIGHT, HIGH);
    digitalWrite(PIN_RELAY_VACUUM, HIGH);

    testRelays();           // Test the four relays
    bowl.begin();           // Initialize the WaterLevel class  
    loadConfiguration();    // Load the configuration from the EEPROM

    // Enable the interrupt vectors assciated to the control buttons
    attachInterrupt(IRQ_MANUAL_PUMP_BTN, irq_manualWaterPump, LOW);
    attachInterrupt(IRQ_SET_TARE_BTN, irq_calibrateEmptyBowl, LOW);
    attachInterrupt(IRQ_SET_FULL_BTN, irq_calibrateFullBowl, LOW);
    attachInterrupt(IRQ_SAVE_CONFIG_BTN, irq_saveBowlCalibration, LOW);

    debug("Initialization Complete");
}

/**
 * Main loop of the kitchen control state machine
 * 
 * \note When the system has detected an alarm condition
 * all the snsors stop working until the alarm is not
 * manually reset.
 */
void loop() {
    if(ks.isAlarm == true) {
        // Only accept the reset alarm button
        if(digitalRead(ALARM_RESET_BTN) == LOW) {
            resetAlarms();
        } // Alarm reset
    } else {
        updateSensors();
        checkForAlarmsAndFeedback();
    }
}

// ******************************************************
// Sensors and alarms functions
// ******************************************************

/**
 * Update the sensors readings in the KitchenStatus
 * global object.
 */
void updateSensors() {
    double distance;
    
    // Read the current cooking plate temperature
    ks.tempCookingPlate = ktc.readCelsius();
    debug("Cooking plate temp. " + String(ks.tempCookingPlate, DEC) );
    // Read the distance of something over the cooking plate, if any
    // And set the flag accordingly
    distance = distanceCooking.measureDistanceCm();
    if(distance <= MAX_COOKING_DISTANCE) {
        // Something is over the cooking plate
        ks.isCookingPlateEmpty = false;
        debug("Cooking plate is empty");
    }
    
    // Read the presence of a person in front of the cooking plate
    distance = distanceHuman.measureDistanceCm();
    if(distance <= MAX_HUMAN_DISTANCE) {
        // Someone is near the cooking plate
        ks.isChefPresent = true;
        debug("Chef is present");
    }
    
    // Read the water level for the dog
    // Update all the water level parameters
    bowl.readScale();

    if(bowl.calibration == true) {
        debug("Bowl weight: " + String(bowl.currentWeight, DEC) +
                "\nBowl tare: " + String(bowl.bowlTare, DEC) +
                "\nBowl full: " + String(bowl.bowlFull, DEC) +
                "\nBowl perc: " + String(bowl.percWater, DEC) +
                "\nBowl is low: " + bowl.bowlLow);
    } else {
        debug("Bowl water level is not calibrated");
    }
    
    // Read the gas sensor status
    // If the gas trigger is on
    if(digitalRead(GAS_TRIGGER == HIGH)) {
        ks.isGas = true;
        ks.gasPPM = map(analogRead(GAS_READ_PPM), 0, 1023, GAS_SENSOR_MIN, GAS_SENSOR_MAX);
        debug("Gas detected! " + String(ks.gasPPM, DEC) + " ppm");
    } // Detected gas presence
    else {
        ks.isGas = false;
        ks.gasPPM = 0;  // Value non detectable
        debug("No gas detected");
    }
 }

/** 
 *  Check for alarms, depending on the interpolation of the
 *  sensors status.
 *  
 *  There are some sensors conditions that enable alarms to be
 *  forwarded to the control center and require to activate an
 *  immediate feedack.
 */
boolean checkForAlarmsAndFeedback() {
    // Check if the cooking plate temparature is over 100C
    if(ks.tempCookingPlate >= GAS_RISK_TEMPERATURE) {
        // Check if there is something over the cooking plate
        if(ks.isCookingPlateEmpty == false) {
            // If it is not yet active, power on the vapor vacuum
            if(ks.isVacuum == false) {
                setVacuum(true);
            } // Power on the aspirator
        } // Something is cooking... 
        else {
            if(ks.isVacuum == true) {
                setVacuum(false);
            } // Power off the aspirator
            setGasValve(false); // Forcely close the gas valve
            
        } // Need to power off the gas.
    } // Cooking plate temperature is in the risk zone
}

/**
 * Reset all the alarm conditions and restore the normal
 * operations.
 */
void resetAlarms() {
    
}

// ******************************************************
// Control and feedback functions
// ******************************************************

void setVacuum(boolean set) {
    if(set == true) {
        digitalWrite(PIN_RELAY_VACUUM, LOW);
        ks.isVacuum = true;
    } else {
        digitalWrite(PIN_RELAY_VACUUM, HIGH);
        ks.isVacuum = false;
    }
}

void setGasValve(boolean set) {
    if(set == true) {
        digitalWrite(PIN_RELAY_VACUUM, LOW);
        ks.isVacuum = true;
    } else {
        digitalWrite(PIN_RELAY_VACUUM, HIGH);
        ks.isVacuum = false;
    }
}

/**
 * Test the four relays in sequence for one second
 * in sequence
 * 
 * \note The realys are connected, respectively:\n
 * Relay 1 -> Cooking gas
 * Relay 2 -> Cooking light
 * Relay 3 -> Top cooking plate vacuum
 * Relay 4 -> NOT USED
 */
void testRelays() {
    digitalWrite(PIN_RELAY_GAS, LOW);
    delay(1000);
    digitalWrite(PIN_RELAY_GAS, HIGH);
    digitalWrite(PIN_RELAY_LIGHT, LOW);
    delay(1000);
    digitalWrite(PIN_RELAY_LIGHT, HIGH);
    digitalWrite(PIN_RELAY_VACUUM, LOW);
    delay(1000);
    digitalWrite(PIN_RELAY_VACUUM, HIGH);
}

/**
 * Set the water pump motor PWM speed, accordingly
 * to the trimmer value.
 * 
 * To be able to tst and calibrate the water pump
 * the speed should be read before the pump is started
 * 
 * \note The analog read value from the trimmer (10 bit)
 * is mapped to the PWM min and max values (5-255)
 */
void setPumpSpeed() {

    ks.pumpSpeed = map(analogRead(PUMP_SPEED_TRIMMER), 0, 1023, 5, 255);

    debug("Pump Speed " + String(ks.pumpSpeed, DEC));
}

/**
 * Enable or disable the water pump accordingly to the flag status
 * 
 * The pump speed is set to the current calibration trimmer value.
 * 
 * \param onOff When true the water pump is activated else it is set
 * to off.
 */
void pumpWater(boolean onOff) {

    if(onOff == true) {
        setPumpSpeed();
        analogWrite(MOTOR_ENABLE_PUMP, ks.pumpSpeed);
        digitalWrite(MOTOR_CONTROL_PUMP1, LOW);
        digitalWrite(MOTOR_CONTROL_PUMP2, HIGH);
        ks.isWaterPumpRunning = true;
        debug("Water pump running");
    } else {
        digitalWrite(MOTOR_CONTROL_PUMP1, LOW);
        digitalWrite(MOTOR_CONTROL_PUMP2, LOW);
        ks.isWaterPumpRunning = false;
        debug("Water pump stopped");
    }
}

/**
 * Print a debug string message on the serial console
 * if the debug is enabled.
 */
void debug(String message) {
#ifdef _DEBUG
   Serial << message << endl;
#endif
}

// ******************************************************
// Panel signal functions
// ******************************************************

/**
 * Update the panels status LEDs accordingly to the applicaiton
 * status.
 * 
 * 
 */
void updatePanelLEDs() {

}

/**
 * Set on or off the water alarm LED.
 * 
 * \param set can be HIGH or LOW 
 */
void setWaterAlarm(int set) {
    digitalWrite(PIN_WATER_LEVEL_ALARM_LED, set);
}

/**
 * Set on or off the gas alarm LED.
 * 
 * \param set can be HIGH or LOW 
 */
void setGasAlarm(int set) {
    digitalWrite(PIN_GAS_ALARM_LED, set);
}

/**
 * Set on or off the temperature alarm LED.
 * 
 * \param set can be HIGH or LOW 
 */
void setTemperatureAlarm(int set) {
    digitalWrite(PIN_TEMPERATURE_ALARM_LED, set);
}

/**
 * Set on or off the gas signal LED.
 * 
 * \param set can be HIGH or LOW 
 */
void setGasSignal(int set) {
    digitalWrite(PIN_GAS_LED, set);
}

/**
 * Set on or off the human presence signal LED.
 * 
 * \param set can be HIGH or LOW 
 */
void setPresenceSignal(int set) {
    digitalWrite(PIN_PRESENCE_LED, set);
}

/**
 * Set on or off the pan presence signal LED.
 * 
 * \param set can be HIGH or LOW 
 */
void setCookingSignal(int set) {
    digitalWrite(PIN_COOKING_LED, set);
}

/**
 * Set on or off the tare calibration status LED.
 * 
 * \param set can be HIGH or LOW 
 */
void setTareSignal(int set) {
    digitalWrite(PIN_CALIBRATE_TARE_LED, set);
}

/**
 * Set on or off the tare calibration status LED.
 * 
 * \param set can be HIGH or LOW 
 */
void setWaterFullSignal(int set) {
    digitalWrite(PIN_CALIBRATE_FULL_LED, set);
}

// ******************************************************
// Interrupt callback functions
// ******************************************************

/**
 * Make a reading of the load sensor then saves the value
 * to the persistent configuration structure.
 */
void irq_calibrateEmptyBowl() {
    detachInterrupt(IRQ_SET_TARE_BTN);
    // Check if no other ISR is running
    if(ks.isISR_Running == false) {
        ks.isISR_Running = true;
        setTareSignal(HIGH);
        savedConfig.emptyBowl = bowl.readScale();    
        ks.isISR_Running = false;
    } // No other ISR running
    attachInterrupt(IRQ_SET_TARE_BTN, irq_calibrateEmptyBowl, LOW);
}

/**
 * Make a reading of the load sensor then saves the value
 * to the persistent configuration structure.
 */
void irq_calibrateFullBowl() {
    detachInterrupt(IRQ_SET_FULL_BTN);
    // Check if no other ISR is running
    if(ks.isISR_Running == false) {
        ks.isISR_Running = true;
        setWaterFullSignal(HIGH);
        savedConfig.fullBowl = bowl.readScale();   
        ks.isISR_Running = false;
    } // No other ISR running
    attachInterrupt(IRQ_SET_FULL_BTN, irq_calibrateFullBowl, LOW);
}

/**
 * Stores permanently the last saved configuration values to the EEPROM
 * 
 * If the content of the saved configuration structure is not valid the
 * configuration is not saved at all.
 */
void irq_saveBowlCalibration() {
    detachInterrupt(IRQ_SAVE_CONFIG_BTN);
    // Check if no other ISR is running
    if(ks.isISR_Running == false) {
        ks.isISR_Running = true;
        saveConfiguration();
        ks.isISR_Running = false;
    } // No other ISR running
    attachInterrupt(IRQ_SAVE_CONFIG_BTN, irq_saveBowlCalibration, LOW);
}

/**
 * Activate the manual water pump until the button is pressed.
 */
void irq_manualWaterPump() {
    detachInterrupt(IRQ_MANUAL_PUMP_BTN);
    // Check if no other ISR is running
    if(ks.isISR_Running == false) {
        ks.isISR_Running = true;
        // Check for the manual water pump pushbutton.
        // Change the water pump status accordingly
        while(digitalRead(IRQ_MANUAL_PUMP_BTN) == LOW) {
            pumpWater(true);
        } // Pumping water ...
        pumpWater(false);
        // Restart the interrupts
        ks.isISR_Running = false;   
    } // No other ISR running
    attachInterrupt(IRQ_MANUAL_PUMP_BTN, irq_manualWaterPump, LOW);
}

// ******************************************************
// EEPROM functions
// ******************************************************
/**
* Read the last saved configuration stored in the EEPROM
* and set the WaterLevel class parameters configuration
*/
void loadConfiguration() {
    EEPROM.get(EEPROM_ADDRESS, savedConfig);
    // If the saved configuration contains calibration parameters
    // updates the WaterLevel class configuration
    if( (savedConfig.emptyBowl != 0) && 
        (savedConfig.fullBowl != 0) ) {
        bowl.calibrateTare(savedConfig.emptyBowl);
        bowl.calibrateFullBowl(savedConfig.fullBowl);
        bowl.setCalibration(true);
    } 
    else {
        bowl.setCalibration(false);
    }
}

/**
* Save the current configuration in the EEPROM.
* 
* The save operation is called by the user interaction
* so it is only needed to save the structure as-is
* 
* \note if the calibration is already set, it is possible to
* recalibrate a single value (tare or full bowl)
*/
void saveConfiguration() {
    // Check if both values are non-zero else the
    // calibration could not be saved
    if( (savedConfig.emptyBowl != 0) && 
        (savedConfig.fullBowl != 0) ) {
        EEPROM.put(EEPROM_ADDRESS, savedConfig);
        bowl.calibrateTare(savedConfig.emptyBowl);
        bowl.calibrateFullBowl(savedConfig.fullBowl);
        bowl.setCalibration(true);
        // Disable both the calibration signals
        setWaterFullSignal(LOW);
        setTareSignal(HIGH);
        }
}
