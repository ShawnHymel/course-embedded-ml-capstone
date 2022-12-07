/**
 * The code below can be used for inference in Arduino
 * 
 * TODO: IMU in ISR hangs processor!
 */

// Include the name of the Edge Impulse SDK library you imported. This will 
// switch between Arduino and computer libraries as needed.
#ifdef ARDUINO
    #include <mbed.h>
    #include <Arduino_LSM9DS1.h>
    #include <magic-wand-capstone_inferencing.h>
#else
    #include <thread>
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
#define SAMPLING_PERIOD_US  1000 * SAMPLING_PERIOD_MS   // Sampling period (us)
#define NUM_CHANNELS        EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME // 6 channels
#define NUM_READINGS        EI_CLASSIFIER_RAW_SAMPLE_COUNT      // 150 readings
#define NUM_CLASSES         EI_CLASSIFIER_LABEL_COUNT           // 4 classes

// Define the number of times inference happens each full window (1 second)
#define SLICES_PER_WINDOW   6                           // Inferences per sec

// Raw buffer (half of the double buffer) should be big enough for 1 slice
// This is also the "number of readings per slice"
#define RAW_BUF_SIZE        (NUM_CHANNELS * NUM_READINGS) / SLICES_PER_WINDOW

// Function declarations
static int get_signal_data(size_t offset, size_t length, float *out_ptr);
void do_sampling();
void do_inference();

// Means and standard deviations from our dataset curation
static const float means[] = {-0.2238, -0.3129, 5.6543, -4.8021, 4.0536, -6.4238};
static const float std_devs[] = {5.6031, 7.5372, 7.6538, 149.2136, 125.0134, 133.8875};

// Double buffer (used to capture raw samples from sensor)
static float raw_buf_0[RAW_BUF_SIZE];
static float raw_buf_1[RAW_BUF_SIZE];
static float *raw_buf_wr;
static float *raw_buf_rd;
static int raw_buf_count = 0;
static bool raw_buf_ready = false;

// Buffer that contains a full window for inference. Note that this is now a 
// ring buffer where older samples are overwritten!
static float input_buf[NUM_CHANNELS * NUM_READINGS];
static int input_buf_slice = 0;

// Wrapper for raw input buffer
static signal_t sig;

// Handles to threads
#if ARDUINO
    static rtos::Thread thread_sampling(osPriorityHigh);
    static rtos::Thread thread_inference(osPriorityLow);
#else
    static std::thread thread_sampling;
    static std::thread thread_inference;
#endif

// Global flag that controls the threads
static volatile bool running = true;

/*******************************************************************************
 * Functions
 */

// Callback: fill a section of the out_ptr buffer when requested.
// Note that we must now start at the correct slice in the ring buffer.
static int get_signal_data(size_t offset, size_t length, float *out_ptr) {

    // Find where to start reading from the ring buffer
    size_t idx = offset + (input_buf_slice * RAW_BUF_SIZE);

    // Copy the elements in the ring buffer to the output buffer
    for (size_t i = 0; i < length; i++) {

        // Copy element
        out_ptr[i] = input_buf[idx];     

        // Increment and wrap the ring buffer pointer
        idx++;
        if (idx >= (NUM_CHANNELS * NUM_READINGS)) {
            idx = offset;
        }
    }

    // Uncomment this section to print the whole inference buffer
    // int timestamp = 0;
    // int counter = 0;
    // for (int i = 0; i < length / 6; i++) {
    //     ei_printf("%i, ", timestamp);
    //     timestamp += 10;
    //     for (int j = 0; j < 5; j++) {
    //         ei_printf("%.2f, ", out_ptr[(i * 6) + j]);
    //     }
    //     ei_printf("%.2f\r\n", out_ptr[(i * 6) + 5]);
    // }

    return EIDSP_OK;
}

// Call this if you want to stop the threads
void stop_threads() {
    running = false;
    thread_sampling.join();
    thread_inference.join();
}

/******************************************************************************* 
 * Threads
 */

// High-priority thread that samples from the IMU
void do_sampling() {
    
    unsigned long time_start, time_target, time_actual, to_sleep;
    float acc_x, acc_y, acc_z, gyr_x, gyr_y, gyr_z;
    static bool led_state = false;
  
    // Initialize times
    time_start = millis();
    time_target = 0;

    // Run this thread forever
    while (running) {

        // Determine how long to sleep to meet target
        time_target += SAMPLING_PERIOD_MS;
        time_actual = millis() - time_start;
        if (time_actual < time_target) {
            to_sleep = time_target - time_actual;
        } else {
            to_sleep = 0;
        }
    
        // Sleep before sampling
#if ARDUINO
        rtos::ThisThread::sleep_for(to_sleep);
#else
        std::this_thread::sleep_for(std::chrono::milliseconds(to_sleep));
#endif
    
        // Toggle LED to show that sampling is happening
#if ARDUINO
        led_state = !led_state;
        digitalWrite(LED_R_PIN, led_state);
#endif
        
        // Get raw readings from the sensors
        IMU.readAcceleration(acc_x, acc_y, acc_z);
        IMU.readGyroscope(gyr_x, gyr_y, gyr_z);
    
        // Store the raw readings in the buffer (use the write pointer)
        raw_buf_wr[raw_buf_count + 0] = acc_x;
        raw_buf_wr[raw_buf_count + 1] = acc_y;
        raw_buf_wr[raw_buf_count + 2] = acc_z;
        raw_buf_wr[raw_buf_count + 3] = gyr_x;
        raw_buf_wr[raw_buf_count + 4] = gyr_y;
        raw_buf_wr[raw_buf_count + 5] = gyr_z;
    
        // Increment the counter by the number of readings you stored
        raw_buf_count += NUM_CHANNELS;
    
        // Swap pointers if buffer is full
        if (raw_buf_count >= RAW_BUF_SIZE) {
            raw_buf_count = 0;
            raw_buf_ready = true;
            if (raw_buf_wr == &raw_buf_0[0]) {
                raw_buf_wr = raw_buf_1;
                raw_buf_rd = raw_buf_0;
            } else {
                raw_buf_wr = raw_buf_0;
                raw_buf_rd = raw_buf_1;
            }
        }
    }
}

