
#include <Humidity_Derivitive_2.0_inferencing.h>


static const float features[] = {
    // copy raw features here (for example from the 'Live classification' page)
    // see https://docs.edgeimpulse.com/docs/running-your-impulse-arduino
    -0.0000, -0.0274, -0.0592, -0.0843, -0.0978, -0.1046, -0.1157, -0.1397, -0.1762, -0.2164, -0.2502, -0.2745, -0.2963, -0.3268, -0.3729, -0.4288, -0.4785, -0.5079, -0.5142, -0.4996, -0.4703, -0.4324, -0.3751, -0.2870, -0.1712, -0.0440, 0.0715, 0.1502, 0.1770, 0.1635, 0.1301, 0.0874, 0.0477, 0.0118, -0.0281, -0.0728, -0.1122, -0.1376, -0.1486, -0.1441, -0.1301, -0.1180, -0.0958, -0.0353, 0.0977, 0.3338, 0.6845, 1.1224, 1.5871, 2.0195, 2.3816, 2.6675, 2.9135, 3.1633, 3.4347, 3.7091, 3.9458, 4.1051, 4.1748, 4.1870, 4.1928, 4.2272, 4.2999, 4.3871, 4.4498, 4.4646, 4.4390, 4.3998, 4.3749, 4.3813, 4.4130, 4.4438, 4.4522, 4.4279, 4.3734, 4.3030, 4.2337, 4.1731, 4.1156, 4.0519, 3.9727, 3.8733, 3.7653, 3.6630, 3.5752, 3.5060, 3.4491, 3.3917, 3.3276, 3.2603, 3.1921, 3.1200, 3.0473, 2.9693, 2.8849, 2.8084, 2.7504, 2.7083, 2.6700, 2.6167, 2.5317, 2.4199, 2.3125, 2.2369, 2.2048, 2.2079, 2.2100, 2.1731, 2.0867, 1.9664, 1.8412, 1.7415, 1.6810, 1.6434, 1.6019, 1.5406, 1.4521, 1.3435, 1.2337, 1.1316, 1.0307, 0.9197, 0.7867, 0.6221, 0.4303, 0.2297, 0.0341, -0.1499, -0.3217, -0.4955, -0.6905, -0.9133, -1.1591, -1.4159, -1.6651, -1.8883, -2.0802, -2.2420, -2.3732, -2.4776, -2.5617, -2.6310, -2.6933, -2.7586, -2.8296, -2.9000, -2.9641, -3.0194, -3.0667, -3.1130, -3.1621, -3.2043, -3.2228, -3.2058, -3.1545, -3.0885, -3.0404, -3.0358, -3.0757, -3.1392, -3.1921, -3.2036, -3.1657, -3.0911, -3.0014, -2.9174, -2.8491, -2.7945, -2.7464, -2.6958, -2.6318, -2.5441, -2.4321, -2.2986, -2.1564, -2.0305, -1.9430, -1.9024, -1.9031, -1.9257, -1.9413, -1.9271, -1.8846, -1.8286, -1.7827, -1.7709, -1.7922, -1.8165, -1.8029, -1.7255, -1.5911, -1.4459, -1.3583, -1.3622, -1.4280, -1.4734, -1.3984, -1.1470, -0.7529, -0.3282
};

/**
 * @brief      Copy raw feature data in out_ptr
 *             Function called by inference library
 *
 * @param[in]  offset   The offset
 * @param[in]  length   The length
 * @param      out_ptr  The out pointer
 *
 * @return     0
 */
int raw_feature_get_data(size_t offset, size_t length, float *out_ptr) {
    memcpy(out_ptr, features + offset, length * sizeof(float));
    return 0;
}


/**
 * @brief      Arduino setup function
 */
void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);

    Serial.println("Edge Impulse Inferencing Demo");
}

/**
 * @brief      Arduino main function
 */
void loop()
{
    ei_printf("Edge Impulse standalone inferencing (Arduino)\n");

    if (sizeof(features) / sizeof(float) != EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
        ei_printf("The size of your 'features' array is not correct. Expected %lu items, but had %lu\n",
            EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, sizeof(features) / sizeof(float));
        delay(1000);
        return;
    }

    ei_impulse_result_t result = { 0 };

    // the features are stored into flash, and we don't want to load everything into RAM
    signal_t features_signal;
    features_signal.total_length = sizeof(features) / sizeof(features[0]);
    features_signal.get_data = &raw_feature_get_data;

    // invoke the impulse
    EI_IMPULSE_ERROR res = run_classifier(&features_signal, &result, false /* debug */);
    ei_printf("run_classifier returned: %d\n", res);

    if (res != 0) return;

    // print the predictions
    ei_printf("Predictions ");
    ei_printf("(DSP: %d ms., Classification: %d ms., Anomaly: %d ms.)",
        result.timing.dsp, result.timing.classification, result.timing.anomaly);
    ei_printf(": \n");
    ei_printf("[");
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
        ei_printf("%.5f", result.classification[ix].value);
#if EI_CLASSIFIER_HAS_ANOMALY == 1
        ei_printf(", ");
#else
        if (ix != EI_CLASSIFIER_LABEL_COUNT - 1) {
            ei_printf(", ");
        }
#endif
    }
#if EI_CLASSIFIER_HAS_ANOMALY == 1
    ei_printf("%.3f", result.anomaly);
#endif
    ei_printf("]\n");

    // human-readable predictions
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
        ei_printf("    %s: %.5f\n", result.classification[ix].label, result.classification[ix].value);
    }
#if EI_CLASSIFIER_HAS_ANOMALY == 1
    ei_printf("    anomaly score: %.3f\n", result.anomaly);
#endif

    delay(1000);
}
