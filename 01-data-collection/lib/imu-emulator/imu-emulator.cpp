/**
 * IMU emulator class definition
 */

#include "imu-emulator.h"

// Global ImuEmu object (to emulate the Arduino LSM9DS1 library)
ImuEmu IMU;

// Constructor
ImuEmu::ImuEmu() {

}

// Register accelerometer callback function
int ImuEmu::registerAccelCallback(accel_func_ptr cb) {

    // Assign callback if there is not one already
    if (accel_cb_ptr != 0) {
        return -1;
    } else {
        accel_cb_ptr = cb;
    }
    
    return 0;
}

// Register accelerometer callback function
int ImuEmu::registerGyroCallback(gyro_func_ptr cb) {

    // Assign callback if there is not one already
    if (gyro_cb_ptr != 0) {
        return -1;
    } else {
        gyro_cb_ptr = cb;
    }
    
    return 0;
}

// Blank begin that does nothing
int ImuEmu::begin() {
    return 1;
}

// Let the autograder supply accelerometer values
// x, y, and z are output parameters
// Returns 0 on failure, 1 on success
int ImuEmu::readAcceleration(float& x, float& y, float& z) {
    
    // Call the callback function (implemented by the autograder)
    if (accel_cb_ptr == 0) {
        return 0;
    }
    int ret = accel_cb_ptr(x, y, z);
    
    return ret;
}

// Let the autograder supply gyroscope values
// x, y, and z are output parameters
// Returns 0 on failure, 1 on success
int ImuEmu::readGyroscope(float& x, float& y, float& z) {
    
    // Call the callback function (implemented by the autograder)
    if (gyro_cb_ptr == 0) {
        return 0;
    }
    int ret = gyro_cb_ptr(x, y, z);

    return ret;
}