#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>	
using namespace std;




struct SensorData {
	double time, latitude, longitude, altitude, altitude_wgs84, speed, direction, distance, horizontal_accuracy, vertical_accuracy, satelites;
};

struct TestRecord {
	double accX, accY, accZ;
	string classification;
};

double SafeThreshold(const string testFile) {
	ifstream file(testFile);
	string line;
	double maxSafeAccel = 0.0;

	getline(file, line);
	while (getline(file, line)) {
		stringstream ss(line);
	}
	return maxSafeAccel;
}
void evaluateDrivingData(const string rawFile, double threshold) {
	ifstream file(rawFile);
	if (!file.is_open()) {
		cout << "Failed to open " << rawFile << "\n";
		return;
	}
}
int main() {
	ifstream file("raw_data2.csv");

	vector<SensorData> data;
	string line;
	
	getline(file, line);
	while (getline(file, line)) {
		stringstream ss(line);
		SensorData record;
		string val;
		getline(ss, val, ','); record.time = stod(val);
		getline(ss, val, ','); record.latitude = stod(val);
		getline(ss, val, ','); record.longitude = stod(val);
		getline(ss, val, ','); record.altitude = stod(val);
		getline(ss, val, ','); record.altitude_wgs84 = stod(val);
		getline(ss, val, ','); record.speed = val.empty() ? 0.0 : stod(val);
		getline(ss, val, ','); record.direction = val.empty() ? 0.0 : stod(val);
		getline(ss, val, ','); record.distance = stod(val);
		getline(ss, val, ','); record.horizontal_accuracy = stod(val);
		getline(ss, val, ','); record.vertical_accuracy = stod(val);
		getline(ss, val, ','); record.satelites = stod(val);
		data.push_back(record);
	}
	cout << "Successfully loaded " << data.size() << " sensor records." << endl;
	if (!data.empty()) {
		cout << "Testing Latitude value on first row: " << data[0].latitude << endl;
	}

	double standard = SafeThreshold("test_motion_data.csv");
	evaluateDrivingData("raw_data2.csv", standard);
	return 0;
} 