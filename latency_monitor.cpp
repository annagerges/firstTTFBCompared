#define _USE_MATH_DEFINES
#include <iostream>
#include <cmath>
#include <numeric>
#include<vector>
#include <cstdio>
#include <sstream>
#include <direct.h>
#include <algorithm>
#include "latency_monitor.h"

using namespace std;

// The function for the Bell Curve
double bellCurveFunction(double x, double mu, double sigma) {
    double exponent = -0.5 * pow((x - mu) / sigma, 2);
    double constant = 1.0 / (sigma * sqrt(2 * M_PI));
    return constant * exp(exponent);
}

// Finds the n value for Simpson's Method
int errorBoundFormula(double a, double b, double sigma) {
    double k = 3.0 / (sqrt(2 * M_PI) * pow(sigma, 5));
    double n_raw = pow((k * pow(b - a, 5)) / 0.0009, 0.25);

    int n = ceil(n_raw);

    if (n % 2 != 0) {
        n++;
    }
    return n;
}

// Collects latency data by running the Python script
vector<double> collectLatencyData(int count) {
    vector<double> data;

    // Get absolute current working directory and store it as a c-string so the python script can be executed correctly
    char cwd[256];

    // If the current working directory is successfully obtained, print it to the console
    if (_getcwd(cwd, sizeof(cwd)) != nullptr) {
        cout << "Current working directory: " << cwd << endl;
    }

    // Path to Python script if it's in the same folder as the c++ file
    string script_path = "module1.py";
    string command = "python \"" + script_path + "\" " + to_string(count);

    cout << "Running command: " << command << endl;

    // Open a pipe to the Python script
    FILE* pipe = _popen(command.c_str(), "r");

    // If pipe isn't open
    if (!pipe) {
        cerr << "Error: Could not execute Python script." << endl;
        return data;
    }

    // Read output line by line
    char buffer[256];
    cout << "Python output:" << endl;
    int line_count = 0;

    // While the Python script line can be read
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        //increase line count and print the data onto console
        line_count++;
        cout << "Data point " << line_count << ": " << buffer;

        try {
            //convert buffer array to double and add it to data vector
            double value = stod(buffer);
            data.push_back(value);
            //print the value that was parsed to the console
            cout << "  -> Parsed as: " << value << endl;
        }
        catch (const exception& e) {
            cout << "  -> Could not parse (not a number)" << endl;
        }
    }

    //close data pipe and print the exit status of the python script and how many data points were collected
    int status = _pclose(pipe);
    cout << "Python exited with code: " << status << endl;
    cout << "\nSuccessfully collected " << data.size() << " data points." << endl << endl;

    return data;
}

// Calculate mean and standard deviation from collected data
void dataMunch(const vector<double>& data, double& mean, double& std_dev) {
    // n is the number of points in your data list
    int n = data.size();

    double sum = 0.0;
    for (int point = 0; point < n; point++) {
        sum += data[point];
    }
    // Calculate your project's mean
    mean = sum / n;

    double squared_diff_sum = 0.0;
    for (int point = 0; point < n; point++) {
        double diff = data[point] - mean;
        squared_diff_sum += diff * diff;
    }

    // Calculate your project's std_dev
    std_dev = sqrt(squared_diff_sum / n);
}
