#include "AtackRocket.h"
#include <cmath>
#include "Vec.cpp"

AtackRocket::AtackRocket(double mass,double max_height,double start_speed,float start_x, float start_y, float start_z,float target_x, float target_y, float target_z, float cur_angle, double max_time_alive,double mass_fuel0,double fuel_rashod,double f, float s, float drag):mass(mass),max_height(max_height), cur_height(start_y),start_speed(start_speed), cur_angle(cur_angle),is_active(false), is_destroyed(false), time_alive(0.0), max_time_alive(max_time_alive), mass_fuel0(mass_fuel0), fuel_rashod(fuel_rashod), f(f), s(s), drag(drag) {
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
    	f_direction[1]=1.0f;
    	f_direction[2]=0.0f;
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

double AtackRocket::getMaxHeight() const {
    	return max_height;
}

double AtackRocket::getCurHeight() const {
    	return cur_height;
}

double AtackRocket::getStartSpeed() const {
    	return start_speed;
}

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
float AtackRocket::getS() const {return s;}
float AtackRocket::getDrag() const {return drag;}
double AtackRocket::getMaxTimeAlive() const {return max_time_alive;}




void AtackRocket::setMass(double mass) {
    	this->mass=mass;
}

void AtackRocket::setMaxHeight(double height) {
    	this->max_height=height;
}

void AtackRocket::setCurHeight(double height) {
    	this->cur_height=height;
}

void AtackRocket::setStartSpeed(double speed) {
    	this->start_speed=speed;
}

void AtackRocket::setCurAngle(float angle) {
    	this->cur_angle=angle;
}

void AtackRocket::setX(float x) {
    	xyz[0]=x;
}

void AtackRocket::setY(float y) {
    	xyz[1]=y;
    	cur_height=y;
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
	this->mass_fuel=mass_fuel;
}

void AtackRocket::setFuelRashod(double fuel_rashod) {
	this->fuel_rashod=fuel_rashod;
}

void AtackRocket::setF(double f) {this->f=f;}

void AtackRocket::setS(float s) {this->s=s;}

void AtackRocket::setDrag(float drag) {this->drag=drag;}

void AtackRocket::setFDirection(float* f_direction) {this->f_direction=f_direction;}

double AtackRocket::getDistanceToTarget() const {

	return std::sqrt(std::pow(target_xyz[0]-xyz[0],2)+std::pow(target_xyz[1]-xyz[1],2)+std::pow(target_xyz[2]-xyz[2],2));

}

void AtackRocket::calculateTrajectory() {
	float dx=target_xyz[0]-xyz[0];
	float dy=target_xyz[1]-xyz[1];
	float dz=target_xyz[2]-xyz[2];


	float horizontal_dist=std::sqrt(dx*dx+dz*dz);

	float angle_xz=std::atan2(dz,dx);

	float angle=std::atan(4.0f*max_height/horizontal_dist);
	float angle_y=angle;
	cur_angle=angle*180.0f/M_PI;

	float v0=start_speed;

	velocity[0]=v0*std::cos(angle_y)*std::cos(angle_xz);
    	velocity[1]=v0*std::sin(angle_y);
    	velocity[2]=v0*std::cos(angle_y)*std::sin(angle_xz);
	

	double v=getCurrentSpeed();

	f_direction[0]=velocity[0]/v;
        f_direction[1]=velocity[1]/v;
        f_direction[2]=velocity[2]/v;
}




void AtackRocket::launch() {
	is_active=true;
	is_destroyed=false;
	time_alive=0.0;
	calculateTrajectory();
	mass_fuel=mass_fuel0;

	calculateTrajectory();
}

void AtackRocket::destroy() {
	is_destroyed=true;
	is_active=false;
	velocity[0]=0.0f;
	velocity[1]=0.0f;
	velocity[2]=0.0f;

}

const float RHO_0=1.225f;
const float H=8500.0f;

void AtackRocket::update(float deltaTime) {
	if(is_active==false or is_destroyed==true) return;
	const float G=9.81f;
	double mass=getMass()+getMassFuel();

	float F_x=0.0f;
	float F_y=0.0f;
	float F_z=0.0f;
	if(mass_fuel>0 and time_alive<max_time_alive) {
		F_x=f*f_direction[0];
		F_y=f*f_direction[1];
		F_z=f*f_direction[2];
		mass_fuel-=fuel_rashod*deltaTime;
		if(mass_fuel<=0) mass_fuel=0;

	}
	float FG_x=0.0f;
	float FG_y=-mass*G;
	float FG_z=0.0f;



	float rho=RHO_0*std::exp(-xyz[1]/H);
	double v=getCurrentSpeed();
	float FDrag_x=0.0f;
	float FDrag_y=0.0f;
	float FDrag_z=0.0f;
	float Drag_f=0.5f*rho*v*v*drag*s;
	FDrag_x=-Drag_f*velocity[0]/v;
        FDrag_y=-Drag_f*velocity[1] / v;
        FDrag_z=-Drag_f*velocity[2]/v;
	
	float F_total_x=FDrag_x+FG_x+F_x;
    	float F_total_y=FDrag_y+FG_y+F_y;
    	float F_total_z=FDrag_z+FG_z+F_z;
	
	float ax=F_total_x/mass;
    	float ay=F_total_y/mass;
    	float az=F_total_z/mass;
	
	velocity[0]+=ax*deltaTime;
    	velocity[1]+=ay*deltaTime;
    	velocity[2]+=az*deltaTime;

	xyz[0]+=velocity[0]*deltaTime;
    	xyz[1]+=velocity[1]*deltaTime;
    	xyz[2]+=velocity[2]*deltaTime;

	cur_height=xyz[1];
	float horizontal_speed=std::sqrt(velocity[0]*velocity[0]+velocity[2]*velocity[2]);
    	cur_angle=std::atan2(velocity[1], horizontal_speed) *180.0f/M_PI;
    	time_alive+=deltaTime;
//	float horizontal_speed=std::sqrt(std::pow(velocity[0],2)+std::pow(velocity[2],2));
//	cur_angle=std::atan2(velocity[1],horizontal_speed)*180.0f/M_PI;
//	time_alive+=deltaTime;


	if(xyz[1]<=0.0f) {xyz[1]=0.0f;destroy();return;}
	if(xyz[1]>max_height) {xyz[1]=max_height;velocity[1] = std::min(velocity[1],0.0f);}
	if (getDistanceToTarget()<1.0f) {destroy();return;}
}
