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


void inspectAccelerationAxes(const string& rawFile) {
    ifstream file(rawFile);

    if (!file.is_open()) {
        cout << "Cannot open file: " << rawFile << "\n";
        return;
    }

    string line;
    getline(file, line);

    cout << "\nCSV columns: " << line << "\n";

    double minA[3] = { 0, 0, 0 };
    double maxA[3] = { 0, 0, 0 };
    int valid = 0, skipped = 0;

    while (getline(file, line)) {
        stringstream ss(line);
        string fields[4];
        bool complete = true;

        for (int i = 0; i < 4; i++) {
            if (!getline(ss, fields[i], ',')) {
                complete = false;
                break;
            }
        }

        if (!complete) {
            skipped++;
            continue;
        }

        double a[3];

        try {
            for (int i = 0; i < 3; i++) {
                a[i] = stod(fields[i + 1]);
                if (!isfinite(a[i])) {
                    complete = false;
                }
            }
        }
        catch (...) {
            complete = false;
        }

        if (!complete) {
            skipped++;
            continue;
        }

        for (int i = 0; i < 3; i++) {
            if (valid == 0) {
                minA[i] = maxA[i] = a[i];
            }
            else {
                if (a[i] < minA[i]) minA[i] = a[i];
                if (a[i] > maxA[i]) maxA[i] = a[i];
            }
        }

        valid++;
    }

    cout << "Valid readings: " << valid << "\n";
    cout << "Skipped readings: " << skipped << "\n";

    const char* names[3] = { "X", "Y", "Z" };

    for (int i = 0; i < 3; i++) {
        cout << names[i] << " axis: "
            << "min = " << minA[i]
            << ", max = " << maxA[i] << "\n";
    }
}


int main() {
    double standard = SafeThreshold("test_motion_data.csv");

    cout << "Normal Threshold limit: "
        << standard << " m/s^2\n";

    evaluateDrivingData("raw_data2.csv", standard);

    // Inspect signed acceleration values
    inspectAccelerationAxes("raw_data2.csv");

    return 0;
}
