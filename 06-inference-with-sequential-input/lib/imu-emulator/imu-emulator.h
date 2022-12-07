/**
 * IMU emulator class interface
 */

#ifndef IMUEMU_H
#define IMUEMU_H

// Callback function pointer types
typedef int (*accel_func_ptr)(float&, float&, float&);
typedef int (*gyro_func_ptr)(float&, float&, float&);

class ImuEmu {
    public:
        ImuEmu();
        int registerAccelCallback(accel_func_ptr cb);
        int registerGyroCallback(gyro_func_ptr cb);

        // Arduino interface
        int begin();
        int readAcceleration(float& x, float& y, float& z);
        int readGyroscope(float& x, float& y, float& z);
    private:
        accel_func_ptr accel_cb_ptr = 0;
        gyro_func_ptr gyro_cb_ptr = 0;
};

// Declare global object (to emulate Arduino LSM9DS1 library)
extern ImuEmu IMU;

#endif // IMUEMU_H