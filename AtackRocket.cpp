#include "AtackRocket.h"
#include <cmath>

AtackRocket::AtackRocket(double mass,double max_height,double start_speed,float start_x, float start_y, float start_z,float target_x, float target_y, float target_z):mass(mass),max_height(max_height), cur_height(start_y),start_speed(start_speed), cur_angle(45.0f),is_active(false), is_destroyed(false), time_alive(0.0) {
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
}

AtackRocket::~AtackRocket() {
	delete[] xyz;
	delete[] velocity;
	delete[] target_xyz;
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

float AtackRocket::getVelocityX() const {
    	return velocity[0];
}

float AtackRocket::getVelocityY() const {
    	return velocity[1];
}

float AtackRocket::getVelocityZ() const {
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


void AtackRocket::setVelocityX(float vx) {
    	velocity[0] = vx;
}

void AtackRocket::setVelocityY(float vy) {
    	velocity[1] = vy;
}

void AtackRocket::setVelocityZ(float vz) {
    	velocity[2] = vz;
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

void AtackRocket::calculateTrajectory() {
	float dx=target_xyz[0]-xyz[0];
	float dy=target_xyz[1]-xyz[1];
	float dz=target_xyz[2]-xyz[2];

	// все по траектории под 45 градусов см. конструктор
	float angle_rad=cur_angle*M_PI/180.0f;
	float horizontal_dist=std::sqrt(dx*dx+dz*dz);

	float vx=(dx/horizontal_dist)*start_speed*std::cos(angle_rad);
	float vy=start_speed*std::sin(angle_rad);
	float vz=(dz/horizontal_dist)*start_speed*std::cos(angle_rad);
	velocity[0]=vx;
	velocity[1]=vy;
	velocity[2]=vz;
}



double AtackRocket::getDistanceToTarget() const {
	return std::sqrt(std::pow(target_xyz[0]-xyz[0],2)+std::pow(target_xyz[1]-xyz[1],2)+std::pow(target_xyz[2]-xyz[2],2));

}


void AtackRocket::launch() {
	is_active=true;
	is_destroyed=false;
	time_alive=0.0;
	calculateTrajectory();
}

void AtackRocket::destroy() {
	is_destroyed=true;
	is_active=false;
	velocity[0]=0.0f;
	velocity[1]=0.0f;
	velocity[2]=0.0f;

}

void AtackRocket::update(float deltaTime) {
	const float G=9.81f;
	xyz[0]+=velocity[0]*deltaTime;
    	xyz[1]+=velocity[1]*deltaTime;
    	xyz[2]+=velocity[2]*deltaTime;


	velocity[1]-=G*deltaTime;
	cur_height=xyz[1];

	float horizontal_speed=std::sqrt(std::pow(velocity[0],2)+std::pow(velocity[2],2));
	cur_angle=std::atan2(velocity[1],horizontal_speed)*180.0f/M_PI;
	time_alive+=deltaTime;


	if(xyz[1]<=0.0f) {xyz[1]=0.0f;destroy();}
	if(xyz[1]>max_height) {xyz[1]=max_height;}
	if (getDistanceToTarget()<0.1) {destroy();}
}
