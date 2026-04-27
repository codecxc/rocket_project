import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

df = pd.read_csv('attackrockets.txt', sep=',', skipinitialspace=True)

attackers = df[df['type'] == 'attacker']
defenders = df[df['type'] == 'defender']

fig = plt.figure(figsize=(14, 6))

ax1 = fig.add_subplot(121, projection='3d')

for atk_id, grp in attackers.groupby('id'):
    ax1.plot(grp['x'], grp['y'], grp['z'], 'b-', linewidth=0.8, alpha=0.7)
    ax1.scatter(grp['x'].iloc[0], grp['y'].iloc[0], grp['z'].iloc[0], 
                color='blue', s=30, marker='o')
    ax1.scatter(grp['x'].iloc[-1], grp['y'].iloc[-1], grp['z'].iloc[-1], 
                color='blue', s=30, marker='s')

for def_id, grp in defenders.groupby('id'):
    ax1.plot(grp['x'], grp['y'], grp['z'], 'r-', linewidth=0.8, alpha=0.7)
    ax1.scatter(grp['x'].iloc[0], grp['y'].iloc[0], grp['z'].iloc[0], 
                color='red', s=30, marker='o')
    ax1.scatter(grp['x'].iloc[-1], grp['y'].iloc[-1], grp['z'].iloc[-1], 
                color='red', s=30, marker='s')

ax1.set_xlabel('X')
ax1.set_ylabel('Y')
ax1.set_zlabel('Z')
ax1.set_title('3D Trajectories (all rockets)')

ax2 = fig.add_subplot(122)

if 0 in attackers['id'].values and 0 in defenders['id'].values:
    atk0 = attackers[attackers['id'] == 0].set_index('time')
    def0 = defenders[defenders['id'] == 0].set_index('time')
    common_times = atk0.index.intersection(def0.index)
    dist = np.sqrt((atk0.loc[common_times, 'x'].values - def0.loc[common_times, 'x'].values)**2 +
                   (atk0.loc[common_times, 'y'].values - def0.loc[common_times, 'y'].values)**2 +
                   (atk0.loc[common_times, 'z'].values - def0.loc[common_times, 'z'].values)**2)
    ax2.plot(common_times, dist, 'g-', linewidth=2, marker='o', markersize=3)
    ax2.set_title('Distance: Attacker 0 vs Defender 0')
else:
    times = sorted(df['time'].unique())
    active_attackers = [len(attackers[(attackers['time'] == t) & (attackers['x'].notna())]) for t in times]
    ax2.plot(times, active_attackers, 'b-', label='Active attackers')
    active_defenders = [len(defenders[(defenders['time'] == t) & (defenders['x'].notna())]) for t in times]
    ax2.plot(times, active_defenders, 'r-', label='Active defenders')
    ax2.legend()
    ax2.set_title('Active rocket counts')

ax2.set_xlabel('Time')
ax2.set_ylabel('Distance / Count')
ax2.grid(True, alpha=0.3)

plt.tight_layout()
plt.show()