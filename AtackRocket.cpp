#include <algorithm>
#include "AtackRocket.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include "Vec.cpp"

AtackRocket::AtackRocket(double mass,float start_x, float start_y, float start_z,float target_x, float target_y, float target_z, float cur_angle,double mass_fuel0,double fuel_rashod,double f):mass(mass),cur_angle(cur_angle),is_active(false), is_destroyed(false), time_alive(0.0), mass_fuel0(mass_fuel0), fuel_rashod(fuel_rashod), f(f) {
	xyz=new float[3];
	xyz[0]=start_x;
	xyz[1]=start_y;
	xyz[2]=start_z;
	velocity=new float[3];
	velocity[0]=0.0f;
        velocity[1]=0.0f;
        velocity[2]=0.0f;
	target_xyz=new float[3];
	target_xyz[0]=target_x;
	target_xyz[1]=target_y;
	target_xyz[2]=target_z;
	mass_fuel=mass_fuel0;
	
	f_direction=new float[3];
    	f_direction[0]=0.0f;
    	f_direction[1]=0.0f;
    	f_direction[2]=1.0f; // z
}

AtackRocket::~AtackRocket() {
	delete[] xyz;
	delete[] velocity;
	delete[] target_xyz;
	delete[] f_direction;
}


double AtackRocket::getMass() const {
    	return mass;
}

// double AtackRocket::getMaxHeight() const {
//     	return max_height;
// }

// double AtackRocket::getCurHeight() const {
//     	return cur_height;
// }

// double AtackRocket::getStartSpeed() const {
//     	return start_speed;
// }

float AtackRocket::getCurAngle() const {
    	return cur_angle;
}

float AtackRocket::getX() const {
    	return xyz[0];
}

float AtackRocket::getY() const {
    	return xyz[1];
}

float AtackRocket::getZ() const {
    	return xyz[2];
}

float AtackRocket::getVX() const {
    	return velocity[0];
}

float AtackRocket::getVY() const {
    	return velocity[1];
}

float AtackRocket::getVZ() const {
    	return velocity[2];
}

double AtackRocket::getCurrentSpeed() const {
    	return std::sqrt(velocity[0]*velocity[0]+velocity[1] * velocity[1]+velocity[2]*velocity[2]);
}

float AtackRocket::getTargetX() const {
    	return target_xyz[0];
}

float AtackRocket::getTargetY() const {
    	return target_xyz[1];
}

float AtackRocket::getTargetZ() const {
    	return target_xyz[2];
}


bool AtackRocket::isActive() const {
    	return is_active;
}

bool AtackRocket::isDestroyed() const {
    	return is_destroyed;
}

double AtackRocket::getTimeAlive() const {
    	return time_alive;
}

double AtackRocket::getMassFuel() const {return mass_fuel;}
double AtackRocket::getFuelRashod() const {return fuel_rashod;}
double AtackRocket::getF() const {return f;}
float* AtackRocket::getFDirection() const {return f_direction;}
// float AtackRocket::getS() const {return s;}
// float AtackRocket::getDrag() const {return drag;}
// double AtackRocket::getMaxTimeAlive() const {return max_time_alive;}




void AtackRocket::setMass(double mass) {
    	this->mass=mass;
}

// void AtackRocket::setMaxHeight(double height) {
//     	this->max_height=height;
// }



// void AtackRocket::setStartSpeed(double speed) {
//     	this->start_speed=speed;
// }

void AtackRocket::setCurAngle(float angle) {
    	this->cur_angle=angle;
}

void AtackRocket::setX(float x) {
    	xyz[0]=x;
}

void AtackRocket::setY(float y) {
    	xyz[1]=y;
}

void AtackRocket::setZ(float z) {
    	xyz[2]=z;
}


void AtackRocket::setVX(float vx) {
    	velocity[0] = vx;
}

void AtackRocket::setVY(float vy) {
    	velocity[1] = vy;
}

void AtackRocket::setVZ(float vz) {
    	velocity[2]=vz;
}

void AtackRocket::setTarget(float x, float y, float z) {
    	target_xyz[0]=x;
    	target_xyz[1]=y;
    	target_xyz[2]=z;
}

void AtackRocket::setActive(bool active) {
    	this->is_active=active;
}

void AtackRocket::setDestroyed(bool destroyed) {
    	this->is_destroyed=destroyed;
    	if (destroyed) {
        	is_active = false;
    	}
}

void AtackRocket::setTimeAlive(double time) {
    	this->time_alive=time;
}

void AtackRocket::setMassFuel(double fuel_mass) {
	this->mass_fuel=fuel_mass;
}

