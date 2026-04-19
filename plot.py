import pandas as pd
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import numpy as np

df = pd.read_csv('trajectories.csv')

fig = plt.figure(figsize=(14, 10))
ax = fig.add_subplot(111, projection='3d')

attacker_data = df[df['type'] == 'attacker']
ax.plot(attacker_data['x'], attacker_data['y'], attacker_data['z'], 
        'r-', linewidth=3, label='Attacker Rocket', marker='o', markersize=2, markevery=50)

defender_ids = df[df['type'] == 'defender']['id'].unique()
best_defender = None
min_distance = float('inf')

for defender_id in defender_ids:
    defender_data = df[(df['type'] == 'defender') & (df['id'] == defender_id)]
    for idx, row in defender_data.iterrows():
        attacker_pos = attacker_data[attacker_data['time'] == row['time']]
        if len(attacker_pos) > 0:
            dist = np.sqrt((row['x'] - attacker_pos.iloc[0]['x'])**2 + 
                          (row['y'] - attacker_pos.iloc[0]['y'])**2 + 
                          (row['z'] - attacker_pos.iloc[0]['z'])**2)
            if dist < min_distance:
                min_distance = dist
                best_defender = defender_id

colors = plt.cm.Set1(np.linspace(0, 1, len(defender_ids)))

for i, defender_id in enumerate(defender_ids):
    defender_data = df[(df['type'] == 'defender') & (df['id'] == defender_id)]
    if defender_id == best_defender:
        ax.plot(defender_data['x'], defender_data['y'], defender_data['z'], 
                'b-', linewidth=3, label=f'Best Defender {int(defender_id)}', 
                color='blue', alpha=0.9, marker='s', markersize=2, markevery=50)
    else:
        ax.plot(defender_data['x'], defender_data['y'], defender_data['z'], 
                '-', linewidth=1, label=f'Defender {int(defender_id)}', 
                color=colors[i % len(colors)], alpha=0.5)

attacker_start = attacker_data.iloc[0]
attacker_end = attacker_data.iloc[-1]
ax.scatter([attacker_start['x']], [attacker_start['y']], [attacker_start['z']], 
           color='red', s=150, marker='o', edgecolor='black', linewidth=2)
ax.scatter([attacker_end['x']], [attacker_end['y']], [attacker_end['z']], 
           color='darkred', s=150, marker='x', linewidth=3)

best_defender_data = df[(df['type'] == 'defender') & (df['id'] == best_defender)]
best_start = best_defender_data.iloc[0]
best_end = best_defender_data.iloc[-1]
ax.scatter([best_start['x']], [best_start['y']], [best_start['z']], 
           color='blue', s=150, marker='s', edgecolor='black', linewidth=2)
ax.scatter([best_end['x']], [best_end['y']], [best_end['z']], 
           color='darkblue', s=150, marker='*', linewidth=2)

ax.set_xlabel('X', fontsize=12, fontweight='bold')
ax.set_ylabel('Y', fontsize=12, fontweight='bold')
ax.set_zlabel('Z', fontsize=12, fontweight='bold')
ax.set_title('Rocket Trajectories', fontsize=14, fontweight='bold')

ax.legend(loc='upper left', bbox_to_anchor=(1.05, 1), fontsize=10, framealpha=0.9)

fig, axes = plt.subplots(2, 2, figsize=(14, 10))

ax_xy = axes[0, 0]
ax_xy.plot(attacker_data['x'], attacker_data['y'], 'r-', linewidth=3, label='Attacker')
for i, defender_id in enumerate(defender_ids):
    defender_data = df[(df['type'] == 'defender') & (df['id'] == defender_id)]
    if defender_id == best_defender:
        ax_xy.plot(defender_data['x'], defender_data['y'], 'b-', linewidth=3, 
                   label=f'Best Defender', alpha=0.9)
    else:
        ax_xy.plot(defender_data['x'], defender_data['y'], '-', linewidth=1, 
                   alpha=0.4)
