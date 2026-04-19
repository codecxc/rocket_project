#include "DefenderRocket.h"
#include "map.h"
#include <cmath>
#include <algorithm>
#include "AtackRocket.h"
#include "Vec.cpp"
DefenderRocket::DefenderRocket(double start_mass,double maximal_speed,float start_fuel, Cell* start_cell,float fuel_to_mass,Parametrs* p,float f):p(p),start_mass(start_mass),maximal_speed(maximal_speed),start_fuel(start_fuel),start_cell(start_cell),fuel_to_mass(fuel_to_mass),s(nullptr),f(f),is_active(false),is_destroyed(false),time_alive(0.0),current_fuel(0.0f),current_mass(start_mass) {
	if(start_cell!=nullptr) {
		s=start_cell->getCenter();
	}
}
DefenderRocket::~DefenderRocket() {
	// delete s;
}

double DefenderRocket::getStartMass() const {
	return start_mass;
}

float DefenderRocket::getStartFuel() const {
	return start_fuel;
}

double DefenderRocket::getMaximalSpeed() const {
	return maximal_speed;

}

float DefenderRocket::getFuelToMass() const {
        return fuel_to_mass;
}

Cell* DefenderRocket::getStartCell() const {
	return start_cell;
}

Parametrs* DefenderRocket::getP() const {
	return p;
}

void DefenderRocket::setP(std::vector<float>& v) {
	p->setParametrs(v);
}

float DefenderRocket::getX() const {
	std::vector<float> p1=p->getParametrs();
    	return p1[6];
}
float DefenderRocket::getY() const {
        std::vector<float> p1=p->getParametrs();
        return p1[7];
}
float DefenderRocket::getZ() const {
        std::vector<float> p1=p->getParametrs();
        return p1[8];
}


float DefenderRocket::getVX() const {
        std::vector<float> p1=p->getParametrs();
        return p1[1];
}

float DefenderRocket::getVY() const {
        std::vector<float> p1=p->getParametrs();
        return p1[2];
}

float DefenderRocket::getVZ() const {
        std::vector<float> p1=p->getParametrs();
        return p1[3];
}

double DefenderRocket::getCurrentSpeed() const {
        std::vector<float> p1=p->getParametrs();
        return p1[4];
}

float DefenderRocket::getFuel() const {
        return current_fuel;
}

double DefenderRocket::getCurrentMass() const {
    	return current_mass;
}

bool DefenderRocket::isActive() const {
    	return is_active;
}

bool DefenderRocket::isDestroyed() const {
    	return is_destroyed;
}

double DefenderRocket::getTimeAlive() const {
    	return time_alive;
}

void DefenderRocket::launch() {
	is_active=true;
	is_destroyed=false;
	time_alive=0.0;
	current_fuel=start_fuel;
	current_mass=start_mass;
	
	std::vector<float>p1=p->getParametrs();
	if(s!=nullptr) {
	p1[6]=s->getX();
	p1[7]=s->getY();
	p1[8]=s->getZ();
	} else {
	p1[6]=0.0f;
        p1[7]=0.0f;
        p1[8]=0.0f;
	}
	p1[1]=0.0f;
	p1[2]=10.0f;
	p1[3]=0.0f;
	p1[4]=10.0f;
	p1[5]=current_fuel;
	p->setParametrs(p1);
}

void DefenderRocket::destroy() {
	is_destroyed=true;
    	is_active=false;
}


double DefenderRocket::getDistanceToTarget(AtackRocket* target) const {
	return std::sqrt(std::pow(target->getX()-getX(),2)+std::pow(target->getY()-getY(),2)+std::pow(target->getZ()-getZ(),2));
	

}

const float N=4.0f;
const float G=9.81f;

