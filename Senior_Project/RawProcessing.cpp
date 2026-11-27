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


enum class MotionType {
    Neutral,
    Accelerating,
    Braking
};

struct MotionSample {
    double time;
    double longitudinalAcceleration;
};

struct MotionEvent {
    MotionType type;
    double startTime;
    double endTime;
};

// Determine the movement type using signed acceleration.
MotionType classifyMotion(double acceleration, double threshold) {
    if (acceleration > threshold)
        return MotionType::Accelerating;

    if (acceleration < -threshold)
        return MotionType::Braking;

    return MotionType::Neutral;
}

// Identify periods of acceleration or braking.
vector<MotionEvent> detectMotionEvents(
    const vector<MotionSample>& samples,
    double threshold,
    double minDuration)
{
    vector<MotionEvent> events;

    MotionType current = MotionType::Neutral;
    double start = 0.0;
    double last = 0.0;

    auto finishEvent = [&]() {
        if (current != MotionType::Neutral &&
            last - start >= minDuration) {
            events.push_back({ current, start, last });
        }
        };

    for (const MotionSample& sample : samples) {
        MotionType next = classifyMotion(
            sample.longitudinalAcceleration, threshold
        );

        if (next != current) {
            finishEvent();
            current = next;
            start = sample.time;
        }

        last = sample.time;
    }

    finishEvent();
    return events;
}

// Count acceleration events followed by braking.
int countAccelBrakePairs(
    const vector<MotionEvent>& events,
    double maxGap)
{
    int pairs = 0;

    for (size_t i = 1; i < events.size(); ++i) {
        if (events[i - 1].type == MotionType::Accelerating &&
            events[i].type == MotionType::Braking &&
            events[i].startTime >= events[i - 1].endTime &&
            events[i].startTime - events[i - 1].endTime <= maxGap) {
            pairs++;
        }
    }

    return pairs;
}


bool runDetectorTests() {
    const double threshold = 1.5;
    const double minDuration = 0.3;

    int passed = 0;
    int total = 0;

    auto check = [&](bool condition, const char* label) {
        total++;

        if (condition)
            passed++;

        cout << (condition ? "PASS: " : "FAIL: ")
            << label << "\n";
        };

    // Test 1: Almost no acceleration.
    vector<MotionSample> stationary = {
        {0.0, 0.0},
        {0.5, 0.2},
        {1.0, -0.1}
    };

    check(
        detectMotionEvents(stationary, threshold, minDuration).empty(),
        "No movement = no events"
    );

    // Test 2: A very short acceleration spike.
    vector<MotionSample> spike = {
        {0.0, 0.0},
        {0.1, 2.0},
        {0.2, 0.0}
    };

    check(
        detectMotionEvents(spike, threshold, minDuration).empty(),
        "Short acceleration spike ignored"
    );

    // Tests 3 and 4: Two acceleration-braking cycles.
    vector<MotionSample> sequence = {
        {0.0, 0.0},
        {0.2, 2.0},
        {0.6, 2.0},
        {0.8, 0.0},

        {1.0, -2.0},
        {1.4, -2.0},
        {1.6, 0.0},

        {2.0, 2.3},
        {2.4, 2.3},
        {2.6, 0.0},

        {3.0, -2.3},
        {3.4, -2.3},
        {3.6, 0.0}
    };

    auto events = detectMotionEvents(
        sequence, threshold, minDuration
    );

    check(
        events.size() == 4,
        "Four events detected"
    );

    check(
        countAccelBrakePairs(events, 10.0) == 2,
        "Two acceleration-braking pairs"
    );

    // Test 5: Braking without prior acceleration.
    vector<MotionSample> braking = {
        {0.0, -2.0},
        {0.4, -2.0},
        {0.6, 0.0}
    };

    auto brakingEvents = detectMotionEvents(
        braking, threshold, minDuration
    );

    check(
        brakingEvents.size() == 1 &&
        countAccelBrakePairs(brakingEvents, 10.0) == 0,
        "Braking alone is not a pair"
    );

    cout << passed << "/" << total
        << " tests passed\n";

    return passed == total;
}

struct SequenceEvent {
    int type;  // +1 = accelerating; -1 = braking
    double start;
    double end;
};

