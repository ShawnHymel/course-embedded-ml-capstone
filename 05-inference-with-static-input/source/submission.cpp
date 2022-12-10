/**
 * Static inference programming assignment
 * 
 * Your task is to modify the code below (look for the "YOUR CODE HERE" tags) to
 * perform inference using a static buffer. We want to test inference using the
 * Edge Impulse C++ SDK library (or Arduino library) and ensure we are getting
 * the same results found in the Edge Impulse Studio.
 * 
 * The Edge Impule C++ SDK library has already been unzipped into the lib/ 
 * directory. It comes from the following project:
 * 
 *   https://studio.edgeimpulse.com/public/126176/v5
 * 
 * We have copied the "raw features" from the test sample alpha.151c1bc12b53
 * into the input_buf array for you. This will be our test sample. The input_buf
 * needs to be passed to the run_classifier() function in order to perform inference.
 * 
 * Once inference has been performed, you will need to find the label with the 
 * highest value. You must print the label with the highest prediction value 
 * followed by that value. For example, if "_idle" is the highest class with a 
 * confidence value of 0.983477, you should print the following to the console:
 * 
 *   ANS: _idle, 0.983477
 * 
 * An example call of ei_printf() is given in the code below. The autograder
 * will look for the prefix "ANS:" to find your answer. The label and value
 * must be separated by a comma (,) for the autograder. Any other output will
 * be ignored
 * 
 * To build and test your code, navigate to the directory that contains the
 * Makefile for this project. In a console, enter:
 * 
 *   make -j
 *   ./build/app
 * 
 * When you run the program, you should have the following printed to the
 * console:
 * 
 *   ANS: alpha, 0.731882
 * 
 * If you see this, then you know your code is correct!
 * 
 * Note: you should be able to copy this program into the Arduino IDE and have
 * it run on the Arduino Nano 33 BLE Sense.
 */

// Include the name of the Edge Impulse SDK library you imported. This will 
// switch between Arduino and computer (autograder) libraries as needed.
#ifdef ARDUINO
    #include <magic-wand_inferencing.h>
#else
    #include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#endif

// Constants (many come from lib/ei-cpp-sdk/model-parameters/model_metadata.h)
#define NUM_CLASSES         EI_CLASSIFIER_LABEL_COUNT           // 5 classes

// Function declarations
static int get_signal_data(size_t offset, size_t length, float *out_ptr);

