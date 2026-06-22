# Server TTFB Performance Analyzer (C++ & Python)

## Project Overview
This was a colaborative project to explore how low-level network performance connects with mathematical modeling. The program pings a server multiple times (up to user input) to collect accurate Time-To-First-Byte (TTFB) data, maps that packet arrival behavior onto a bell curve, and uses Simpson's Rule (a calculus method) to calculate the probability of future network lag spikes.

Instead of relying on heavy scientific libraries, I wrote the program integration engine and the data parsing.

---

## Key Features
* True TTFB Tracking: The Python script uses stream-based polling to measure exactly when the first byte of data returns from a server, making the tracking highly accurate.
* Custom Math Engine: Implements Simpson's Rule using a dynamic error-bound formula to calculate the exact number of steps needed for high precision.
* Program Interoperability: Uses standard Windows pipes (`_popen` and `_pclose`) to send data seamlessly from the Python tracking script straight into the C++ math engine.

---

## How It Works

### 1. Collecting Telemetry
The Python script runs a loop to ping a target URL. It handles potential connection errors or timeouts safely so the main program doesn't crash, then prints the exact millisecond values to the console stream.

### 2. Processing the Math
The C++ program reads the data points, calculates the mean and standard deviation, and determines a window between the highest observed TTFB and a "worst-case scenario" threshold. It then runs Simpson's Rule to find the exact area under the tail of the bell curve, giving us the percentile ranking.

---

## Technical Skills Shown
* Code inter-process communication (IPC) using standard input/output pipes.
* Applied calculus 2 concepts to real-world software without using pre-made math libraries.
* Handled memory and loops efficiently (such as optimizing the alternating coefficients inside the integration loop).
* Collaborative development and source control with my co-author, Nico.

---

## How to Build and Run
Built and tested on Windows using Visual Studio (MSVC) and GCC.

```powershell
# Compile the C++ backend
g++ -std=c++17 main.cpp ttfb_collector.cpp -o ttfb_analyzer.exe

# Run the application
./ttfb_analyzer.exe
