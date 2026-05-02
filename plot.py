import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

df = pd.read_csv("trajectories.csv")

atk = df[df["type"] == "atk"]
dfs = df[df["type"] == "def"]

fig = plt.figure(figsize=(14, 6))

ax1 = fig.add_subplot(1, 2, 1, projection='3d')

for aid in atk["id"].unique():
    g = atk[atk["id"] == aid]
    ax1.plot(g["x"], g["y"], g["z"], 'r-', alpha=0.7, lw=1.5)
    ax1.scatter(g["x"].iloc[0], g["y"].iloc[0], g["z"].iloc[0], c='r', s=40, marker='o')
    ax1.scatter(g["x"].iloc[-1], g["y"].iloc[-1], g["z"].iloc[-1], c='r', s=40, marker='x')

for did in dfs["id"].unique():
    g = dfs[dfs["id"] == did]
    ax1.plot(g["x"], g["y"], g["z"], 'b--', alpha=0.7, lw=1.5)
    ax1.scatter(g["x"].iloc[0], g["y"].iloc[0], g["z"].iloc[0], c='b', s=40, marker='o')

ax1.set_xlabel("X, м")
ax1.set_ylabel("Y, м")
ax1.set_zlabel("Z, м")
ax1.set_title("траектории")
ax1.legend(['Атакер', 'Дефендер'], loc='upper right')

ax2 = fig.add_subplot(1, 2, 2)

for aid, did in zip(sorted(atk["id"].unique()), sorted(dfs["id"].unique())):
    ga = atk[atk["id"] == aid].set_index("t")
    gd = dfs[dfs["id"] == did].set_index("t")
    
    common = ga.index.intersection(gd.index)
    
    if len(common) > 1:
        dist = np.sqrt(
            (ga.loc[common, "x"].values - gd.loc[common, "x"].values)**2 +
            (ga.loc[common, "y"].values - gd.loc[common, "y"].values)**2 +
            (ga.loc[common, "z"].values - gd.loc[common, "z"].values)**2
        )
        ax2.plot(common, dist, lw=1.5, alpha=0.8)

ax2.axhline(y=150, color='red', ls='--', lw=1.5)
ax2.set_xlabel("Время, с")
ax2.set_ylabel("Дистанция, м")
ax2.set_title("Расстояние между ракетами")
ax2.grid(True)

plt.tight_layout()
plt.show()