void DefenderRocket::calculateTrajectory(AtackRocket* target,float dt) {
	if(target->isActive()==false or current_fuel<=0.0f) return;
	
	std::vector<float>p1=p->getParametrs();
	Vec R(target->getX()-getX(),target->getY()-getY(),target->getZ()-getZ());

	float r_len=R.getLength();
	if(r_len<0.5f) return;
	Vec R_n=R.normalized();
	Vec V_target(target->getVX(),target->getVY(),target->getVZ());
    	Vec V_defender(p1[1],p1[2],p1[3]);
	Vec V_rel=V_target-V_defender;

	float Vc=-R_n.scalar(V_rel);
	Vec omega=R.cross(V_rel)/(r_len*r_len);
	Vec a=omega.cross(R_n)*(N*Vc);
	Vec a_g(0,G,0);
	Vec a_total=a+a_g;
	Vec f_direction=a_total.normalized();
	
	float a1=f/current_mass;
	Vec velocity_change=f_direction*(a1*dt);

	Vec new_velocity(p1[1]+velocity_change.getX(),p1[2]+velocity_change.getY(),p1[3]+velocity_change.getZ());
	float speed=new_velocity.getLength();
	p1[4]=speed;
	p1[1]=new_velocity.getX();
	p1[2]=new_velocity.getY();
	p1[3]=new_velocity.getZ();
	float horizontal_speed=std::sqrt(p1[1]*p1[1]+p1[3]*p1[3]);
    	p1[0]=std::atan2(p1[2],horizontal_speed)*180.0f/M_PI;
    	p->setParametrs(p1);


}




const float I=250.0f;


void DefenderRocket::updateFuel(float deltaTime, float a) {
    	if(current_fuel<=0.0f) {
        	current_fuel=0.0f;
        	return;
    	}

//    	float fuel_c=a*deltaTime*0.01f;
//    	current_fuel-=fuel_c;
//    	if(current_fuel<0.0f) current_fuel=0.0f;
//    	current_mass=start_mass*(1.0-fuel_to_mass*(1.0-current_fuel/100.0f));

	float dm=f/(I*G);
	float fuel_used=dm*deltaTime;
	current_fuel-=fuel_used;



	float m_structure=start_mass*(1.0f-fuel_to_mass);
    	float m_fuel_current=start_mass*fuel_to_mass*(current_fuel/start_fuel);
    	current_mass=m_structure+m_fuel_current;
}

const float RHO_0 = 1.225f;
const float H=8500.0f;
const float DRAG=0.25f;
const float S=0.05f;
void DefenderRocket::update(float deltaTime, AtackRocket* target) {
	if(is_active==false or is_destroyed==true) return;
	std::vector<float>p1=p->getParametrs();
	if (current_fuel >0.0f) {
        	calculateTrajectory(target,deltaTime);
        	p1=p->getParametrs();
    	}
	float h=p1[7];
	float rho=RHO_0*std::exp(-h/H);
	float speed=p1[4];
	float drag_f=0.5f*rho*speed*speed*DRAG*S;
	float drag_a=drag_f/current_mass;
	if(speed>0) {
	float v_norm_x=p1[1]/speed;
        float v_norm_y=p1[2]/speed;
        float v_norm_z=p1[3]/speed;
	p1[1]-=v_norm_x * drag_a*deltaTime;
        p1[2]-=v_norm_y * drag_a*deltaTime;
        p1[3]-=v_norm_z * drag_a*deltaTime;
	}
	p1[2]-=G*deltaTime;
	p1[4]=std::sqrt(p1[1]*p1[1]+p1[2]*p1[2]+p1[3]*p1[3]);
	p1[6]+=p1[1]*deltaTime;
	p1[7]+=p1[2]*deltaTime;
	p1[8]+=p1[3]*deltaTime;
	updateFuel(deltaTime,0.0f);
    	p1[5]=current_fuel;
	p->setParametrs(p1);
    	time_alive+=deltaTime;

	if (target && getDistanceToTarget(target)<3.0f) {
        	destroy();
        	target->destroy();
        	return;
    	}
}