// Raw features copied from test sample (Edge Impulse > Model testing)
static float input_buf[] = {
    0.2103, 1.2563, -0.2607, 0.0577, -0.0496, 0.0125, 0.2156, 1.2617, -0.2748, 0.0533, -0.0486, 0.0202, 0.2103, 1.2523, -0.2824, 0.0573, -0.0563, 0.0284, 0.1666, 1.2791, -0.2454, 0.0689, -0.0661, 0.0309, 0.1578, 1.2804, -0.2466, 0.0693, -0.0584, 0.0303, 0.1631, 1.2858, -0.2505, 0.0653, -0.0568, 0.0236, 0.1753, 1.2697, -0.2441, 0.0649, -0.0584, 0.0193, 0.2033, 1.2697, -0.2492, 0.0693, -0.0594, 0.0169, 0.1946, 1.2590, -0.2249, 0.0764, -0.0589, 0.0202, 0.1753, 1.2764, -0.2428, 0.0782, -0.0506, 0.0246, 0.1753, 1.2777, -0.2415, 0.0706, -0.0537, 0.0270, 0.1788, 1.2617, -0.2415, 0.0693, -0.0563, 0.0246, 0.1893, 1.2523, -0.2466, 0.0689, -0.0527, 0.0246, 0.1823, 1.2550, -0.2390, 0.0666, -0.0429, 0.0328, 0.1631, 1.2737, -0.2338, 0.0649, -0.0361, 0.0323, 0.1823, 1.2670, -0.2223, 0.0591, -0.0387, 0.0299, 0.2033, 1.2550, -0.2517, 0.0541, -0.0413, 0.0275, 0.1893, 1.2643, -0.2505, 0.0524, -0.0486, 0.0303, 0.1771, 1.2670, -0.2300, 0.0511, -0.0511, 0.0299, 0.1806, 1.2643, -0.2096, 0.0426, -0.0361, 0.0261, 0.2033, 1.2670, -0.2377, 0.0408, -0.0243, 0.0236, 0.2243, 1.2643, -0.2952, 0.0493, -0.0279, 0.0217, 0.1946, 1.2911, -0.2837, 0.0604, -0.0609, 0.0212, 0.1701, 1.2483, -0.2121, 0.0888, -0.0883, 0.0057, 0.2033, 1.2550, -0.2172, 0.0635, -0.0346, 0.0250, 0.1963, 1.2724, -0.2441, 0.0547, -0.0263, 0.0347, 0.1736, 1.2751, -0.2415, 0.0506, -0.0470, 0.0319, 0.1753, 1.2550, -0.2313, 0.0520, -0.0574, 0.0255, 0.1998, 1.2509, -0.2390, 0.0457, -0.0511, 0.0255, 0.1981, 1.2603, -0.2275, 0.0319, -0.0429, 0.0353, 0.2611, 0.4271, 0.7698, 0.1467, 0.0332, -0.0884, 0.2804, 0.4204, 0.7429, 0.1262, 0.0522, -0.0903, 0.3504, 0.4016, 0.6470, 0.1186, 0.0585, -0.1058, 0.3819, 0.3400, 0.5984, 0.1262, 0.0615, -0.1218, 0.4064, 0.2757, 0.5959, 0.1613, 0.0455, -0.1222, 0.4117, 0.2274, 0.6483, 0.1983, 0.0083, -0.1111, 0.4134, 0.2140, 0.6918, 0.2032, 0.0021, -0.0677, 0.4292, 0.2422, 0.5511, 0.1365, 0.0249, -0.0542, 0.4222, 0.2047, 0.5435, 0.1342, -0.0036, -0.0508, 0.4047, 0.1953, 0.6112, 0.1498, -0.0522, -0.0348, 0.3784, 0.2341, 0.6470, 0.1583, -0.0604, -0.0111, 0.3609, 0.2569, 0.6100, 0.1480, -0.0320, 0.0019, 0.3644, 0.2368, 0.5626, 0.1605, -0.0304, -0.0005, 0.3644, 0.2234, 0.6100, 0.1689, -0.0614, 0.0004, 0.3679, 0.2140, 0.6074, 0.1627, -0.0584, 0.0043, 0.3732, 0.2073, 0.5831, 0.1520, -0.0532, 0.0121, 0.3539, 0.1940, 0.5908, 0.1387, -0.0712, 0.0154, 0.3487, 0.2422, 0.6905, 0.0938, -0.0950, 0.0347, 0.3049, 0.2717, 0.6726, 0.0746, -0.0625, 0.0502, 0.2926, 0.3105, 0.6496, 0.0604, -0.0429, 0.0555, 0.2979, 0.3212, 0.6304, 0.0453, -0.0299, 0.0459, 0.2961, 0.3239, 0.5972, 0.0350, -0.0216, 0.0337, 0.3066, 0.3226, 0.5984, 0.0168, -0.0470, 0.0115, 0.3346, 0.2837, 0.6381, 0.0173, -0.0366, 0.0062, 0.3522, 0.2636, 0.6253, 0.0297, -0.0186, 0.0125, 0.3469, 0.2569, 0.6330, 0.0395, -0.0139, 0.0222, 0.3609, 0.2850, 0.6304, 0.0457, -0.0093, 0.0333, 0.3749, 0.2998, 0.5921, 0.0582, 0.0021, 0.0342, 0.3749, 0.2850, 0.5742, 0.0501, -0.0253, 0.0391, 0.3609, 0.3293, 0.6227, 0.0644, -0.0470, 0.0502, 0.3557, 0.3427, 0.6304, 0.0684, -0.0444, 0.0415, 0.3574, 0.3279, 0.5959, 0.0622, -0.0325, 0.0280, 0.3487, 0.3065, 0.6138, 0.0564, -0.0413, 0.0193, 0.3399, 0.3427, 0.6636, 0.0586, -0.0480, 0.0227, 0.3311, 0.3547, 0.6266, 0.0555, -0.0294, 0.0169, 0.3399, 0.3333, 0.6048, 0.0564, -0.0237, 0.0019, 0.3311, 0.3065, 0.6381, 0.0657, -0.0397, -0.0039, 0.3329, 0.2917, 0.6624, 0.0720, -0.0449, 0.0004, 0.3329, 0.2810, 0.6483, 0.0617, -0.0154, 0.0163, 0.3329, 0.2971, 0.6509, 0.0551, -0.0165, 0.0236, 0.3294, 0.3226, 0.6406, 0.0577, -0.0066, 0.0270, 0.3294, 0.3293, 0.6215, 0.0591, 0.0042, 0.0241, 0.3276, 0.3105, 0.6304, 0.0613, 0.0006, 0.0174, 0.3224, 0.2931, 0.6317, 0.0622, 0.0057, 0.0154, 0.3259, 0.3373, 0.5793, 0.0568, 0.0083, 0.0154, 0.3364, 0.3373, 0.5384, 0.0541, 0.0067, 0.0053, 0.3416, 0.2917, 0.5626, 0.0595, -0.0160, -0.0136, 0.3381, 0.2623, 0.6151, 0.0724, -0.0382, -0.0179, 0.3504, 0.2676, 0.6227, 0.0755, -0.0274, -0.0049, 0.3627, 0.2221, 0.5882, 0.0773, -0.0072, -0.0015, 0.3609, 0.2341, 0.6138, 0.0773, -0.0154, 0.0101, 0.3592, 0.2422, 0.6227, 0.0751, -0.0165, 0.0163, 0.3697, 0.2516, 0.6048, 0.0715, -0.0088, 0.0174, 0.3679, 0.2408, 0.5844, 0.0595, -0.0066, 0.0130, 0.3697, 0.2449, 0.5984, 0.0555, -0.0294, 0.0135, 0.3732, 0.2288, 0.6215, 0.0666, -0.0299, 0.0140, 0.3644, 0.2127, 0.6560, 0.0764, -0.0351, 0.0183, 0.3662, 0.2341, 0.6598, 0.0795, -0.0232, 0.0294, 0.3767, 0.2516, 0.6342, 0.0844, 0.0000, 0.0353, 0.3697, 0.2449, 0.6279, 0.1044, 0.0135, 0.0410, 0.3837, 0.2596, 0.5933, 0.1133, 0.0217, 0.0482, 0.3907, 0.2542, 0.5882, 0.1173, 0.0150, 0.0482, 0.3889, 0.2770, 0.5946, 0.1102, 0.0052, 0.0541, 0.3854, 0.3279, 0.5563, 0.0911, 0.0047, 0.0608, 0.3872, 0.3534, 0.5179, 0.0635, -0.0496, 0.0159, 0.3679, 0.2904, 0.6291, 0.0524, -0.0867, -0.0044, 0.3697, 0.2797, 0.6636, 0.0617, -0.0795, -0.0039, 0.3749, 0.2770, 0.6317, 0.0759, -0.0444, 0.0034, 0.3749, 0.2529, 0.6061, 0.0893, -0.0186, 0.0091, 0.3697, 0.2395, 0.6202, 0.1057, -0.0248, 0.0188, 0.3592, 0.2636, 0.6381, 0.1293, -0.0066, 0.0425, 0.3574, 0.2529, 0.6368, 0.1632, 0.0166, 0.0512, 0.3504, 0.2489, 0.6036, 0.1921, 0.0486, 0.0613, 0.3592, 0.2609, 0.5652, 0.2063, 0.0688, 0.0690, 0.3662, 0.2784, 0.5371, 0.2014, 0.0693, 0.0686, 0.3767, 0.2891, 0.4527, 0.1734, 0.0512, 0.0420, 0.4064, 0.2502, 0.4284, 0.1573, 0.0305, 0.0241, 0.4239, 0.2234, 0.4028, 0.1551, 0.0067, 0.0135, 0.4432, 0.2060, 0.3581, 0.1609, -0.0061, 0.0101, 0.4625, 0.1752, 0.3082, 0.1640, -0.0263, 0.0096, 0.4695, 0.1323, 0.3121, 0.1863, -0.1126, 0.0280, 0.4537, 0.1497, 0.3734, 0.1943, -0.1188, 0.0569, 0.4502, 0.1685, 0.3121, 0.1992, -0.0867, 0.0787, 0.4730, 0.1739, 0.2187, 0.2183, -0.0542, 0.0903, 0.5132, 0.1538, 0.0896, 0.2272, -0.0139, 0.0980, 0.5570, 0.1055, -0.0664, 0.2080, 0.0187, 0.0874, 0.5745, 0.0051, -0.3323, 0.1992, -0.1679, 0.0627, 0.5447, -0.1035, -0.2965, 0.1702, -0.3699, 0.0386, 0.5132, -0.2227, -0.1367, 0.1374, -0.5767, 0.0149, 0.4975, -0.3379, 0.0334, 0.1418, -0.7689, -0.0175, 0.4870, -0.4129, 0.2443, 0.1801, -0.8976, -0.1135, 0.2436, -0.9153, 0.6036, 0.1067, -0.8247, -0.5159, 0.1053, -1.3668, 0.6061, -0.0059, -0.7359, -0.2386, 0.0965, -1.1953, 0.6138, 0.0066, -0.7069, 0.2163, 0.0528, -0.7840, 0.7237, -0.0125, -0.7586, 0.5288, -0.0470, -0.7251, 0.8733, -0.0584, -0.8128, 0.5936, -0.0488, -0.6942, 0.9283, -0.1402, -0.7896, 0.6303, -0.0435, -0.7103, 1.1162, -0.1838, -0.8118, 0.6709, -0.0295, -0.6929, 1.2262, -0.2069, -0.8335, 0.7274, -0.0470, -0.7773, 1.3438, -0.2322, -0.8072, 0.7641, -0.0891, -0.9072, 1.4537, -0.2629, -0.7389, 0.8225, -0.1818, -0.8925, 1.5522, -0.4164, -0.5699, 1.1079, -0.2904, -0.4745, 1.6110, -0.3661, -0.4542, 1.2292, -0.3219, -0.3794, 1.6084, -0.3879, -0.3358, 1.2799, -0.3604, -0.3553, 1.6084, -0.3902, -0.2459, 1.3104, -0.4130, -0.2937, 1.6161, -0.4035, -0.1788, 1.3485, -0.4515, -0.1879, 1.6123, -0.4377, -0.1167, 1.5224, -0.6423, 0.0761, 1.6468, -0.4791, -0.0346, 1.4741, -0.7421, 0.2221, 1.6660, -0.4969, -0.0031, 1.4978, -0.8402, 0.3855, 1.6391, -0.5164, 0.0786, 1.5180, -0.9645, 0.5302, 1.6174, -0.5120, 0.1892, 1.5407, -1.0870, 0.6119, 1.6289, -0.4747, 0.2863, 1.6200, -1.3006, 1.2844, 1.6097, -0.4120, 0.5484, 1.7630, -1.2376, 1.3246, 1.5023, -0.4115, 0.7550, 1.5818, -1.3251, 1.3313, 1.2722, -0.3697, 0.9462, 1.3359, -1.2726, 1.3996, 1.1341, -0.3270, 0.9669, 1.1263, -1.3076, 1.3635, 1.0613, -0.2901, 0.9560, 0.9742, -1.3566, 1.4050, 0.9833, -0.2852, 1.0372, 0.7916, -1.3636, 1.6127, 0.7199, -0.3052, 1.0558, 0.6979, -1.3251, 1.6542, 0.6266, -0.2905, 1.0470, 0.5796
};

