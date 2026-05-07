#define _USE_MATH_DEFINES
#include <iostream>
#include <cmath>
#include <iomanip>
#include <numeric>
#include<vector>
#include <cstdio>
#include <sstream>
#include <direct.h>
#include <algorithm>

using namespace std;

int errorBoundFormula(double, double, double);
double bellCurveFunction(double, double, double);
void dataMunch(const vector<double>& data, double& mean, double& std_dev);
vector<double> collectLatencyData(int count);

int main() {
    // Project data
    int num_measurements;

    cout << "Enter amount of data points would you like to collect (minimum: 10): ";
    cin >> num_measurements;

    while (num_measurements < 10) {
        cout << "Enter at least 10 data points.";
        cout << "Enter amount of data points would you like to collect (minimum: 10): ";
        cin >> num_measurements;
    }

    // Collect latency data from the Python script
    cout << "Collecting " << num_measurements << " latency measurements..." << endl;
    vector<double> latency_data = collectLatencyData(num_measurements);

    //if no data was collected, exit the program with an error message
    if (latency_data.empty()) {
        cerr << "Error: No latency data collected. Make sure Python script is accessible." << endl;
        return 1;
    }

    // Calculate mean and std_dev from collected data
    double mean, std_dev;
    dataMunch(latency_data, mean, std_dev);

    // Select observed value (highest latency in dataset)
    double observed_x = *max_element(latency_data.begin(), latency_data.end());

    // Integrate to "infinity" (5 standard deviations)
    double a = observed_x;
    double b = mean + 5 * std_dev;

    // n must be an even number for Simpson's Rule
    int n = errorBoundFormula(a, b, std_dev);
    double h = (b - a) / static_cast<double>(n); // This is Delta X / 3

    // Start with adding the first and last values of Simpson's rule
    double sum = bellCurveFunction(a, mean, std_dev) + bellCurveFunction(b, mean, std_dev);

    // Add all values within the summation formula to sum
    for (int i = 1; i < n; i++) {
        double current_x = a + i * h;

        // Toggles between 2 and 4
        double coefficient = 3.0 + pow(-1, i - 1);

        sum += coefficient * bellCurveFunction(current_x, mean, std_dev);
    }

    // Final step for Simpson's rule which is to
    // multiply everything by h (delta X) / 3
    double probability = (h / 3.0) * sum;

    // Outputs the results
    double z_score = (observed_x - mean) / std_dev;

    cout << fixed << setprecision(6);
    cout << "--- Google Latency Analysis Project ---" << endl;
    cout << "Observed Latency: " << observed_x << " ms" << endl;
    cout << "Calculated Z-score: " << z_score << endl;
    cout << "Probability (Area under tail): " << probability << endl;
    cout << "Percentile: " << (1.0 - probability) * 100 << endl;

    return 0;
}

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
    cout << "\nSuccessfully collected " << data.size() << " data points." << endl<<endl;

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
