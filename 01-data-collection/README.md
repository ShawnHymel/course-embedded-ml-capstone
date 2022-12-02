# Data Collection Programming Assignment

Your task is to modify the *submission.cpp* code in the *source/* directory to read accelerometer and gyroscope data from the IMU for 1.5 seconds with a 100 Hz sampling rate. You then need to print the raw values to the terminal in a comma-separated values (CSV) format.

## 1. Install dependencies

If you are on Windows, install Windows Subsystem for Linux (WSL) by following [these instructions](https://learn.microsoft.com/en-us/windows/wsl/install). I will assume you are using the default Ubuntu flavor with WSL. Once done, start a WSL instance by opening a command prompt and entering `wsl`.

The basic terminal in macOS and most versions of Linux will work for this project. For Linux, I assume you are using a variant of Debian (e.g. Ubuntu), as I show installation with the aptitude package manager.

### Install Python

Follow [the instructions here](https://www.python.org/downloads/) to download and install the latest version of Python 3 for your operating system.

### Install gcc and make

#### For Linux and Windows/WSL:

In Windows, start a WSL instance by opening a command prompt and entering `wsl`.

```
sudo apt update
sudo apt install build-essential
```

#### For macOS:

Follow [the instructions here](https://brew.sh/) to install Homebrew.

Then, install *gcc* and *make*:

```
brew update
brew install gcc make
```

## 2. Download this repository

In a terminal (WSL if on Windows), download this repository:

```
curl https://github.com/ShawnHymel/course-embedded-ml-capstone/archive/refs/heads/main.zip -L -o main.zip
unzip -q main.zip
cd course-embedded-ml-capstone-main/
```

## 3. Modify submission.cpp

Open **01-data-collection/source/submission.cpp**. Follow the directions at the top of the document to modify the file with your own code.

When you are done, you can build and run the program on your computer with the following:

```
cd 01-data-collection
make -j
./build/app tests/alpha.9af6dce9cdd9.gf.csv
```

Note that we call the application and provide it with a test sample. It should print this to the console (truncated for brevity):

```
timestamp,accX,accY,accZ,gyrX,gyrY,gyrZ
0,-9.61,1.27,1.37,6.16,14.10,-3.78
10,-9.81,1.37,1.67,12.94,12.27,-6.16
20,-9.81,1.47,1.86,3.97,18.80,-9.28
30,-9.90,1.57,0.88,3.72,19.53,-10.62
40,-10.00,1.37,0.59,11.05,14.04,-13.24
...
1490,0.39,-8.43,11.96,2.44,-44.01,-55.48
```

Carefully check your output! The timestamp and gyroscope readings should match what you find in *tests/alpha.9af6dce9cdd9.gf.csv*. However, the accelerometer readings will be different (as you must convert from G-force to m/s^2 in your code).

Once you are happy with the way your program works to collect data, you can submit it for a grade or copy the code to Arduino to run it on real hardware.

## 4. (Optional) Submit code for grading

If you choose to pay for a certificate (optional), you will have access to the autograder. In the assignment section, go to **My submissions** and click **Create submission**. Submit only the file **submission.cpp** that you modified. Do not submit any other files!

After submission, wait a few minutes to see your grade. You might need to refresh the page. If there are errors, read the output of the grader. Feel free to post any issues you run into to the course discussion forums.

## 5. (Optional) Run program on real hardware

The *submission.cpp* file is structured so that it will run on an Arduino Nano 33 BLE Sense! Simply copy the code from that file into a new Arduino sktech. Make sure that you have installed the **Arduino Mbed OS nano Boards** package in *Tools > Boards Manager*.

After uploading the program to your Arduino board, open the Serial Terminal to see the output.

For data collection, you can run *serial-data-collect-csv.py* found in this directory to store the raw readings to a CSV file. With the Arduino running your collection code and connected to your computer, run the script as follows:

```
python serial-data-collect-csv.py -p <SERIAL_PORT> -b 115200 -d . -l <LABEL>
```

After each sample is collected, the Arduino will send the raw readings over the serial port, which will be read by the *serial-data-collect-csv.py* script. The script will save those raw readings in a CSV file with the *label* as the filename prefix.

This will help you collect your own dataset for machine learning training! Note that for future programming assignments, you will be given a dataset to work with (as it makes grading much easier).

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