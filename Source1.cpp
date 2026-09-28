#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

class TelemetryData {
public:
    double accel_x, accel_y, accel_z;
    double gyro_x, gyro_y, gyro_z;
    std::string classification_label;
    double timestamp;

    TelemetryData(double ax, double ay, double az, double gx, double gy, double gz, std::string label, double ts)
        : accel_x(ax), accel_y(ay), accel_z(az), gyro_x(gx), gyro_y(gy), gyro_z(gz), classification_label(label), timestamp(ts) {
    }
};

class DataIngestion {
public:
    std::vector<TelemetryData> parseCSV(const std::string& filename) {
        std::vector<TelemetryData> records;
        std::ifstream file(filename);
        std::string line;

        // Skip the header row
        std::getline(file, line);

        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string val, label;
            double ax, ay, az, gx, gy, gz, ts;

            std::getline(ss, val, ','); ax = std::stod(val);
            std::getline(ss, val, ','); ay = std::stod(val);
            std::getline(ss, val, ','); az = std::stod(val);
            std::getline(ss, val, ','); gx = std::stod(val);
            std::getline(ss, val, ','); gy = std::stod(val);
            std::getline(ss, val, ','); gz = std::stod(val);
            std::getline(ss, label, ',');
            std::getline(ss, val, ','); ts = std::stod(val);

            records.emplace_back(ax, ay, az, gx, gy, gz, label, ts);
        }
        return records;
    }
};

int main() {
    DataIngestion ingestion;
    std::vector<TelemetryData> data = ingestion.parseCSV("test_motion_data.csv");

    std::cout << "Successfully loaded " << data.size() << " telemetry records." << std::endl;

    if (!data.empty()) {
        std::cout << "Testing Gyroscope X value on first row: " << data[0].gyro_x << " (Used for swerve detection)" << std::endl;
    }
    return 0;
}
