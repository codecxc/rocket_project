import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
import numpy as np
import sys

try:
    df=pd.read_csv("trajectories.csv")
except FileNotFoundError:
    print("Файл trajectories.csv не найден — запусти симуляцию сначала")
    sys.exit(1)

atk=df[df["type"]=="atk"]
dfs=df[df["type"]=="def"]
atk_ids=sorted(atk["id"].unique())
def_ids=sorted(dfs["id"].unique())

def atk_color(i,n): return plt.cm.Reds(0.4+0.5*i/max(n-1,1))
def def_color(i,n): return plt.cm.Blues(0.4+0.5*i/max(n-1,1))

fig=plt.figure(figsize=(18,10))
gs=gridspec.GridSpec(2,3,figure=fig)

ax3d=fig.add_subplot(gs[:,0],projection="3d")
for i,aid in enumerate(atk_ids):
    g=atk[atk["id"]==aid]
    ax3d.plot(g["x"],g["y"],g["z"],color=atk_color(i,len(atk_ids)),lw=1.2,alpha=0.8)
    ax3d.scatter(g["x"].iloc[0],g["y"].iloc[0],g["z"].iloc[0],color=atk_color(i,len(atk_ids)),s=40,marker="o",zorder=5)
    ax3d.scatter(g["x"].iloc[-1],g["y"].iloc[-1],g["z"].iloc[-1],color=atk_color(i,len(atk_ids)),s=40,marker="x",zorder=5)
for i,did in enumerate(def_ids):
    g=dfs[dfs["id"]==did]
    ax3d.plot(g["x"],g["y"],g["z"],color=def_color(i,len(def_ids)),lw=1.2,alpha=0.8,linestyle="--")
    ax3d.scatter(g["x"].iloc[0],g["y"].iloc[0],g["z"].iloc[0],color=def_color(i,len(def_ids)),s=40,marker="o",zorder=5)

ax3d.set_xlabel("X, м"); ax3d.set_ylabel("Y, м"); ax3d.set_zlabel("Z, м")
ax3d.set_title("Все траектории 3D")
ax3d.plot([],[],color="tomato",lw=2,label="Атакер")
ax3d.plot([],[],color="steelblue",lw=2,ls="--",label="Дефендер")
ax3d.legend(fontsize=8)

ax_top=fig.add_subplot(gs[0,1])
for i,aid in enumerate(atk_ids):
    g=atk[atk["id"]==aid]
    ax_top.plot(g["x"],g["y"],color=atk_color(i,len(atk_ids)),lw=1,alpha=0.8)
for i,did in enumerate(def_ids):
    g=dfs[dfs["id"]==did]
    ax_top.plot(g["x"],g["y"],color=def_color(i,len(def_ids)),lw=1,alpha=0.8,ls="--")
ax_top.set_xlabel("X, м"); ax_top.set_ylabel("Y, м")
ax_top.set_title("Вид сверху")
ax_top.set_aspect("equal"); ax_top.grid(True,alpha=0.3)

ax_z=fig.add_subplot(gs[1,1])
for i,aid in enumerate(atk_ids):
    g=atk[atk["id"]==aid]
    ax_z.plot(g["t"],g["z"],color=atk_color(i,len(atk_ids)),lw=1,alpha=0.8)
for i,did in enumerate(def_ids):
    g=dfs[dfs["id"]==did]
    ax_z.plot(g["t"],g["z"],color=def_color(i,len(def_ids)),lw=1,alpha=0.8,ls="--")
ax_z.set_xlabel("Время, с"); ax_z.set_ylabel("Высота, м")
ax_z.set_title("Высота от времени")
ax_z.grid(True,alpha=0.3)

ax_dist=fig.add_subplot(gs[:,2])
for i,(aid,did) in enumerate(zip(atk_ids,def_ids)):
    ga=atk[atk["id"]==aid].set_index("t")
    gd=dfs[dfs["id"]==did].set_index("t")
    common=ga.index.intersection(gd.index)
    if len(common)<2: continue
    dist=np.sqrt(
        (ga.loc[common,"x"].values-gd.loc[common,"x"].values)**2+
        (ga.loc[common,"y"].values-gd.loc[common,"y"].values)**2+
        (ga.loc[common,"z"].values-gd.loc[common,"z"].values)**2
    )
    ax_dist.plot(common,dist,lw=1.5,alpha=0.85,label=f"пара {i}")

ax_dist.axhline(y=150,color="red",ls=":",lw=1.5,label="радиус\nпоражения")
ax_dist.set_xlabel("Время, с"); ax_dist.set_ylabel("Дистанция, м")
ax_dist.set_title("Дистанция атакер↔дефендер\nпо парам")
ax_dist.legend(fontsize=7,ncol=2); ax_dist.grid(True,alpha=0.3)

plt.suptitle("Симуляция перехвата ракет",fontsize=14,y=1.01)
plt.tight_layout()

plt.show()
print("График сохранён в trajectories.png")