void analyzeAccelBrakeSequences(const string& fileName) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cout << "Cannot open " << fileName << "\n";
        return;
    }

    // The export is: Time (s), X (m/s^2), Y (m/s^2), Z (m/s^2).
    // For a flat phone with its top facing forward, Y is the
    // candidate longitudinal axis. Its sign still needs calibration.
    struct Reading { double time, y; };
    vector<Reading> readings;
    string line;
    getline(file, line);  // header
    int skipped = 0;

    while (getline(file, line)) {
        stringstream ss(line);
        double t, x, y, z;
        char c1, c2, c3;
        if (!(ss >> t >> c1 >> x >> c2 >> y >> c3 >> z) ||
            c1 != ',' || c2 != ',' || c3 != ',' ||
            !isfinite(t) || !isfinite(y) ||
            (!readings.empty() && t <= readings.back().time)) {
            ++skipped;
            continue;
        }
        readings.push_back({ t, y });
    }

    if (readings.empty()) {
        cout << "No usable acceleration records.\n";
        return;
    }

    // Trailing 0.25-second moving average removes brief sensor spikes.
    vector<Reading> smoothed;
    size_t left = 0;
    double sum = 0;
    for (size_t right = 0; right < readings.size(); ++right) {
        sum += readings[right].y;
        while (readings[right].time - readings[left].time > 0.25) {
            sum -= readings[left].y;
            ++left;
        }
        smoothed.push_back({ readings[right].time,
                            sum / static_cast<double>(right - left + 1) });
    }

    const double threshold = 1.5;  // m/s^2, provisional
    const double minDuration = 0.4; // seconds, provisional
    const double maxGap = 8.0;      // seconds between events
    const double maxSampleGap = 0.2; // detect breaks in recording

    vector<SequenceEvent> events;
    int state = 0;   // +1 acceleration, -1 braking, 0 neutral
    double start = 0, last = 0;

    auto closeEvent = [&]() {
        if (state != 0 && last - start >= minDuration)
            events.push_back({ state, start, last });
        };

    for (size_t i = 0; i < smoothed.size(); ++i) {
        const auto& sample = smoothed[i];
        int next = sample.y > threshold ? 1 :
            sample.y < -threshold ? -1 : 0;

        bool gap = i > 0 &&
            sample.time - smoothed[i - 1].time > maxSampleGap;
        if (gap) {
            closeEvent();
            state = 0;
        }

        if (next != state) {
            closeEvent();
            state = next;
            start = sample.time;
        }
        last = sample.time;
    }
    closeEvent();

    int accelerating = 0, braking = 0, accelBrakePairs = 0;
    for (const auto& e : events) {
        if (e.type == 1) ++accelerating;
        else ++braking;
    }

    // Count acceleration followed by braking.
    for (size_t i = 1; i < events.size(); ++i) {
        double gap = events[i].start - events[i - 1].end;

        if (events[i - 1].type == 1 &&
            events[i].type == -1 &&
            gap >= 0 && gap <= maxGap) {
            ++accelBrakePairs;
        }
    }

    // Find repeated alternating acceleration/braking sequences.
    int repeatedSequences = 0;
    size_t sequenceStart = 0;

    for (size_t i = 1; i <= events.size(); ++i) {
        bool continues = i < events.size() &&
            events[i].type != events[i - 1].type &&
            events[i].start >= events[i - 1].end &&
            events[i].start - events[i - 1].end <= maxGap;

        if (!continues) {
            if (i - sequenceStart >= 4) {
                ++repeatedSequences;

                cout << "Repeated sequence: "
                    << events[sequenceStart].start
                    << " - "
                    << events[i - 1].end
                    << " s ("
                    << i - sequenceStart
                    << " events)\n";
            }

            sequenceStart = i;
        }
    }

    cout << "\n--- Acceleration / Braking analysis ---\n";
    cout << "Usable samples: " << readings.size() << "\n";
    cout << "Skipped samples: " << skipped << "\n";
    cout << "Acceleration events: " << accelerating << "\n";
    cout << "Braking events: " << braking << "\n";
    cout << "Acceleration -> braking pairs: "
        << accelBrakePairs << "\n";
    cout << "Repeated alternating sequences: "
        << repeatedSequences << "\n";
}


    int main() {
    double standard = SafeThreshold("test_motion_data.csv");

    cout << "Normal Threshold limit: "
         << standard << " m/s^2\n";

    evaluateDrivingData("raw_data2.csv", standard);
    inspectAccelerationAxes("raw_data2.csv");

    // Analyse the new phyphox acceleration recording
    analyzeAccelBrakeSequences("Linear Accelerometer.csv");

    cout << "\n--- Event Detection Tests ---\n";

    if (!runDetectorTests()) {
        return 1;
    }

    return 0;
}

