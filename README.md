# TTFB Latency Analysis Tool

## Overview
This project measures and analyzes Time To First Byte (TTFB) latency for requests made to `google.com`. It collects a minimum of 10 data points per run and performs basic statistical analysis to compare initial request latency with subsequent requests.

The project was developed as an honors collaboration between Nico and I, aiming to combine programming concepts with Calculus II ideas related to data behavior and variation over repeated trials.

---

## What It Does
- Sends repeated requests to `google.com`
- Measures TTFB (Time To First Byte) for each request
- Collects at least 10 data samples per run
- Analyzes latency differences across requests using simpsons rule and other Calc 2 concepts
- Displays results for comparison and interpretation

---

## Technologies Used
- C++
- Python
- Windows PowerShell environment
- Requests library (Python)

---

## What I Learned
- How to build a data pipeline between C++ and Python
- Differences in performance between programming languages
- Basics of network latency and request timing (TTFB)
- How to configure and manage Windows dev environments
- Using Windows explorer for workflow
- Integration of human code and agent-based execution and data handling systems

---

## Platform Compatibility
- Currently supports Windows only
- A macOS version exists separately

---

## How to Run (Windows)
1. Ensure Python is installed
2. Install required library:
3. Configure PowerShell to run Python scripts
4. Run the C++ program to initiate the pipeline
5. Execute Python script when triggered by the C++ process

---

## Future Improvements
- Support multiple websites instead of only google.com
- Allow user selection of target URL
- Improve performance and optimize data pipeline
- Add graphical visualization, catchy background music, and loading screens.
- Expand cross-platform support (macOS/Linux)

---

## Notes
This project is experimental and focused on learning concepts related to networking latency, data pipelines, and cross-language integration.
