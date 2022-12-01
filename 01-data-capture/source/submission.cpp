/**
 * Data collection programming assignment
 * 
 * Your task is to modify the code below (look for the "YOUR CODE HERE" tags) to
 * read accelerometer and gyroscope data from the IMU for 1 second with a 100 Hz
 * sampling rate and print the raw values to the terminal in a comma-separated
 * values (CSV) format.
 * 
 * Every 10 ms, add 6 samples (acc_x, acc_y, acc_z, gyr_x, gyr_y, gyr_z) to 
 * input_buf[]. The order of input_buf[] will be as follows:
 * 
 * [acc_x0, acc_y0, acc_z0, gyr_x0, gyr_y0, gyr_z0, acc_x1, ...]
 * 
 * where all 6 readings of the first sample are first, followed by all 6 
 * readings of the second sample, and so on.
 * 
 * You should also keep track of the timestamp when each reading was taken. You
 * can use the millis() function to get the timestamp. Record the timestamp in 
 * the timestamps[] buffer. 
 * 
 * Once you have filled the buffer with 600 values (100 readings, each with 6
 * channels), print the header with the channel names, and then print the 
 * timestamp and values of each reading on a new line. For example, the first
 * 4 readings would look like this on the console:
 * 
 *  timestamp,accX,accY,accZ,gyrX,gyrY,gyrZ
 *  0,1.47,1.69,9.19,1.59,-0.55,-1.59
 *  10,1.50,1.61,9.01,0.98,-0.37,-1.89
 *  20,1.50,1.62,8.85,0.61,-0.06,-1.59
 *  30,1.54,1.64,8.65,0.18,-0.55,-1.34
 *  ...
 * 
 * You may print other things to the console to help you debug. However, when
 * you are ready to actually collect data or submit this assignment to the
 * autograder, you should not print anything else (other than the CSV data shown
 * above) to the console.
 * 
 * To build and test your code, navigate to the directory that contains the
 * Makefile for this project. In a console, enter:
 * 
 *  make -j
 *  ./build/app tests/alpha.8ed919a3a61d.csv
 * 
 * This will run the simulator and feed values from the given .csv file to your
 * IMU object. You should see a header and CSV values printed to the screen 
 * (they should match what's in the .csv file).
 * 
 * Try with the other test files!
 * 
 * Note: you should be able to copy this program into the Arduino IDE and have
 * it run on the Arduino Nano 33 BLE Sense. If you run 
 * serial-data-collect-csv.py and connect it to the serial port with your
 * Arduino, it will save your CSV readings in .csv files.
 */

// Include a library to help us read from the attached IMU. If not on an 
// Arduino, use the emulators instead to approximate Arduino functionality.
#ifdef ARDUINO
    #include <stdarg.h>
    #include <Arduino_LSM9DS1.h>
#else
    #include "time-emulator.h"
    #include "imu-emulator.h"
    #include "print-emulator.h"
#endif

// Settings
#define LED_R_PIN           22        // Red LED pin

// Constants
#define CONVERT_G_TO_MS2    9.80665f  // Used to convert G to m/s^2
#define SAMPLING_FREQ_HZ    100       // 100 Hz sampling rate
#define SAMPLING_PERIOD_MS  1000 / SAMPLING_FREQ_HZ     // Sampling period (ms)
#define NUM_CHANNELS        6         // Accel x, y, z and gyro x, y, z
#define NUM_READINGS        100       // 100 readings at 100 Hz is 1 sec window

// Function declarations
void ei_printf(const char *format, ...);

// Store raw readings in a buffer that has 6 * 100 = 600 elements
static float input_buf[NUM_CHANNELS * NUM_READINGS];

// Store timestamps (we need them for training and test data)
static int timestamps[NUM_READINGS];

// Common wrapper to print to the console or serial temrinal.
// Use ei_printf("Some string") instead of Serial.print("Some string") and 
// ei_printf("Some string/r/n") instead of Serial.println("Some string").
#ifdef ARDUINO
void ei_printf(const char *format, ...) {
    static char print_buf[1024] = { 0 };

    va_list args;
    va_start(args, format);
    int r = vsnprintf(print_buf, sizeof(print_buf), format, args);
    va_end(args);

    if (r > 0) {
        Serial.write(print_buf);
    }
}
#endif

// Setup function that is called once as soon as the program starts
void setup() {

    // Enable LED pin (RGB LEDs are active low on the Nano 33 BLE Sense)
    // and initialize serial (only on Arduino)
#ifdef ARDUINO
    pinMode(LED_R_PIN, OUTPUT);
    digitalWrite(LED_R_PIN, HIGH);
    Serial.begin(115200);
#endif

    // Start IMU
    if (!IMU.begin()) {
        ei_printf("ERROR: Failed to initialize IMU!\r\n");
        while (1);
    }
}

// Loop function that is called repeatedly after setup()
void loop() {

    float acc_x, acc_y, acc_z, gyr_x, gyr_y, gyr_z;
    unsigned long start_timestamp;
    unsigned long timestamp;

    // Turn on LED to show we're recording
#ifdef ARDUINO
    digitalWrite(LED_R_PIN, LOW);
#endif

    // Sample the IMU for 1 second. You should end up with 100 readings for each 
    // of the 6 channels after the 1 second is over.
    //  - Store the timestamp in the timestamps[i] element
    //  - Read acc_x, acc_y, acc_z, gyr_x, gyr_y, and gyr_z from the IMU. See 
    //    the documentation here to read from the accelerometer and gyroscope:
    //    https://www.arduino.cc/reference/en/libraries/arduino_lsm9ds1/
    //  - Convert accelerometer raw readings from G-force to m/s^2
    //  - Recall that the order of input_buf[] should be 
    //    [acc_x0, acc_y0, acc_z0, gyr_x0, gyr_y0, gyr_z0, acc_x1, ...]
    //  - Make sure you wait between each reading long enough for a 100 Hz 
    //    sampling rate.
    start_timestamp = millis();
    for (int i = 0; i < NUM_READINGS; i++) {
        timestamp = millis();
        // --- YOUR CODE HERE ---

        // --- END CODE ---
    }

    // Turn off LED to show we're done recording
#ifdef ARDUINO
    digitalWrite(LED_R_PIN, HIGH);
#endif

    // Print header
    ei_printf("timestamp,accX,accY,accZ,gyrX,gyrY,gyrZ\r\n");

    // Print out the entire buffer in CSV format with a reading on each line
    //  - Loop through all readings in the buffer
    //  - Print out each set of readings on a line, starting with the timestamp
    //    and in the order: acc_x, acc_y, acc_z, gyr_x, gyr_y, gyr_z
    //  - The timestamps should start at 0 and increment relatively. Use the
    //    value in start_timestamp to achieve this.
    //  - All readings should be printed in floating point format with
    //    2 decimal places. See this page on how to work with printf() (and
    //    similarly, ei_printf()):
    //    https://www.sas.upenn.edu/~saul/parasite/man/man3/printf.3.html
    //  - Don't forget to end your line with "\r\n"
    // --- YOUR CODE HERE ---

    // --- END CODE ---

    // Print empty line to transmit termination of recording
    ei_printf("\r\n");

    // Wait some time before collecting data again
    delay(1000);
}