void AtackRocket::setFuelRashod(double fuel_rashod) {
	this->fuel_rashod=fuel_rashod;
}

void AtackRocket::setF(double f) {this->f=f;}

// void AtackRocket::setS(float s) {this->s=s;}

// void AtackRocket::setDrag(float drag) {this->drag=drag;}

void AtackRocket::setFDirection(float* f_direction) {this->f_direction=f_direction;}

double AtackRocket::getDistanceToTarget() const {

	return std::sqrt(std::pow(target_xyz[0]-xyz[0],2)+std::pow(target_xyz[1]-xyz[1],2)+std::pow(target_xyz[2]-xyz[2],2));

}

// void AtackRocket::calculateTrajectory() {
//     float dx=target_xyz[0]-xyz[0];
//     float dy=target_xyz[1]-xyz[1];
//     float dz=target_xyz[2]-xyz[2];
//     float dist=std::sqrt(dx*dx+dy*dy+dz*dz);
// 	float horiz=std::sqrt(dx*dx+dy*dy);
//     if (horiz<1e-3f) {f_direction[0]=0.0f;f_direction[1]=0.0f;f_direction[2]=0.0f;return;}
//     f_direction[0]=dx/dist;
//     f_direction[1]=dy/dist;
//     f_direction[2]=0.0f;
// }




// void AtackRocket::launch() {
// 	is_active=true;
// 	is_destroyed=false;
// 	time_alive=0.0;
// 	mass_fuel=mass_fuel0;
// 	velocity[0]=0.0f;velocity[1]=0.0f;velocity[2]=200.0f;
// 	calculateTrajectory();
// }
void AtackRocket::launch() {
    const float G=9.81f;
    is_active=true;is_destroyed=false;time_alive=0.0;
    mass_fuel=mass_fuel0;

    float dx=target_xyz[0]-xyz[0];
    float dy=target_xyz[1]-xyz[1];
    float horiz_dist=std::sqrt(dx*dx+dy*dy);

    float v0=std::sqrt(horiz_dist*G);

    float v_horiz=v0*0.7071;
    float v_vert =v0*0.7071;

    if(horiz_dist>1e-3f) {
        velocity[0]=(dx/horiz_dist)*v_horiz;
        velocity[1]=(dy/horiz_dist)*v_horiz;
    }
    velocity[2]=v_vert;

    cur_angle=45.f;
}
void AtackRocket::calculateTrajectory() {}

void AtackRocket::destroy() {
	is_destroyed=true;
	is_active=false;
	velocity[0]=0.0f;
	velocity[1]=0.0f;
	velocity[2]=0.0f;

}

// const float RHO_0=1.225f;
// const float H=8500.0f;

// void AtackRocket::update(float deltaTime) {
//     if (!is_active or is_destroyed) return;
//     const float G=9.81f;
//     if(mass_fuel>0.0) {
//         calculateTrajectory();
//         float total_mass=mass+mass_fuel;
//         float a=(float)(f/total_mass);
//         velocity[0]+=f_direction[0]*a*deltaTime;
//         velocity[1]+=f_direction[1]*a*deltaTime;
//         // velocity[2]+=f_direction[2]*a*deltaTime;
//         mass_fuel-=fuel_rashod*deltaTime;
//         if(mass_fuel<0.0) mass_fuel=0.0;
//     }
//     velocity[2]-=G*deltaTime;

//     xyz[0]+=velocity[0]*deltaTime;
//     xyz[1]+=velocity[1]*deltaTime;
//     xyz[2]+=velocity[2]*deltaTime;


//     float horiz=std::sqrt(velocity[0]*velocity[0]+velocity[1]*velocity[1]);
//     cur_angle=std::atan2(velocity[2], horiz)*180.f/M_PI;
//     time_alive+=deltaTime;

//     if(xyz[2]<=0.f) {xyz[2]=0.f;destroy();return;}
//     if(getDistanceToTarget()<30.f) {destroy();return;}
// }

void AtackRocket::update(float deltaTime) {
    if(!is_active or is_destroyed) return;
    const float G=9.81f;
    velocity[2]-=G*deltaTime;

    xyz[0]+=velocity[0]*deltaTime;
    xyz[1]+=velocity[1]*deltaTime;
    xyz[2]+=velocity[2]*deltaTime;

    float horiz=std::sqrt(velocity[0]*velocity[0]+velocity[1]*velocity[1]);
    cur_angle=std::atan2(velocity[2],horiz)*180.f/M_PI;
    time_alive+=deltaTime;
    if(xyz[2]<=0.f) {
        xyz[2]=0.f;
        destroy();
    }
}