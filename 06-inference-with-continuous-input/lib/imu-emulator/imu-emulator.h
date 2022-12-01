/**
 * Emulate the IMU class in Arduino.
 * 
 * Author: Shawn Hymel (Edge Impulse)
 * Date: August 26, 2022
 * License: Apache-2.0
 * 
 * Copyright 2022 EdgeImpulse, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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