# Inference With Static Input Programming Assignment

Your task is to modify the *submission.cpp* code in the *source/* directory to perform inference using the given "magic wand" Edge Impulse C++ SDK and provided raw features from a test sample. You then need to determine the label with the highest predicted value (output of the model) and print it to the console.

## 1. Install dependencies

Make sure you have all of the dependencies installed and have downloaded this repository to your computer. See [parts 1 and 2 from the Data Collection assignment README](../01-data-collection/README.md) to repeat the steps if needed.

## 2. Modify submission.cpp

Open **05-inference-with-static-input/source/submission.cpp**. Follow the directions at the top of the document to modify the file with your own code.

When you are done, you can build and run the program on your computer with the following:

```
cd 01-data-collection
make -j
./build/app
```

Note that we do not need to call the application with a test sample, as we copied in our sample to the static buffer inside submission.cpp. When you run the program, it should print the following to the console:

```
run_classifier returned: 0
Timing: DSP 1 ms, inference 2 ms, anomaly 0 ms
Predictions:
  _idle: 0.00551
  _unknown: 0.00186
  alpha: 0.73188
  beta: 0.00907
  gamma: 0.25167
ANS: alpha, 0.731882
```

Carefully check your output! If you see the following, then you know that you performed inference correctly.

## 3. (Optional) Submit code for grading

If you choose to pay for a certificate (optional), you will have access to the autograder. In the assignment section, go to **My submissions** and click **Create submission**. Submit only the file **submission.cpp** that you modified. Do not submit any other files!

The autograder looks for any lines printed to the console that start with `ANS:`, so make sure your answer is on that line. It must include the name of the label, followed by a comma (,), then the predicted probability of that label: `ANS: alpha, 0.7318821`. The autograder ignores any other values printed to the console.

After submission, wait a few minutes to see your grade. You might need to refresh the page. If there are errors, read the output of the grader. Feel free to post any issues you run into to the course discussion forums.

## 4. (Optional) Run program on real hardware

The *submission.cpp* file is structured so that it will run on an Arduino Nano 33 BLE Sense! Simply copy the code from that file into a new Arduino sktech. Make sure that you have installed the **Arduino Mbed OS nano Boards** package in *Tools > Boards Manager*.

After uploading the program to your Arduino board, open the Serial Monitor to see the output. Note that you might need to reset the board and re-open the Serial Monitor to see the output. The output values should be the same as above.

## License

Unless otherwise noted, all code and datasets in this repository are licensed as follows:

Copyright 2022 EdgeImpulse, Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

[http://www.apache.org/licenses/LICENSE-2.0](http://www.apache.org/licenses/LICENSE-2.0)

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.