ax_xy.scatter([attacker_start['x']], [attacker_start['y']], color='red', s=100, marker='o', edgecolor='black', zorder=5)
ax_xy.scatter([attacker_end['x']], [attacker_end['y']], color='darkred', s=100, marker='x', linewidth=3, zorder=5)
ax_xy.scatter([best_start['x']], [best_start['y']], color='blue', s=100, marker='s', edgecolor='black', zorder=5)
ax_xy.set_xlabel('X')
ax_xy.set_ylabel('Y')
ax_xy.set_title('XY Projection')
ax_xy.grid(True, alpha=0.3)
ax_xy.legend(fontsize=10)

ax_xz = axes[0, 1]
ax_xz.plot(attacker_data['x'], attacker_data['z'], 'r-', linewidth=3, label='Attacker')
for i, defender_id in enumerate(defender_ids):
    defender_data = df[(df['type'] == 'defender') & (df['id'] == defender_id)]
    if defender_id == best_defender:
        ax_xz.plot(defender_data['x'], defender_data['z'], 'b-', linewidth=3, alpha=0.9)
    else:
        ax_xz.plot(defender_data['x'], defender_data['z'], '-', linewidth=1, alpha=0.4)
ax_xz.scatter([attacker_start['x']], [attacker_start['z']], color='red', s=100, marker='o', edgecolor='black', zorder=5)
ax_xz.scatter([attacker_end['x']], [attacker_end['z']], color='darkred', s=100, marker='x', linewidth=3, zorder=5)
ax_xz.scatter([best_start['x']], [best_start['z']], color='blue', s=100, marker='s', edgecolor='black', zorder=5)
ax_xz.set_xlabel('X')
ax_xz.set_ylabel('Z')
ax_xz.set_title('XZ Projection')
ax_xz.grid(True, alpha=0.3)

ax_yz = axes[1, 0]
ax_yz.plot(attacker_data['y'], attacker_data['z'], 'r-', linewidth=3, label='Attacker')
for i, defender_id in enumerate(defender_ids):
    defender_data = df[(df['type'] == 'defender') & (df['id'] == defender_id)]
    if defender_id == best_defender:
        ax_yz.plot(defender_data['y'], defender_data['z'], 'b-', linewidth=3, alpha=0.9)
    else:
        ax_yz.plot(defender_data['y'], defender_data['z'], '-', linewidth=1, alpha=0.4)
ax_yz.scatter([attacker_start['y']], [attacker_start['z']], color='red', s=100, marker='o', edgecolor='black', zorder=5)
ax_yz.scatter([attacker_end['y']], [attacker_end['z']], color='darkred', s=100, marker='x', linewidth=3, zorder=5)
ax_yz.scatter([best_start['y']], [best_start['z']], color='blue', s=100, marker='s', edgecolor='black', zorder=5)
ax_yz.set_xlabel('Y')
ax_yz.set_ylabel('Z')
ax_yz.set_title('YZ Projection')
ax_yz.grid(True, alpha=0.3)

ax_time = axes[1, 1]
attacker_distance = np.sqrt(attacker_data['x']**2 + attacker_data['y']**2 + attacker_data['z']**2)
ax_time.plot(attacker_data['time'], attacker_distance, 'r-', linewidth=3, label='Attacker')
for i, defender_id in enumerate(defender_ids):
    defender_data = df[(df['type'] == 'defender') & (df['id'] == defender_id)]
    defender_distance = np.sqrt(defender_data['x']**2 + defender_data['y']**2 + defender_data['z']**2)
    if defender_id == best_defender:
        ax_time.plot(defender_data['time'], defender_distance, 'b-', linewidth=3, label=f'Best Defender', alpha=0.9)
    else:
        ax_time.plot(defender_data['time'], defender_distance, '-', linewidth=1, alpha=0.4)
ax_time.set_xlabel('Time')
ax_time.set_ylabel('Distance from Origin')
ax_time.set_title('Distance over Time')
ax_time.grid(True, alpha=0.3)
ax_time.legend(fontsize=10)

plt.tight_layout()
plt.show()
