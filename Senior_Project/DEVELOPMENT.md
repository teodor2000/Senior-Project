# Development Log

## Baseline

Current implementation:
- CSV input processing
- Kaggle normal-driving reference threshold
- phyphox acceleration comparison
- Separate Python visualization

Current limitations:
- No distinction between acceleration and braking
- No repeated-event detection
- No traffic-context detection

Next task:
Implement signed acceleration and braking event detection.


## Signed Acceleration Analysis

### Changes made
- Added the inspectAccelerationAxes() function to RawProcessing.cpp.
- Added a file-opening check for the phyphox CSV.
- Added validation for acceleration values.
- Added minimum and maximum values for the X, Y and Z axes.
- Changed the output from "Aggressive events detected" to "Readings above threshold".

### Testing
- Built the project using Visual Studio in Debug x64.
- Ran the program using raw_data2.csv.
- Confirmed that the original threshold calculation still works.

### Results
- Normal threshold: 6.3141 m/s^2
- Total records: 248770
- Readings above threshold: 91

Axis inspection:
Valid readings: 248770
Skipped readings: 0
X axis: min = -10.8787, max = 15.5735
Y axis: min = -15.0181, max = 7.53121
Z axis: min = -9.77832, max = 25.7076

### Next steps
- Determine which acceleration axis represents forward vehicle movement.
- Identify acceleration and braking events separately.
- Test the detection algorithm using known acceleration values.


## Acceleration and Braking Event Detection

### Implementation
Added MotionSample and MotionEvent structures.

Added classifyMotion() to classify signed acceleration
as accelerating, braking or neutral.

Added detectMotionEvents() to group consecutive
measurements into individual events.

Added countAccelBrakePairs() to recognize acceleration
followed by braking within a specified time window.

### Testing
Five synthetic test cases were created.

Tested:
1. Near-zero acceleration
2. Short acceleration spikes
3. Individual event detection
4. Acceleration-braking pair detection
5. Braking without preceding acceleration

Result:
PASS: No movement = no events
PASS: Short acceleration spike ignored
PASS: Four events detected
PASS: Two acceleration-braking pairs
PASS: Braking alone is not a pair
5/5 tests passed

### Current limitations
The algorithm has only been tested with synthetic data.
The real phone acceleration axis is not yet calibrated.
The algorithm does not yet account for stop-and-go traffic.

### Next steps
Calibrate the sensor orientation.
Connect real phyphox acceleration data.
Add timestamp validation.
Detect repeated acceleration-braking cycles.
Use GPS speed to identify stop-and-go traffic.

## November 26, 2026 — Acceleration and Braking Sequence Detection

### 1. Existing functionality verified

Built and ran the original C++ program successfully in Visual Studio using Debug x64.

Results:
- Normal acceleration threshold calculated from Kaggle data: 6.3141 m/s²
- Total phyphox records processed: 248,770
- Readings above the threshold: 91
- Program exited successfully with code 0

The existing threshold analysis remained functional after adding new features.

### 2. Signed acceleration analysis

Added `inspectAccelerationAxes()` to examine the X, Y, and Z acceleration components separately.

The function:
- Reads acceleration values from a CSV file.
- Checks whether the file can be opened.
- Validates numerical measurements.
- Calculates minimum and maximum values for each axis.
- Counts valid and skipped readings.

Testing with `raw_data2.csv`:
- Valid readings: 248,770
- Skipped readings: 0

### 3. Basic motion event detection

Implemented:
- `MotionSample` and `MotionEvent` data structures.
- `classifyMotion()` to distinguish acceleration, braking, and neutral readings.
- `detectMotionEvents()` to group sustained measurements into events.
- `countAccelBrakePairs()` to identify acceleration followed by braking.

Added five synthetic tests.

**Test results: 5/5 passed.**

The tests cover near-zero acceleration, short spikes, individual event detection, acceleration–braking pairs, and braking without preceding acceleration.

### 4. New phyphox recording format

Created a combined phyphox experiment using:
- Linear acceleration
- Location (GPS)
- Requested acceleration sampling rate: 50 Hz

A short hand-held recording was exported to verify the data format.

The new acceleration CSV contains timestamps and X, Y, and Z acceleration values.

### 5. Acceleration–braking sequence analysis

Added `analyzeAccelBrakeSequences()` to process the new phyphox acceleration CSV.

The algorithm is designed to:
- Read timestamped acceleration samples.
- Smooth the acceleration signal.
- Identify sustained acceleration and braking events.
- Count acceleration followed by braking.
- Identify repeated alternating acceleration–braking sequences.

Initial test with the hand-held recording:
- Usable samples: 736
- Skipped samples: 0
- Acceleration events: 0
- Braking events: 0
- Acceleration–braking pairs: 0
- Repeated alternating sequences: 0

The program built and ran successfully.

This recording was not collected while driving, so it does not validate the accuracy of driving-event detection.

### 6. Current limitations

- The new sequence detector has not yet been verified using the synthetic driving CSV.
- The phone's longitudinal acceleration axis and sign still need calibration.
- The current sequence detector does not use GPS speed.
- The thresholds and filtering parameters are provisional.
- Repeated acceleration and braking cannot independently establish tailgating.
- The original threshold counter still uses the misleading output label "Aggressive events detected", which should be changed to "Readings above threshold".

### 7. Next development steps

1. Test the sequence detector with known synthetic acceleration and braking patterns.
2. Record actual driving data using a securely mounted phone.
3. Calibrate the forward acceleration axis.
4. Implement CSV loading and synchronization for acceleration and GPS data.
5. Distinguish stop-and-go traffic from higher-speed acceleration–braking patterns.
6. Improve automated testing and generate driving-behaviour reports.

