import matplotlib.pyplot as plt
import numpy as np

def read_csv(filename):
	with open(filename,'r')as f:
		lines=f.readlines()
	headers=lines[0].strip().split(',')
	data={h:[]for h in headers}
	for line in lines[1:]:
		vals=line.strip().split(',')
		for i,h in enumerate(headers):
			if i<len(vals):
				try:
					data[h].append(float(vals[i]))
				except:
					data[h].append(vals[i])
	return data

data=read_csv('rocket_trajectory.csv')

fig,axes=plt.subplots(2,3,figsize=(15,10))

axes[0,0].plot(data['time'],data['x'],label='X')
axes[0,0].plot(data['time'],data['y'],label='Y')
axes[0,0].plot(data['time'],data['z'],label='Z')
axes[0,0].set_xlabel('time(s)')
axes[0,0].set_ylabel('position(m)')
axes[0,0].legend()
axes[0,0].grid(True)

axes[0,1].plot(data['time'],data['vx'],label='Vx')
axes[0,1].plot(data['time'],data['vy'],label='Vy')
axes[0,1].plot(data['time'],data['vz'],label='Vz')
axes[0,1].set_xlabel('time(s)')
axes[0,1].set_ylabel('velocity(m/s)')
axes[0,1].legend()
axes[0,1].grid(True)

axes[0,2].plot(data['time'],data['v'],'r-')
axes[0,2].set_xlabel('time(s)')
axes[0,2].set_ylabel('speed(m/s)')
axes[0,2].grid(True)

axes[1,0].plot(data['time'],data['rho'],'g-')
axes[1,0].set_xlabel('time(s)')
axes[1,0].set_ylabel('distance(m)')
axes[1,0].grid(True)

axes[1,1].plot(data['time'],data['height'],'b-')
axes[1,1].set_xlabel('time(s)')
axes[1,1].set_ylabel('height(m)')
axes[1,1].grid(True)

axes[1,2].plot(data['x'],data['z'],'purple')
axes[1,2].scatter(data['x'][0],data['z'][0],color='green',s=100)
axes[1,2].scatter(data['x'][-1],data['z'][-1],color='red',s=100,marker='x')
axes[1,2].set_xlabel('X(m)')
axes[1,2].set_ylabel('Z(m)')
axes[1,2].grid(True)
axes[1,2].axis('equal')

plt.tight_layout()
plt.show()

fig2=plt.figure(figsize=(10,8))
ax=fig2.add_subplot(111,projection='3d')
ax.plot(data['x'],data['z'],data['y'],'b-')
ax.scatter(data['x'][0],data['z'][0],data['y'][0],color='green',s=100)
ax.scatter(data['x'][-1],data['z'][-1],data['y'][-1],color='red',s=100,marker='x')
ax.set_xlabel('X(m)')
ax.set_ylabel('Z(m)')
ax.set_zlabel('Y(m)')
plt.tight_layout()
plt.show()