// Low-priority thread that performs inference
void do_inference() {
  
    float acc_x, acc_y, acc_z, gyr_x, gyr_y, gyr_z;
    ei_impulse_result_t result; // Used to store inference output
    EI_IMPULSE_ERROR res;       // Return code from inference
    int start_slice_offset;     // Index of the current slice in input_buf

    // Do inference forever
    while (running) {
    
        // If buffer is already full, it has been overrun
        if (raw_buf_ready) {
            ei_printf("ERROR: Buffer overrun\r\n");
            continue;
        }
    
        // Wait until buffer is full
        while (!raw_buf_ready) {
            delay(10);
            if (!running) {
                break;
            }
        }
        raw_buf_ready = false;
    
        // Compute the index of the current slice for input_buf
        start_slice_offset = RAW_BUF_SIZE * input_buf_slice;
    
        // Transform and copy contents of raw (read) buffer to input (ring) buffer
        for (int i = 0; i < (RAW_BUF_SIZE / NUM_CHANNELS); i++) {
    
            // Get accelerometer and gyroscope data from raw (read) buffer
            acc_x = raw_buf_rd[(NUM_CHANNELS * i) + 0];
            acc_y = raw_buf_rd[(NUM_CHANNELS * i) + 1];
            acc_z = raw_buf_rd[(NUM_CHANNELS * i) + 2];
            gyr_x = raw_buf_rd[(NUM_CHANNELS * i) + 3];
            gyr_y = raw_buf_rd[(NUM_CHANNELS * i) + 4];
            gyr_z = raw_buf_rd[(NUM_CHANNELS * i) + 5];
    
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
    
            // Fill the correct slice in input_buf with the standardized readings
            input_buf[start_slice_offset + (NUM_CHANNELS * i) + 0] = acc_x;
            input_buf[start_slice_offset + (NUM_CHANNELS * i) + 1] = acc_y;
            input_buf[start_slice_offset + (NUM_CHANNELS * i) + 2] = acc_z;
            input_buf[start_slice_offset + (NUM_CHANNELS * i) + 3] = gyr_x;
            input_buf[start_slice_offset + (NUM_CHANNELS * i) + 4] = gyr_y;
            input_buf[start_slice_offset + (NUM_CHANNELS * i) + 5] = gyr_z;
        }
    
        // Increment and wrap slice counter
        input_buf_slice++;
        if (input_buf_slice >= SLICES_PER_WINDOW) {
            input_buf_slice = 0;
        }
    
        // Call run_classifier() to perform preprocessing and inferece
        res = run_classifier(&sig, &result, false);
    
        // Find the label with the highest classification value
        float max_val = 0.0;
        int max_idx = -1;
        for (int i = 0; i < NUM_CLASSES; i++) {
            if (result.classification[i].value > max_val) {
                max_val = result.classification[i].value;
                max_idx = i;
            }
        }
    
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
    
        // Uncomment to point out the end of each .csv file
#ifndef ARDUINO
        static int slice_counter = 0;
        slice_counter++;
        if (slice_counter >= SLICES_PER_WINDOW) {
            slice_counter = 0;
            ei_printf("^^^\r\n");
        }
#endif
        ei_printf("---\r\n");
    }
}

/*******************************************************************************
 * Main
 * Note: setup and loop are not in a thread!
 */

// Spawn our two worker threads
void setup() {

    // Enable LED pin (RGB LEDs are active low on the Nano 33 BLE Sense)
    // and initialize serial (only on Arduino)
#ifdef ARDUINO
    pinMode(LED_R_PIN, OUTPUT);
    digitalWrite(LED_R_PIN, HIGH);
    Serial.begin(115200);
#endif

    // Initialize double buffer pointers
    raw_buf_wr = raw_buf_0;
    raw_buf_rd = raw_buf_1;

    // Clear ring buffer
    memset(input_buf, 0, (NUM_CHANNELS * NUM_READINGS) * sizeof(float));

    // Start IMU
    if (!IMU.begin()) {
        ei_printf("ERROR: Failed to initialize IMU!\r\n");
        while (1);
    }

    ei_printf("Raw size: %i\r\n", RAW_BUF_SIZE);
    ei_printf("Ring size: %i\r\n", NUM_CHANNELS * NUM_READINGS);

    // Assign callback function to fill buffer used for preprocessing/inference
    sig.total_length = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;
    sig.get_data = &get_signal_data;

    // Start threads
#if ARDUINO
    thread_sampling.start(mbed::callback(&do_sampling));
    thread_inference.start(mbed::callback(&do_inference));
#else
    thread_sampling = std::thread(do_sampling);
    thread_inference = std::thread(do_inference);
#endif
}

// Sleep indefinitely to let the threads do the work
void loop() {

#if ARDUINO
    rtos::ThisThread::sleep_for(100);
#else
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
#endif
}