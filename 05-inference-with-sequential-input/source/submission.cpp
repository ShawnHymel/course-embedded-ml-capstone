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
#define ANOMALY_THRESHOLD   0.3       // Anything over this is an anomaly

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
static const float means[] = {-0.2238, -0.3129, 5.6543, -4.8021, 4.0536, -6.4238};
static const float std_devs[] = {5.6031, 7.5372, 7.6538, 149.2136, 125.0134, 133.8875};

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
    //  - Make sure you wait between each reading long enough for a 100 Hz 
    //    sampling rate.
    //  - For each reading, perform standardization using the mean and std_dev
    //    associated with each channel
    //  - Store your standardized readings in input_buf[]
    //  - Recall that the order of input_buf[] should be 
    //    [acc_x0, acc_y0, acc_z0, gyr_x0, gyr_y0, gyr_z0, acc_x1, ...]
    // --- YOUR CODE HERE ---
    for (int i = 0; i < NUM_READINGS; i++) {

        // Take timestamp so we can hit our target frequency
        timestamp = millis();

        // Get raw readings from the accelerometer and gyroscope
        IMU.readAcceleration(acc_x, acc_y, acc_z);
        IMU.readGyroscope(gyr_x, gyr_y, gyr_z);

        // Convert accelerometer units from G to m/s^s
        acc_x *= CONVERT_G_TO_MS2;
        acc_y *= CONVERT_G_TO_MS2;
        acc_z *= CONVERT_G_TO_MS2;

        // Perform standardization on each reading
        // Use the values from means[] and std_devs[]
        acc_x = (acc_x - means[0]) / std_devs[0];
        acc_y = (acc_y - means[1]) / std_devs[1];
        acc_z = (acc_z - means[2]) / std_devs[2];
        gyr_x = (gyr_x - means[3]) / std_devs[3];
        gyr_y = (gyr_y - means[4]) / std_devs[4];
        gyr_z = (gyr_z - means[5]) / std_devs[5];

        // Fill input_buf with the standardized readings. Recall tha the order
        // is [acc_x0, acc_y0, acc_z0, gyr_x0, gyr_y0, gyr_z0, acc_x1, ...]
        input_buf[(NUM_CHANNELS * i) + 0] = acc_x;
        input_buf[(NUM_CHANNELS * i) + 1] = acc_y;
        input_buf[(NUM_CHANNELS * i) + 2] = acc_z;
        input_buf[(NUM_CHANNELS * i) + 3] = gyr_x;
        input_buf[(NUM_CHANNELS * i) + 4] = gyr_y;
        input_buf[(NUM_CHANNELS * i) + 5] = gyr_z;

        // Wait just long enough for our sampling period
        while (millis() - timestamp < SAMPLING_PERIOD_MS);
    }
    // --- END CODE ---

    // Turn off LED to show we're done recording
#ifdef ARDUINO
    digitalWrite(LED_R_PIN, HIGH);
#endif

    // Call run_classifier() to perform preprocessing and inferece
    // --- YOUR CODE HERE ---
    res = run_classifier(&sig, &result, false);
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
    for (int i = 0; i < NUM_CLASSES; i++) {
        if (result.classification[i].value > max_val) {
            max_val = result.classification[i].value;
            max_idx = i;
        }
    }
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

        // Print anomaly score
#if EI_CLASSIFIER_HAS_ANOMALY == 1
        ei_printf("Anomaly: %.3f\r\n", result.anomaly);
#endif
    
        // Print the answer (line must begin with "ANS: " for the autograder)
        if (result.anomaly < ANOMALY_THRESHOLD) {
          ei_printf("ANS: %s, %f\r\n", 
                      ei_classifier_inferencing_categories[max_idx], 
                      result.classification[max_idx].value);
        } else {
          ei_printf("ANS: anomaly, %f\r\n", 
                      ei_classifier_inferencing_categories[max_idx], 
                      result.anomaly);
        }

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

    // Uncomment this section to print the whole inference buffer
    int timestamp = 0;
    int counter = 0;
    for (int i = 0; i < length / NUM_CHANNELS; i++) {
        ei_printf("%i, ", timestamp);
        timestamp += SAMPLING_PERIOD_MS;
        for (int j = 0; j < (NUM_CHANNELS - 1); j++) {
            ei_printf("%.2f, ", out_ptr[(i * NUM_CHANNELS) + j]);
        }
        ei_printf("%.2f\r\n", out_ptr[(i * NUM_CHANNELS) + (NUM_CHANNELS - 1)]);
    }

    return EIDSP_OK;
}