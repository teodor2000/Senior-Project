import pandas as pd
import matplotlib.pyplot as plt

raw_path = 'raw_data2.csv'
behavior_path = 'test_motion_data.csv'

raw_df = pd.read_csv(raw_path)
behavior_df = pd.read_csv(behavior_path)

time = raw_df['Time (s)']
acc_x = raw_df['Linear Acceleration x (m/s^2)']
acc_y = raw_df['Linear Acceleration y (m/s^2)']
acc_z = raw_df['Linear Acceleration z (m/s^2)']
abs_acc = raw_df['Absolute acceleration (m/s^2)']

fig1, (ax1, ax2) = plt.subplots(2, 1, figsize=(14, 8))

ax1.plot(time, acc_x, label='X-Axis', color='red', alpha=0.7)
ax1.plot(time, acc_y, label='Y-Axis', color='green', alpha=0.7)
ax1.plot(time, acc_z, label='Z-Axis', color='blue', alpha=0.7)
ax1.set_title('Raw Data: Linear Acceleration Vectors')
ax1.set_xlabel('Time (s)')
ax1.set_ylabel('Acceleration (m/s^2)')
ax1.legend()
ax1.grid(True)

ax2.plot(time, abs_acc, label='Absolute Magnitude', color='black')
ax2.set_title('Raw Data: Absolute Acceleration')
ax2.set_xlabel('Time (s)')
ax2.set_ylabel('Acceleration (m/s^2)')
ax2.legend()
ax2.grid(True)
fig1.tight_layout()


behavior_clean = behavior_df.iloc[:, [7, 0]].dropna()
behavior_time_norm = behavior_clean.iloc[:, 0] - behavior_clean.iloc[:, 0].iloc[0]
behavior_val = behavior_clean.iloc[:, 1]

fig2, ax3 = plt.subplots(1, 1, figsize=(14, 4))

ax3.plot(behavior_time_norm, behavior_val, color='red')
ax3.set_title('Behavior Data: X-Axis Acceleration')
ax3.set_xlabel('Normalized Time (units)')
ax3.set_ylabel('Acceleration Magnitude')
ax3.grid(True)
fig2.tight_layout()

plt.show()