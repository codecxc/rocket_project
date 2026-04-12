#include "AtackRocket.h"

AtackRocket::AtackRocket(double mass,double max_height,double start_speed,float start_x, float start_y, float start_zfloat target_x, float target_y, float target_z):mass(mass),max_height(max_height), cur_height(start_y),start_speed(start_speed), cur_angle(45.0f),is_active(false), is_destroyed(false), time_alive(0.0) {
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
