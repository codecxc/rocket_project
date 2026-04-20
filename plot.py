import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from mpl_toolkits.mplot3d import Axes3D

df = pd.read_csv('data.txt', sep=',', skipinitialspace=True)

attacker = df[df['type'] == 'attacker']
defender = df[df['type'] == 'defender']

fig = plt.figure(figsize=(14, 6))

ax1 = fig.add_subplot(121, projection='3d')
ax1.plot(attacker['x'], attacker['y'], attacker['z'], 'b-', label='Attacker', linewidth=2)
ax1.plot(defender['x'], defender['y'], defender['z'], 'r-', label='Defender', linewidth=2)
ax1.scatter(attacker['x'].iloc[0], attacker['y'].iloc[0], attacker['z'].iloc[0], color='blue', s=50, marker='o')
ax1.scatter(defender['x'].iloc[0], defender['y'].iloc[0], defender['z'].iloc[0], color='red', s=50, marker='o')
ax1.scatter(attacker['x'].iloc[-1], attacker['y'].iloc[-1], attacker['z'].iloc[-1], color='blue', s=50, marker='s')
ax1.scatter(defender['x'].iloc[-1], defender['y'].iloc[-1], defender['z'].iloc[-1], color='red', s=50, marker='s')
ax1.set_xlabel('X')
ax1.set_ylabel('Y')
ax1.set_zlabel('Z')
ax1.set_title('3D Trajectories')
ax1.legend()

ax2 = fig.add_subplot(122)
times = np.unique(df['time'])
distances = []

for t in times:
    att = attacker[attacker['time'] == t]
    deff = defender[defender['time'] == t]
    if len(att) > 0 and len(deff) > 0:
        dist = np.sqrt((att['x'].values[0] - deff['x'].values[0])**2 + 
                       (att['y'].values[0] - deff['y'].values[0])**2 +
                       (att['z'].values[0] - deff['z'].values[0])**2)
        distances.append(dist)
    else:
        distances.append(np.nan)

ax2.plot(times, distances, 'g-', linewidth=2, marker='o', markersize=3)
ax2.set_xlabel('Time')
ax2.set_ylabel('Distance')
ax2.set_title('Distance vs Time')
ax2.grid(True, alpha=0.3)

plt.tight_layout()
plt.show()