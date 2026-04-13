#include "AtackRocket.h"
#include <iostream>
#include <fstream>
#include <iomanip>

int main() {
	double mass=100.0;
	double max_height=1000.0;
	double start_speed=100.0;
	float start_x=0.0f;
	float start_y=0.0f;
	float start_z=0.0f;
	float target_x=500.0f;
	float target_y=0.0f;
	float target_z=300.0f;

	AtackRocket rocket(mass,max_height,start_speed,start_x,start_y,start_z,target_x,target_y,target_z);

	rocket.launch();

	std::ofstream outfile("rocket_trajectory.csv");
	outfile<<"time,x,y,z,vx,vy,vz,v,rho,height\n";

	double deltaTime=0.01;
	double currentTime=0.0;

	while(rocket.isActive()&&!rocket.isDestroyed()) {
		outfile<<std::fixed<<std::setprecision(6)
			<<currentTime<<","
			<<rocket.getX()<<","
			<<rocket.getY()<<","
			<<rocket.getZ()<<","
			<<rocket.getVelocityX()<<","
			<<rocket.getVelocityY()<<","
			<<rocket.getVelocityZ()<<","
			<<rocket.getCurrentSpeed()<<","
			<<rocket.getDistanceToTarget()<<","
			<<rocket.getCurHeight()<<"\n";

		rocket.update(deltaTime);
		currentTime+=deltaTime;

		if(currentTime>60.0) {
			rocket.destroy();
			break;
		}
	}

	outfile.close();
	return 0;
}
