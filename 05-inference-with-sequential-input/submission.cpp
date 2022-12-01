/**
 * Sequential inference programming assignment
 * 
 * Your task is to modify the code below (look for the "YOUR CODE HERE" tags) to
 * perform inference in a sequential manner. The idea is that the magic wand 
 * will sample the IMU for 1 second, filling up a buffer with accelerometer and
 * gyroscope readings. Once filled, that buffer needs to be passed to the 
 * run_classifier() function in order to perform inference.
 * 
 * Once you receive inference results, find the label with the highest value.
 * You must print the label with the highest prediction value followed by that
 * value. For example, if "alpha" is the highest class with a confidence value
 * of 0.983477, you should print the following to the console:
 * 
 * ANS: alpha, 0.983477
 * 
 * An example call of ei_printf() is given in the code below. The autograder
 * will look for the prefix "ANS:" to find your answer. The label and value
 * must be separated by a comma (,) for the autograder.
 * 
 * To build and test your code, navigate to the directory that contains the
 * Makefile for this project. In a console, enter:
 * 
 *  make -j
 *  ./build/app tests/alpha.8ed919a3a61d.csv
 * 
 * This will run the simulator and feed values from the given .csv file to your
 * IMU object. You should see the answer printed to the screen:
 * 
 *  ANS: alpha, 0.996094
 * 
 * Try with the other test files!
 * 
 * Note: you should be able to copy this program into the Arduino IDE and have
 * it run on the Arduino Nano 33 BLE Sense.
 */

// Include the name of the Edge Impulse SDK library you imported. This will 
// switch between Arduino and computer (autograder) libraries as needed.
#ifdef ARDUINO
    #include <Arduino_LSM9DS1.h>
    #include <magic-wand-capstone_inferencing.h>
#else
    #include "time-emulator.h"
    #include "imu-emulator.h"
    #include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#endif

// Settings
#define LED_R_PIN           22        // Red LED pin

// Constants
#define CONVERT_G_TO_MS2    9.80665f  // Used to convert G to m/s^2
#define SAMPLING_FREQ_HZ    EI_CLASSIFIER_FREQUENCY     // 100 Hz sampling rate
#define SAMPLING_PERIOD_MS  1000 / SAMPLING_FREQ_HZ     // Sampling period (ms)
#define NUM_CHANNELS        EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME // 6 channels
#define NUM_READINGS        EI_CLASSIFIER_RAW_SAMPLE_COUNT      // 100 readings
#define NUM_CLASSES         EI_CLASSIFIER_LABEL_COUNT           // 4 classes

// Function declarations
static int get_signal_data(size_t offset, size_t length, float *out_ptr);

// Means and standard deviations from our dataset curation
static const float means[] = {0.4869, -0.6364, 8.329, -0.1513, 4.631, -9.8836};
static const float std_devs[] = {3.062, 7.2209, 6.9951, 61.3324, 104.1638, 108.3149};

// Store raw readings in a buffer that has 6 * 100 = 600 elements
static float input_buf[NUM_CHANNELS * NUM_READINGS];

// Wrapper for raw input buffer
static signal_t sig;

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

    // Assign callback function to fill buffer used for preprocessing/inference
    sig.total_length = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;
    sig.get_data = &get_signal_data;
}

// Loop function that is called repeatedly after setup()
void loop() {

    float acc_x, acc_y, acc_z, gyr_x, gyr_y, gyr_z;
    unsigned long timestamp;
    ei_impulse_result_t result; // Used to store inference output
    EI_IMPULSE_ERROR res;       // Return code from inference

    // Turn on LED to show we're recording
#ifdef ARDUINO
    digitalWrite(LED_R_PIN, LOW);
#endif

    // Sample the IMU for 1 second. You should end up with 100 readings for each 
    // of the 6 channels after the 1 second is over.
    //  - Convert accelerometer raw readings from G-force to m/s^2
    //  - For each reading, perform standardization using the mean and std_dev
    //    associated with each channel
    //  - Store your standardized readings in input_buf[]
    //  - Recall that the order of input_buf[] should be 
    //    [acc_x0, acc_y0, acc_z0, gyr_x0, gyr_y0, gyr_z0, acc_x1, ...]
    //  - Make sure you wait between each reading long enough for a 100 Hz 
    //    sampling rate.
    //  - Hint: this looks similar to the loop you wrote for the data collection
    //    exercise (the differences are that you don't need to collect the 
    //    timestamps and you need to perform standardization on the readings)
    // --- YOUR CODE HERE ---

    // --- END CODE ---

    // Turn off LED to show we're done recording
#ifdef ARDUINO
    digitalWrite(LED_R_PIN, HIGH);
#endif

    // Call run_classifier() to perform preprocessing and inference
    // --- YOUR CODE HERE ---
    
    // --- END CODE ---

    // Find the label with the highest classification value
    // - Category labels are stored in the 
    //   ei_classifier_inferencing_categories[] array
    // - Prediction values are stored in result.classification[i].value for each
    //   label i
    // - Store the highest prediction value in max_val
    // - Store the index of the label with the highest value in max_idx
    float max_val = 0.0;
    int max_idx = -1;
    // --- YOUR CODE HERE ---
    
    // --- END CODE ---

    // Print return code and how long it took to perform inference
    ei_printf("run_classifier returned: %d\r\n", res);
    ei_printf("Timing: DSP %d ms, inference %d ms, anomaly %d ms\r\n", 
            result.timing.dsp, 
            result.timing.classification, 
            result.timing.anomaly);

    // Print inference/prediction results
    ei_printf("Predictions:\r\n");
    for (uint16_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
        ei_printf("  %s: ", ei_classifier_inferencing_categories[i]);
        ei_printf("%.5f\r\n", result.classification[i].value);
    }

    // Print the answer (line must begin with "ANS: " for the autograder)
    ei_printf("ANS: %s, %f\r\n", 
                ei_classifier_inferencing_categories[max_idx], 
                result.classification[max_idx].value);

    // Wait some time before running inference again
#if ARDUINO
    delay(1000);
#else
    delay(10);
#endif
}

// Callback: fill a section of the out_ptr buffer when requested
static int get_signal_data(size_t offset, size_t length, float *out_ptr) {
    for (size_t i = 0; i < length; i++) {
        out_ptr[i] = (input_buf + offset)[i];
    }

    return EIDSP_OK;
}