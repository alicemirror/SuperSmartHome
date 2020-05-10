/**
 *  \file waterlevel.h
 *  \brief Class managing the load cell sensor methods and calculations
 *  to determine the water level in the dog bowl.
 *  
 *  Calculate the water level (ml) based on the weight (gr)
 *  
 *  \author Enrico Miglino <balearicdynamics@gmail.com> \n
 *  Balearic Dynamics sl <www.balearicdynamics.com> SPAIN
 *  \date May 2020
 *  \version 1.1
 *  Licensed under GNU LGPL 3.0
 */

#ifndef _WATERLEVEL
#define _WATERLEVEL

#include <HX711.h>
#include "config.h"

//! Number of samples read every weight reading. 
//! Multiple samples reading gives more stability to the measure
#define SCALE_SAMPLES 10

//! Weight structure variables and flags
struct Weight {
    //! Status of the weight sensor. If this value is false,
    //! the weight sensor needs calibration
    boolean calibration;
    //! This is the low level alarm flag. It raise when the 
    //! remaining water weight is less than 10%
    boolean bowlLow;
    //! Remaining percentage of water in the bowl
    float percWater;
    //! Bowl tare. Calculated during calibration and persistent
    //! in the EEPROM
    float bowlTare;
    //! Bowl maximum weight (full of water, no tare). Calculated during calibration
    //! and persistend in the EEPROM.
    float bowlFull;
    //! Last weight get from the sensor, without the tare.
    float currentWeight;
};

/**
 * Class managing the load sensor
 */
class WaterLevel {

  public:
    //! Application status
    Weight currentStatus;  

    //! Sensor library instance
    HX711 scaleSensor;

    /**
     * Initializes the sensor library and the initial default setup
     */
    void begin(void);

    /**
     * Update the tare calibration.
     */
    void calibrateTare(float value);

    /**
     * Update the full bowl calibration.
     */
    void calibrateFullBowl(float value);

    /**
     * Set the calibration status.
     */
     void setCalibration(boolean flag);

    /**
     * Return the calibration status
     */
    boolean getCalibration(void);
    
    /** 
     * Set the default values when the class is instantiated
     */
    void initDefaults(void);

    /**
     * Read the load sensor weight and update the status values.
     * 
     * \note The method also return the weight value to be used
     * during the calibration process.
     */
    float readScale(void);

    /**
     * Optimizes the floating value reducing the precision to one
     * decimal value
     * 
     * \param value The float value to be treated
     * \return The optimized float value
     */
     float valOptimizer(float value);
    
    /**
     * Calculate the remaining weight percentage
     * 
     * The applied formula: (w - tare) * 100 / full bowl weight
     * 
     *  \param w weight in grams
     *  \return the remaining weight in percentage
     */
    void calcRemainingPerc(void);
};

#endif
