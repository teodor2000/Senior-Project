#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;


ifstream file("raw_data.csv");

struct SensorData {
	double time, latitude, longitude, altitude, altitude_wgs84, speed, direction, distance, horizontal_accuracy, vertical_accuracy, satelites;
};

int main() {
	vector<SensorData> data;
	string line;
	// Skip the header row
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
		getline(ss, val, ','); record.speed = stod(val);
		getline(ss, val, ','); record.direction = stod(val);
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
	return 0;
}