// Wrapper for raw input buffer
static signal_t sig;

// Setup function that is called once as soon as the program starts
void setup() {

    ei_impulse_result_t result; // Used to store inference output
    EI_IMPULSE_ERROR res;       // Return code from inference

    // Initialize serial (Arduino only) and wait to connect
#ifdef ARDUINO
    Serial.begin(115200);
    while (!Serial);
#endif

    // run_classifier() uses a callback function to fill its internal buffer
    // from your input_buf when requested. We need to assign the inference
    // buffer length and callback function here.
    sig.total_length = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;
    sig.get_data = &get_signal_data;

    // Call run_classifier(). Pass in the addresses for the sig struct and
    // the result struct. Set debug to false. Save the return code in the res
    // variable. See here for more information and an example:
    // https://docs.edgeimpulse.com/reference/c++-inference-sdk-library/functions/run_classifier
    // --- YOUR CODE HERE ---
    
    // --- END CODE ---

    // Print return code, time it took to perform inference, and inference
    // results. Note that the grader will ignore these outputs.
    ei_printf("run_classifier returned: %d\r\n", res);
    ei_printf("Timing: DSP %d ms, inference %d ms, anomaly %d ms\r\n", 
            result.timing.dsp, 
            result.timing.classification, 
            result.timing.anomaly);
    ei_printf("Predictions:\r\n");
    for (uint16_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
        ei_printf("  %s: ", ei_classifier_inferencing_categories[i]);
        ei_printf("%.5f\r\n", result.classification[i].value);
    }

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

    // Print the answer (line must begin with "ANS: " for the autograder)
    if (max_idx == -1) {
        ei_printf("Index of label with highest probability not set\r\n");
    } else {
        ei_printf("ANS: %s, %f\r\n", 
                    ei_classifier_inferencing_categories[max_idx], 
                    max_val);
    }
}

// Loop function that is called repeatedly after setup()
void loop() {

    // Just sleep
    ei_sleep(100);
}

// Callback: fill a section of the out_ptr buffer when requested
static int get_signal_data(size_t offset, size_t length, float *out_ptr) {
    for (size_t i = 0; i < length; i++) {
        out_ptr[i] = (input_buf + offset)[i];
    }

    return EIDSP_OK;
}