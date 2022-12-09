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
 *   https://studio.edgeimpulse.com/public/126176/v4
 * 
 * We have copied the "raw features" from the test sample alpha.a74f589ed1b9
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
    0.3276, -1.3815, -1.1454, -0.0219, -0.0729, 0.1570, 0.2734, -1.3815, -1.1007, 0.0582, -0.0790, 0.1081, 0.2559, -1.3333, -1.0163, 0.1156, -0.1100, 0.1241, 0.2786, -1.2180, -1.0009, 0.1245, -0.0925, 0.1608, 0.4187, -2.3755, -0.8935, 0.1422, -0.0764, 0.2048, 0.2716, -1.1296, -0.8897, -0.1518, 0.2512, 1.4388, 0.2839, -1.0359, -0.9971, -0.1224, 0.4584, 1.4021, 0.3767, -0.9849, -0.9882, -0.0681, 0.4832, 1.3051, 0.5132, -0.9005, -1.0239, 0.0093, 0.5039, 1.1974, 0.4992, -0.9957, -1.0265, 0.1351, 0.4739, 1.0525, 0.4835, -1.1055, -1.1134, 0.1672, 0.4961, 0.8631, 0.4222, -0.8081, -1.1659, 0.1049, 0.5390, 0.7820, 0.5570, -0.4518, -1.4970, 0.2067, 0.5577, 0.4767, 0.5588, -0.7478, -1.4740, 0.2997, 0.2868, -0.0894, 0.4484, -1.2730, -1.2183, 0.3219, -0.1178, -0.4691, 0.5185, -1.2877, -0.9715, 0.1049, -0.1617, -0.1720, 0.8967, -0.9273, -1.2528, 0.0653, -0.1173, -0.0962, 0.6288, -0.8349, -0.9460, 0.1511, -0.5566, 0.0250, 0.5588, -1.0827, -0.8104, -0.0472, -0.7064, 0.0009, 0.6866, -1.3641, -1.0035, -0.0660, -0.4805, -0.0498, 0.6585, -1.3225, -1.0661, -0.0277, -0.4237, 0.2076, 0.5990, -1.2020, -1.1902, 0.0017, -0.4707, 0.1999, 0.5728, -1.1886, -1.0981, 0.0199, -0.5431, 0.1685, 0.5938, -1.1752, -1.0687, 0.0279, -0.5617, 0.1584, 0.6340, -1.1243, -1.0853, 0.0190, -0.5462, 0.1574, 0.6568, -1.0693, -1.0431, 0.0243, -0.5529, 0.1386, 0.6813, -1.0693, -0.9549, 0.0457, -0.4930, 0.0372, 0.6918, -1.1002, -0.9575, 0.0386, -0.4175, 0.0048, 0.7146, -1.1176, -0.9511, 0.0346, -0.3508, -0.0083, 0.7058, -1.1524, -0.9434, 0.0350, -0.2992, -0.0049, 0.3311, 0.0640, 0.7071, 0.0573, 0.0269, -0.1150, 0.3189, 0.0412, 0.7531, 0.1129, 0.0249, -0.1039, 0.3241, 0.0024, 0.6956, 0.1124, 0.1050, -0.0914, 0.3224, 0.0051, 0.6509, 0.0951, 0.1266, -0.0793, 0.3416, 0.0158, 0.6279, 0.0884, 0.1168, -0.0763, 0.3434, -0.0070, 0.5984, 0.0795, 0.1091, -0.0903, 0.3452, -0.0405, 0.5959, 0.0778, 0.0988, -0.0986, 0.3522, -0.0539, 0.6138, 0.0915, 0.0817, -0.0903, 0.3609, -0.0686, 0.5703, 0.0938, 0.0956, -0.0822, 0.3679, -0.0633, 0.5307, 0.0951, 0.1034, -0.0735, 0.3679, -0.0686, 0.5230, 0.1031, 0.0878, -0.0720, 0.3732, -0.0767, 0.5384, 0.1097, 0.0636, -0.0720, 0.3819, -0.0954, 0.5166, 0.1044, 0.0590, -0.0768, 0.3959, -0.1436, 0.4834, 0.0973, 0.0377, -0.0740, 0.3994, -0.1678, 0.5026, 0.1031, 0.0094, -0.0662, 0.3942, -0.1664, 0.5384, 0.1031, -0.0098, -0.0527, 0.3837, -0.1490, 0.5626, 0.0928, -0.0171, -0.0411, 0.3784, -0.1329, 0.5716, 0.0702, -0.0129, -0.0377, 0.3767, -0.1544, 0.5563, 0.0395, -0.0113, -0.0459, 0.3732, -0.1624, 0.5435, 0.0399, -0.0124, -0.0430, 0.3784, -0.1825, 0.5575, 0.0479, -0.0294, -0.0348, 0.3802, -0.2133, 0.5754, 0.0608, -0.0444, -0.0271, 0.3872, -0.2133, 0.5895, 0.0640, -0.0470, -0.0097, 0.3942, -0.1838, 0.5665, 0.0582, -0.0429, 0.0289, 0.3819, -0.1356, 0.5742, 0.0533, -0.0640, 0.0319, 0.3942, -0.1396, 0.6048, 0.0435, -0.0816, 0.0289, 0.3872, -0.1302, 0.6240, 0.0391, -0.0827, 0.0314, 0.3714, -0.0968, 0.6330, 0.0310, -0.0729, 0.0386, 0.3714, -0.0740, 0.6432, 0.0293, -0.0579, 0.0342, 0.3627, -0.0485, 0.6368, 0.0315, -0.0454, 0.0328, 0.3644, -0.0458, 0.6163, 0.0289, -0.0372, 0.0289, 0.3732, -0.0445, 0.6087, 0.0315, -0.0418, 0.0255, 0.3802, -0.0485, 0.6202, 0.0386, -0.0475, 0.0241, 0.3784, -0.0753, 0.6227, 0.0448, -0.0511, 0.0222, 0.3819, -0.0646, 0.6304, 0.0524, -0.0542, 0.0391, 0.3837, -0.0365, 0.6432, 0.0555, -0.0563, 0.0482, 0.3889, -0.0311, 0.6406, 0.0555, -0.0557, 0.0444, 0.3854, -0.0338, 0.6355, 0.0520, -0.0522, 0.0386, 0.3907, -0.0298, 0.6406, 0.0488, -0.0537, 0.0353, 0.3977, -0.0325, 0.6253, 0.0439, -0.0511, 0.0275, 0.4012, -0.0244, 0.6253, 0.0511, -0.0516, 0.0265, 0.4047, -0.0258, 0.6240, 0.0577, -0.0522, 0.0250, 0.4064, -0.0338, 0.5984, 0.0564, -0.0454, 0.0202, 0.4187, -0.0445, 0.5601, 0.0564, -0.0418, 0.0202, 0.4169, -0.0525, 0.5371, 0.0680, -0.0599, 0.0270, 0.4099, -0.0325, 0.5320, 0.0697, -0.0687, 0.0289, 0.4064, -0.0271, 0.5102, 0.0653, -0.0707, 0.0280, 0.4099, -0.0231, 0.4859, 0.0622, -0.0729, 0.0261, 0.4082, -0.0043, 0.4629, 0.0608, -0.0780, 0.0261, 0.4012, 0.0238, 0.4361, 0.0630, -0.0930, 0.0212, 0.3994, 0.0211, 0.3696, 0.0640, -0.1075, 0.0096, 0.3977, 0.0225, 0.3300, 0.0680, -0.1214, 0.0072, 0.3977, 0.0158, 0.2929, 0.0649, -0.1343, 0.0029, 0.3977, -0.0284, 0.2660, 0.0604, -0.1643, -0.0126, 0.4012, -0.0713, 0.2584, 0.0693, -0.2046, -0.0247, 0.3942, -0.1718, 0.2392, 0.1057, -0.2470, -0.0271, 0.3924, -0.2749, 0.2251, 0.1169, -0.2630, -0.0285, 0.3732, -0.3566, 0.2264, 0.1271, -0.2920, -0.0126, 0.3311, -0.3821, 0.2507, 0.1222, -0.3286, 0.0193, 0.3189, -0.3633, 0.2916, 0.0987, -0.3716, 0.0507, 0.2909, -0.3459, 0.3785, 0.0506, -0.4387, 0.0879, 0.2331, -0.3312, 0.4156, 0.0372, -0.4500, 0.1154, 0.2016, -0.2950, 0.4693, 0.0137, -0.4692, 0.1463, 0.1788, -0.2267, 0.5345, -0.0059, -0.4888, 0.1642, 0.1666, -0.2173, 0.5754, -0.0214, -0.4878, 0.1550, 0.1666, -0.2414, 0.5447, -0.0112, -0.4423, 0.1516, 0.1753, -0.3700, 0.5166, 0.0346, -0.4635, 0.1642, 0.1666, -0.4625, 0.5729, 0.0871, -0.5053, 0.1869, 0.1858, -0.5563, 0.6151, 0.1182, -0.5219, 0.2260, 0.2086, -0.6353, 0.6074, 0.1449, -0.4991, 0.2840, 0.1911, -0.6674, 0.5767, 0.1836, -0.4723, 0.4337, 0.1456, -0.6782, 0.5831, 0.2054, -0.4816, 0.4907, 0.1193, -0.6728, 0.5869, 0.1653, -0.4920, 0.5245, 0.0790, -0.6541, 0.6074, 0.1000, -0.4971, 0.5443, 0.0458, -0.6407, 0.6317, 0.0075, -0.5110, 0.5462, 0.0107, -0.6460, 0.6636, -0.0935, -0.5301, 0.5337, -0.0926, -0.7063, 0.8030, -0.2367, -0.6118, 0.5288, -0.1101, -0.7224, 0.8605, -0.3212, -0.6511, 0.5496, -0.1381, -0.7023, 0.9270, -0.3760, -0.6764, 0.5999, -0.2011, -0.6768, 1.0101, -0.3853, -0.6780, 0.6631, -0.2641, -0.6621, 1.0919, -0.3755, -0.6615, 0.7288, -0.3377, -0.6259, 1.1559, -0.3657, -0.5896, 0.8602, -0.4672, -0.5308, 1.1738, -0.3959, -0.5488, 0.9152, -0.4672, -0.4089, 1.2070, -0.4235, -0.5208, 0.9959, -0.5373, -0.2200, 1.2517, -0.4382, -0.4898, 1.0732, -0.6055, -0.0740, 1.3259, -0.4787, -0.4206, 1.0973, -0.6178, -0.0070, 1.3937, -0.6553, -0.1824, 1.0191, -0.6791, 0.0747, 1.2390, -0.7433, -0.0867, 0.9462, -0.7246, 0.1779, 1.1891, -0.8310, -0.0790, 0.9244, -0.7614, 0.3601, 1.1993, -0.8892, -0.1043, 0.9558, -0.8086, 0.4431, 1.2645, -0.9070, -0.1255, 0.9795, -0.8997, 0.5128, 1.3272, -0.8807, -0.1193, 1.0346, -0.9872, 1.0647, 1.5113, -0.7620, 0.0470, 1.2718, -0.9522, 1.2416, 1.5164, -0.6628, 0.2475, 1.0732, -0.8034, 1.0835, 1.3860, -0.5303, 0.4383, 0.8337, -0.8787, 1.1197, 1.4307, -0.3888, 0.5489, 0.7375, -0.8296, 1.1183, 1.1968, -0.2580, 0.7675, 0.7245, -0.7509, 1.1210, 0.8823, -0.1967, 0.8176, 0.5347, -0.6231, 0.9844, 0.8925, -0.1513, 0.7096, 0.3922, -0.6055, 1.0888, 0.9130, -0.1180, 0.6801, 0.3395, -0.5653, 1.0540, 1.0050, -0.1073, 0.6894, 0.2826, -0.5495, 1.0902, 0.8963, -0.0868, 0.7808, 0.2443, -0.5845, 1.0513, 0.8708, -0.0077, 0.8899, 0.2062, -0.5040, 1.1143, 0.7915, 0.0337, 1.0067, 0.1796, -0.4742, 1.1089, 0.6777, 0.0733, 1.0857, 0.1313, -0.4620, 1.1170, 0.6189, 0.1298, 1.0982, 0.0951, -0.4130, 1.0741, 0.5486, 0.1782, 1.0857, 0.0642, -0.3569, 1.1625, 0.5409, 0.2370, 1.0542, 0.0062, -0.2309, 1.1290, 0.5665, 0.3330, 1.0940, -0.1039, -0.1941, 1.0554, 0.3785, 0.3166, 1.1824, -0.1899, -0.1101, 1.0902, 0.2571, 0.2966, 1.2149, -0.2237, -0.0348, 1.1143, 0.2008, 0.3010, 1.1628, -0.2816, 0.0230, 1.0286, 0.1817, 0.3148, 1.0682, -0.3879, 0.1018, 0.7392, 0.1842, 0.3344, 0.9370, -0.5521, 0.1316, 0.6106, 0.1983, 0.3171, 0.8909, -0.5792, 0.1718, 0.5463, 0.1983, 0.2801, 0.8656, -0.5773, 0.2121, 0.5811, 0.1599, 0.2446, 0.8604, -0.5449, 0.2331, 0.6615, 0.0909, 0.2134, 0.8631, -0.5227, 0.2524, 0.7111, -0.0574, 0.1729, 0.8398, -0.5714, 0.3084, 0.7713, -0.1213, 0.1898, 0.7969, -0.6198
};

// Wrapper for raw input buffer
static signal_t sig;

// Setup function that is called once as soon as the program starts
void setup() {

    ei_impulse_result_t result; // Used to store inference output
    EI_IMPULSE_ERROR res;       // Return code from inference

    // Initialize serial (Arduino only)
#ifdef ARDUINO
    Serial.begin(115200);
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