# TTFB Latency Analysis Tool: Windows Systems & Numerical Computing Framework

## Executive Overview
Developed as an Honors Calculus 2 Research collaboration, this framework bridges systems-level network engineering and computational math on Windows environments. The uses Time-To-First-Byte (TTFB) network telemetry, maps the behavior into a probability distribution, and implements numerical integration (Simpson’s Rule) to predict network latency boundaries. 

The primary objective is solving this problem: Given a localized sample vector of network ttfb, can we accurately model and quantify the probability of subsequent packet arrival times within arbitrary millisecond thresholds?

---

## Core Capabilities
* Stochastic Data Ingestion: Automates high-frequency, telemetry-driven HTTP/S requests using stream-based connection tracking to capture true packet headers prior to full body download.
* Statistical Modeling: Ingests raw millisecond latency datasets, extracts descriptive features (μ, σ), and dynamically normalizes target bounds via Z-score calculations.
* Numerical Integration Engine: Utilizes Simpson's Rule to solve the definite integral of the Probability Density Function (PDF) across tail distributions without relying on heavy external mathematical runtimes.
* Polymorphic Runtime Pipeline:ombines ultra-fast compiled code (like C++) with flexible Python script, passing data back and forth through a direct data pipeline(`popen/pclose`) to get both speed and adaptability. 
---

## Architectural Workflow & Mathematical Engine

### 1. Mathematical Modeling
The framework models the network latency as a continuous random variable following a normal distribution, X ~ N(μ, σ²), where the mean (μ) and standard deviation (σ) are derived from the vector. 

To compute the probability P(a ≤ X ≤ b) that the next request's latency falls within a specific window [a, b], the tool evaluates the definite integral of the Gaussian PDF:

P(a ≤ X ≤ b) = ∫_{a}^{b} [1 / (σ * √(2π))] * e^(-0.5 * ((x - μ) / σ)²) dx

Because this integral lacks an elementary antiderivative, the Numerical Integration Engine dynamically determines the optimal subinterval step-count (n) via a tailored analytical error-bound function and implements Simpson's Rule across n equally spaced subintervals:

∫_{a}^{b} f(x) dx ≈ (h / 3) * [f(x_0) + 4 * ∑ f(x_{2i-1}) + 2 * ∑ f(x_{2i}) + f(x_n)]

### 2. Runtime Pipeline
* Telemetry Ingestion Subsystem (Python): Executes remote polling iterations using isolated, streamlined connection reuse paradigms. Handles transport layer anomalies and passes raw stdout tokens cleanly to the host listener.
* Core Engine Execution Layer (C++): Main routine provisions dynamic error buffers, configures integration step increments across a verified infinity-boundary framework ([Observed_Max, μ + 5σ]), and parses standard stream buffers on the fly.

---

## Engineering Stack
* Systems Layer: C++17 (Chosen for memory management, raw stream extraction, and fast execution loops during numeric summation).
* Networking Layer: Python 3.8+ (Leveraging requests and sys subsystems to build zero-overhead HTTP session pools).
* Environment: Visual Studio (Optimized for PowerShell pipelines, native Win32 tracking, and standard MSVC/GCC toolchains).
* Version Control & Collaborations: Sending the code back and forth thorugh gmail.

---

## Verification and Validation
To ensure compliance with scientific computing standards, the numerical integration engine was verified against standard z-tables and algorithmic baselines:
* Baseline Validation: Verified that the total area under the curve approximates 1.0 by testing wide boundary thresholds ([-5σ, 5σ]).
* Adaptive Precision Test: An error-bound computation derives an absolute upper boundary limit for the fourth derivative of the Gaussian distribution, dynamically selecting a scale up to n subintervals to target floating-point precision down to 10^-6.

---

## Installation & Deployment

### Prerequisites
* Compiler: GCC / G++ (v9.0+) or MSVC supporting C++17
* Runtime: Python 3.8+ (Configured within Windows Environment Variables)
* Dependencies: Run 'pip install requests' via PowerShell

### Windows Implementation (PowerShell)
```powershell
# Compile the computational backend via g++
g++ -std=c++17 main.cpp ttfb_collector.cpp -o latency_analyzer.exe

# Execute the integrated data pipeline
./latency_analyzer.exe
