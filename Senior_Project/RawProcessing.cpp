#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

double safe_stod(string val) {
    double number = 0.0;
    stringstream x(val);
    x >> number;
    return number;
}

double SafeThreshold(const string testFile) {
    ifstream file(testFile);
    string line, val, classification;
    double maxAccel = 0.0, x, y, z, magnitude;

    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);
        getline(ss, val, ','); x = safe_stod(val);
        getline(ss, val, ','); y = safe_stod(val);
        getline(ss, val, ','); z = safe_stod(val);

        for (int i = 0; i < 3; i++) {
            getline(ss, val, ',');
        }
        getline(ss, classification, ',');

        if (classification.find("NORMAL") != string::npos) {
            magnitude = sqrt((x * x) + (y * y) + (z * z));
            if (magnitude > maxAccel) {
                maxAccel = magnitude;
            }
        }
    }
    return maxAccel;
}

void evaluateDrivingData(const string rawFile, double threshold) {
    ifstream file(rawFile);
    string line, val;
    double absolute_accel;
    int total = 0, aggressive = 0;

    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);
        for (int i = 0; i < 4; i++) {
            getline(ss, val, ',');
        }

        getline(ss, val, ',');
        absolute_accel = safe_stod(val);

        if (absolute_accel > threshold) {
            aggressive++;
        }
        total++;
    }

    cout << "Total records processed: " << total << "\n";
    cout << "Aggressive events detected: " << aggressive << "\n";
}

int main() {
    double standard = SafeThreshold("test_motion_data.csv");
    cout << "Normal Threshold limit: " << standard << " m/s^2\n";

    evaluateDrivingData("raw_data2.csv", standard);

    return 0;
}