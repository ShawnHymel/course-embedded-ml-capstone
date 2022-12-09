/**
 * Main application entrypoint for the static inferencing assignment
 *
 * Simply call the setup and loop functions in the submission file.
 * 
 * Author: Shawn Hymel (EdgeImpulse, Inc.)
 * Date: August 28, 2022
 * License: Apache-2.0
 */

#include <stdio.h>
#include <cstdlib>

#include "submission.h"



/*******************************************************************************
 * Main
 */

// Main function to call setup and loop
int main(int argc, char **argv) {

    // Call user supplied setup() function once
    setup();

    // Call user supplied loop() function a few times
    for (int i = 0; i < 3; i++) {
        loop();
    }

    // We're done here
    return 0;
}