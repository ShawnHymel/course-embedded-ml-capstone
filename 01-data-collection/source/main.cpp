/**
 * Main application entrypoint for the sequential inferencing assignment
 *
 * Reads CSV files from tests/ and constructs a vector of raw readings. The 
 * student implements setup() and loop() functions in submission.cpp. The IMU
 * object can read from a virtual accelerometer and gyroscope, which pull
 * values from the CSV files.
 * 
 * In this program, loop() is called once for each CSV file. The student needs
 * to read a full CSV file (1 second's worth of data), perform inference, and
 * print the predicted label and value to the console.
 * 
 * Author: Shawn Hymel (EdgeImpulse, Inc.)
 * Date: August 28, 2022
 * License: Apache-2.0
 */

#include <stdio.h>
#include <cstdlib>
#include <array>

#include "csv.h"
#include "time-emulator.h"
#include "imu-emulator.h"
#include "submission.h"

// Declare our helper functions
int findClosestIdx(unsigned long time_ms);
int readAccelerometerCallback(float& x, float& y, float& z);
int readGyroscopeCallback(float& x, float& y, float& z);

// Vector of raw readings to be supplied to the user via callbacks
static std::vector<std::array<float, 7>> raw_readings;

// Current timestamp (milliseconds) of user's IMU readings (used in callbacks)
static unsigned long first_reading_timestamp = 0;
static bool is_first_reading = true;

// How the arrays in the raw readings vector are indexed
enum VectorIDXs {
    TIME_IDX = 0,
    ACC_X_IDX,
    ACC_Y_IDX,
    ACC_Z_IDX,
    GYR_X_IDX,
    GYR_Y_IDX,
    GYR_Z_IDX
};

/*******************************************************************************
 * Main
 */

// Main function to call setup and loop
int main(int argc, char **argv) {

    float timestamp, accX, accY, accZ, gyrX, gyrY, gyrZ;
    std::array<float, 7> reading;
    
    float sample_rate = 0.0;
    int reading_idx = 0;

    // Check to make sure we've beens supplied at least one input file
    if (argc < 2) {
    printf("ERROR: No input file specified\r\n");
        return 1;
    }

    // Register the callback functions to simulate reading from the IMU
    IMU.registerAccelCallback(readAccelerometerCallback);
    IMU.registerGyroCallback(readGyroscopeCallback);

    // Call user supplied setup() function once
    setup();

    // Loop through all files provided as arguments
    for (int file_idx = 1; file_idx < argc; file_idx++) {

        // Clear the raw readings vector
        raw_readings.clear();

        // Read CSV header
        io::CSVReader<7> csv_reader(argv[file_idx]);
        csv_reader.read_header( io::ignore_extra_column, 
                                "timestamp", 
                                "accX", 
                                "accY",
                                "accZ",
                                "gyrX",
                                "gyrY",
                                "gyrZ");

        // Construct vector of raw values from CSV file
        while (csv_reader.read_row( timestamp, 
                                    accX, 
                                    accY, 
                                    accZ, 
                                    gyrX, 
                                    gyrY, 
                                    gyrZ)) {

            // Calculate sample rate (and use that instead of what's in CSV)
            if (reading_idx == 0) {
                sample_rate = timestamp;
            } else if (reading_idx == 1) {
                sample_rate = timestamp - sample_rate;
            }
            timestamp = sample_rate * raw_readings.size();

            // Read values into array
            reading[TIME_IDX] = timestamp;
            reading[ACC_X_IDX] = accX;
            reading[ACC_Y_IDX] = accY;
            reading[ACC_Z_IDX] = accZ;
            reading[GYR_X_IDX] = gyrX;
            reading[GYR_Y_IDX] = gyrY;
            reading[GYR_Z_IDX] = gyrZ;

            // Push array onto vector
            raw_readings.push_back(reading);

            // Increment our index
            reading_idx++;
        }

        // Reset timestamp readings for callbacks
        first_reading_timestamp = 0;
        is_first_reading = true;

        // Call user supplied loop() function (once for each CSV file)
        loop();
    }

    // We're done here
    return 0;
}

/*******************************************************************************
 * Functions
 */

// Read accelerometer callback function
int readAccelerometerCallback(float& x, float& y, float& z) {

    // Return 0's if the readings vector is empty
    if (raw_readings.empty()) {
        x = 0.0;
        y = 0.0;
        z = 0.0;

        return 1;
    }

    // Update timestamp
    if (is_first_reading) {
        is_first_reading = false;
        first_reading_timestamp = millis();
    }

    // Calculated elapsed time
    unsigned long elapsed = millis() - first_reading_timestamp;
    int closest_time_idx = findClosestIdx(elapsed);

    // Assign values from the row closest to the requested elapsed timestamp
    x = raw_readings[closest_time_idx][ACC_X_IDX];
    y = raw_readings[closest_time_idx][ACC_Y_IDX];
    z = raw_readings[closest_time_idx][ACC_Z_IDX];

    return 1;
}

// Read gyroscope callback function
int readGyroscopeCallback(float& x, float& y, float& z) {

    // Return 0's if the readings vector is empty
    if (raw_readings.empty()) {
        x = 0.0;
        y = 0.0;
        z = 0.0;

        return 1;
    }

    // Update timestamp
    if (is_first_reading) {
        is_first_reading = false;
        first_reading_timestamp = millis();
    }

    // Calculated elapsed time
    unsigned long elapsed = millis() - first_reading_timestamp;
    int closest_time_idx = findClosestIdx(elapsed);

    // Assign values from the row closest to the requested elapsed timestamp
    x = raw_readings[closest_time_idx][GYR_X_IDX];
    y = raw_readings[closest_time_idx][GYR_Y_IDX];
    z = raw_readings[closest_time_idx][GYR_Z_IDX];

    return 1;
}

// Get closest reading from vector of readings
int findClosestIdx(unsigned long time_ms) {

    unsigned long closest_time = raw_readings[0][TIME_IDX];
    int closest_time_idx = 0;
    unsigned long num;

    // Loop through timestamps in vector to find closest
    for (long unsigned int i = 0; i < raw_readings.size(); i++) {
        num = raw_readings[i][TIME_IDX];
        if (abs((long long)time_ms - (long long)num) < 
            abs((long long)time_ms - (long long)closest_time)) {
                closest_time = num;
                closest_time_idx = i;
        }
    }

    return closest_time_idx;
}