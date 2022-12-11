# Inference With Static Input Programming Assignment

Your task is to modify the *submission.cpp* code in the *source/* directory to perform inference using the given "magic wand" Edge Impulse C++ SDK and several test CSV files. You need to determine the label with the highest predicted value (output of the model) and print it to the console.

## 1. Install dependencies

Make sure you have all of the dependencies installed and have downloaded this repository to your computer. See [parts 1 and 2 from the Data Collection assignment README](../01-data-collection/README.md) to repeat the steps if needed.

## 2. Modify submission.cpp

Open **06-inference-with-static-input/source/submission.cpp**. Follow the directions at the top of the document to modify the file with your own code.

When you are done, you can build and run the program on your computer with the following:

```
cd 06-data-collection
make -j
./build/app tests/alpha.07550d51428f.csv
```

Note that you provided a test sample (in CSV format) to the application. Your code should print the following to the console:

```
ANS: alpha, 1.000000
```

You may see other information printed (and you are welcome to use the `ei_printf()` function to print your own information). However, only lines that start with `ANS:` will be considered by the autograder.

Try the different test files with the project! You should get the following output. If you do, then you know that your code is working. Some misclassifications are OK! I put them here on purpose so you can check that your solution matches mine (and what the grader will expect).

|         Filename          |           Output         |
|---------------------------|--------------------------|
| _idle.0c0bb1db5f98.csv     | ANS: _idle, 1.000000    |
| _idle.2ea19a4d31be.csv     | ANS: _idle, 1.000000    |
| _unknown.7f8f640c175f.csv | ANS: alpha, 1.000000     |
| _unknown.b9919a398a76.csv  | ANS: _unknown, 1.000000 |
| alpha.07550d51428f.csv     | ANS: alpha, 1.000000    |
| alpha.69d4ba7eca3b.csv     | ANS: beta, 1.000000     |
| beta.04c2828e445f.csv      | ANS: beta, 1.000000     |
| beta.ebc0224b155a.csv      | ANS: beta, 1.000000     |
| gamma.1affb24ae350.csv     | ANS: gamma, 1.000000    |
| gamma.d7d5f7d5fe3d.csv     | ANS: gamma, 0.999690    |

Also, note that the autograder uses different files than what you have in the *tests/* folder. So, you can't just print these numbers to the console for a good grade!

## 3. (Optional) Submit code for grading

If you choose to pay for a certificate (optional), you will have access to the autograder. In the assignment section, go to **My submissions** and click **Create submission**. Submit only the file **submission.cpp** that you modified. Do not submit any other files!

The autograder looks for any lines printed to the console that start with `ANS:`, so make sure your answer is on that line. It must include the name of the label, followed by a comma (,), then the predicted probability of that label. The autograder ignores any other values printed to the console.

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