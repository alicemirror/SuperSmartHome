/**
 *  \file waterlevel.cpp
 *  \brief Class managing the load cell sensor methods and calculations
 *  
 *  Weight from load cell sensor is used to calculate the water level in
 *  the dog bowl
 *  
 *  \author Enrico Miglino <balearicdynamics@gmail.com> \n
 *  Balearic Dynamics <we-are-borg.com> SPAIN
 *  \date May 2020
 *  \version 1.1
 *  Licensed under GNU LGPL 3.0
 */

#include "waterlevel.h"

void WaterLevel::begin(void) {
    // Initialize the scale library instance
    scaleSensor.begin(PIN_HX711_DOUT, PIN_HX711_CLK);
    // Initialize the default values
    initDefaults();
}

void WaterLevel::calibrateTare(float value) {
    currentStatus.bowlTare = value;
}

void WaterLevel::calibrateFullBowl(float value) {
    currentStatus.bowlFull = value;
}

void WaterLevel::setCalibration(boolean flag) {
    currentStatus.calibration = flag;
}

float WaterLevel::readScale(void) {
    // Read the new scale value
    float lastRead = scaleSensor.get_units(SCALE_SAMPLES);

    // The current water weight is the net weight. Without calibration the tare
    // is set to zero and has no influence on the reading.
    currentStatus.currentWeight = valOptimizer(lastRead - currentStatus.bowlTare);

    // If the weight is not yet calibrated only the bare sensor reading is assigned
    // as without calibration there is no tare and no full bowl weight
    if(currentStatus.calibration == true) {
        // Check if the remaining quantity of water is under 10% of the total weight
        calcRemainingPerc();
        
        if(currentStatus.percWater <= 10.0) {
            currentStatus.bowlLow = true;
        } else {
            currentStatus.bowlLow = false;
        } // Not low level
    } // Is calibrated

    return currentStatus.currentWeight;
}

void WaterLevel::initDefaults(void) {

    // Initializes the parameters status to false untile
    // a saved configuration setting
    currentStatus.calibration = false;
    // This flag will become true when the water reach the lower level
    // of the bowl. False until calibration values are not acquired.
    currentStatus.bowlLow = false;
    // Set the tare initially to 0. Should be read from the saved calibration
    currentStatus.bowlTare = 0.0;
    // Set full blow weight initially to 0. Should be read from the saved calibration
    currentStatus.bowlFull = 0;
    // No readings from the sensor yet
    currentStatus.currentWeight = 0;
}

float WaterLevel::valOptimizer(float value) {
    int optimizer;
    // Reduce the precison of a factor 10 (0.01) according 
    // with the physical precison of the readings
    optimizer = int(value * 10);
    return optimizer / 10;
}

void WaterLevel::calcRemainingPerc() {
    currentStatus.percWater = valOptimizer(currentStatus.currentWeight * 100 /
                                            currentStatus.bowlFull);
}
