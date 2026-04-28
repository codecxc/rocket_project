#include "DefenderRocket.h"
#include "map.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include "AtackRocket.h"
#include "Vec.cpp"

const float RHO_0 = 1.225f;
const float H_SCALE=8500.0f;
const float DRAG_CD=0.25f;
const float DRAG_S=0.05f;
const float I=250.0f;
const float KILL_RADIUS=120.0f;

const float PN_N=1.0f;
const float G=9.81f;
const float S=0.05f;



DefenderRocket::DefenderRocket(double start_mass,double maximal_speed,float start_fuel, Cell* start_cell,Parametrs* p,float f):p(p),start_mass(start_mass),maximal_speed(maximal_speed),start_fuel(start_fuel),start_cell(start_cell),s(nullptr),f(f),is_active(false),is_destroyed(false),time_alive(0.0),current_fuel(0.0f),current_mass(start_mass) {
	if(start_cell!=nullptr) {
		s=start_cell->getCenter();
		std::vector<float>p1=p->getParametrs();
		p1[6]=s->getX();
		p1[7]=s->getY();
		p1[8]=s->getZ();
		p1[5]=current_fuel;
		p1[3]=100;
		p->setParametrs(p1);
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

//float DefenderRocket::getFuelToMass() const {
//        return fuel_to_mass;
//}

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





void DefenderRocket::calculateV(AtackRocket* target, float /*dt*/) {
    const int MAX_ITER = 30;
    const float TOLERANCE = 0.5f;

    float Rx = getX(), Ry = getY(), Rz = getZ();
    float Tx = target->getX(), Ty = target->getY(), Tz = target->getZ();
    float Vx = target->getVX(), Vy = target->getVY(), Vz = target->getVZ();
    float Vd = static_cast<float>(maximal_speed);


    float dx0 = Tx - Rx, dy0 = Ty - Ry, dz0 = Tz - Rz;
    float t = std::sqrt(dx0*dx0 + dy0*dy0 + dz0*dz0) / Vd;

    for (int i = 0; i < MAX_ITER; ++i) {
        float Px = Tx + Vx * t;
        float Py = Ty + Vy * t;
        float Pz = Tz + Vz * t;

        float dx = Px - Rx, dy = Py - Ry, dz = Pz - Rz;
        float dist = std::sqrt(dx*dx + dy*dy + dz*dz);
        float t_new = dist / Vd;

        if (std::fabs(t_new - t) < TOLERANCE / Vd) { t = t_new; break; }
        t = t_new;
    }


    float Px = Tx + Vx * t;
    float Py = Ty + Vy * t;
    float Pz = Tz + Vz * t;
    float dx = Px - Rx, dy = Py - Ry, dz = Pz - Rz;
    float dist = std::sqrt(dx*dx + dy*dy + dz*dz);
    if (dist < 1e-3f) return;

    std::vector<float> p1 = p->getParametrs();
    p1[1] = (dx / dist) * Vd;
    p1[2] = (dy / dist) * Vd;
    p1[3] = (dz / dist) * Vd;
    p1[4] = Vd;
    p->setParametrs(p1);
}










// void DefenderRocket::launch(AtackRocket* target) {
// 	is_active=true;
// 	is_destroyed=false;
// 	time_alive=0.0;
// 	current_fuel=start_fuel;
// 	current_mass=start_mass+current_fuel;
// 	// std::vector<float>p1=p->getParametrs();
// 	// if(s!=nullptr) {
// 	// p1[6]=s->getX();
// 	// p1[7]=s->getY();
// 	// p1[8]=s->getZ();
// 	// } else {
// 	// p1[6]=0.0f;
//     //     p1[7]=0.0f;
//     //     p1[8]=0.0f;
// 	// }
// 	// p1[1]=0.0f;
// 	// p1[2]=0.0f;
// 	// p1[3]=start_;
// 	// p1[4]=0.0f;
// 	// p1[5]=current_fuel;
// 	// p->setParametrs(p1);
// 	calculateV(target, 0.0f);
// }
void DefenderRocket::launch(AtackRocket* target) {
    is_active=true;
    is_destroyed=false;
    time_alive=0.0;
    current_fuel=start_fuel;
    current_mass=start_mass+current_fuel;
    calculateV(target, 0.0f);
}

void DefenderRocket::destroy() {
	is_destroyed=true;
    	is_active=false;
}


double DefenderRocket::getDistanceToTarget(AtackRocket* target) const {
	return std::sqrt(std::pow(target->getX()-getX(),2)+std::pow(target->getY()-getY(),2)+std::pow(target->getZ()-getZ(),2));
	

}


// void DefenderRocket::calculateTrajectory(AtackRocket* target,float dt) {
// 	if(target->isActive()==false or current_fuel<=0.0f) return;
	
// 	std::vector<float>p1=p->getParametrs();
// 	Vec R(target->getX()-getX(),target->getY()-getY(),target->getZ()-getZ()); // вектор атакующей
// //	Vec X(1,0,0);Vec Y(0,1,0);Vec Z(0,0,1);
// //	p1[1]=p1[4]*R.scalar(X);
// //	p1[2]=p1[4]*R.scalar(Y);
// //	p1[3]=p1[4]*R.scalar(Z);
// 	float r_len=R.getLength();
// 	if(r_len<0.5f) return;
// 	Vec R_n=R.normalized(); // единичный вектор А
// 	Vec V_target(target->getVX(),target->getVY(),target->getVZ());
//     	Vec V_defender(p1[1],p1[2],p1[3]);
// 	Vec V_rel=V_target-V_defender;

// 	float Vc=-R_n.scalar(V_rel);
// 	Vec omega=R.cross(V_rel)/(r_len*r_len);
// 	Vec a=omega.cross(V_rel)*(N);
// 	Vec a_g(0,0,-G);
// 	Vec a_total=a-a_g;
// 	Vec f_direction=a_total.normalized();
	
// 	float a1=f/current_mass;
// 	Vec velocity_change=f_direction*(a1*dt);

// 	Vec new_velocity(p1[1]+velocity_change.getX(),p1[2]+velocity_change.getY(),p1[3]+velocity_change.getZ());
// 	float speed=new_velocity.getLength();
// 	p1[4]=speed;
// 	p1[1]=new_velocity.getX();
// 	p1[2]=new_velocity.getY();
// 	p1[3]=new_velocity.getZ();
// 	float horizontal_speed=std::sqrt(p1[1]*p1[1]+p1[2]*p1[2]);
//     	p1[0]=std::atan2(p1[3],horizontal_speed)*180.0f/M_PI;
//     	p->setParametrs(p1);


// }

void DefenderRocket::calculateTrajectory(AtackRocket* target, float dt) {
    if (!target->isActive() or current_fuel <= 0.0f) return;

    const int MAX_ITER = 20;
    const float TOLERANCE = 0.5f;
    float Vd=maximal_speed;

    float Rx=getX(), Ry=getY(), Rz=getZ();
    float Tx=target->getX(), Ty=target->getY(), Tz=target->getZ();
    float TVx=target->getVX(), TVy=target->getVY(), TVz=target->getVZ();

    float dx0=Tx-Rx, dy0=Ty-Ry, dz0=Tz-Rz;
    float t=std::sqrt(dx0*dx0 + dy0*dy0 + dz0*dz0) / Vd;
    for (int i=0; i<MAX_ITER; ++i) {
        float Px=Tx+TVx * t, Py = Ty + TVy * t, Pz = Tz + TVz * t;
        float dx=Px - Rx, dy = Py - Ry, dz = Pz - Rz;
        float dist=std::sqrt(dx*dx + dy*dy + dz*dz);
        float t_new=dist/Vd;
        if (std::fabs(t_new-t)<TOLERANCE/Vd) {t=t_new;break;}
        t=t_new;
    }
	float Px = Tx + TVx * t, Py = Ty + TVy * t, Pz = Tz + TVz * t;
    float dx = Px - Rx, dy = Py - Ry, dz = Pz - Rz;
    float dist = std::sqrt(dx*dx + dy*dy + dz*dz);
    if (dist < 1e-3f) return;
    float desired_vx = (dx / dist) * Vd;
    float desired_vy = (dy / dist) * Vd;
    float desired_vz = (dz / dist) * Vd;

    const float TURN_RATE = 10.0f; // рад/с
    float alpha=std::min(1.0f, TURN_RATE * dt);

    std::vector<float> p1 = p->getParametrs();
    float new_vx = p1[1] + alpha * (desired_vx - p1[1]);
    float new_vy = p1[2] + alpha * (desired_vy - p1[2]);
    float new_vz = p1[3] + alpha * (desired_vz - p1[3]);
    float spd = std::sqrt(new_vx*new_vx + new_vy*new_vy + new_vz*new_vz);
    if (spd > 1e-3f) {
        float scale = Vd / spd;
        new_vx *= scale;
        new_vy *= scale;
        new_vz *= scale;
    }

    p1[1] = new_vx;
    p1[2] = new_vy;
    p1[3] = new_vz;
    p1[4] = Vd;
    float horiz = std::sqrt(new_vx*new_vx + new_vy*new_vy);
    p1[0] = std::atan2(new_vz, horiz) * 180.0f / M_PI;
    p->setParametrs(p1);
}






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



    	
    	current_mass=start_mass+current_fuel;
}


// void DefenderRocket::update(float deltaTime, AtackRocket* target) {
// 	if(is_active==false or is_destroyed==true) return;
// 	std::vector<float>p1=p->getParametrs();
// 	if (current_fuel >0.0f) {
//         	calculateTrajectory(target,deltaTime);
//         	p1=p->getParametrs();
//     	}
// 	float h=p1[8];
// 	float rho=RHO_0*std::exp(-h/H);
// 	float speed=p1[4];
// 	float drag_f=0.5f*rho*speed*speed*DRAG*S;
// 	float drag_a=drag_f/current_mass;
// 	if(speed>0) {
// 	float v_norm_x=p1[1]/speed;
//         float v_norm_y=p1[2]/speed;
//         float v_norm_z=p1[3]/speed;
// 	p1[1]-=v_norm_x * drag_a*deltaTime;
//         p1[2]-=v_norm_y * drag_a*deltaTime;
//         p1[3]-=v_norm_z * drag_a*deltaTime;
// 	}
// 	if(current_fuel<=0.0f) {
// 		p1[3]-=G*deltaTime;
// 	}
// 	p1[4]=std::sqrt(p1[1]*p1[1]+p1[2]*p1[2]+p1[3]*p1[3]);
// 	p1[6]+=p1[1]*deltaTime;
// 	p1[7]+=p1[2]*deltaTime;
// 	p1[8]+=p1[3]*deltaTime;
	
// 	if(p1[8]<0.0f) {p1[8]=0.0f; destroy();return;}
// 	if(current_fuel>0) updateFuel(deltaTime,0.0f);
//     	p1[5]=current_fuel;
// 	p->setParametrs(p1);
//     	time_alive+=deltaTime;

// 	if (getDistanceToTarget(target)<70.0f) {
//         	destroy();
//         	target->destroy();
//         	return;
//     	}
// }


void DefenderRocket::update(float deltaTime, AtackRocket* target) {
    if (!is_active or is_destroyed) return;

    std::vector<float> p1=p->getParametrs();

    if (current_fuel>0.0f) {
        calculateTrajectory(target, deltaTime);
        p1=p->getParametrs();
        updateFuel(deltaTime, 0.0f);
        p1[5]=current_fuel;
    } else {
        float h=p1[8];
        float rho=RHO_0*std::exp(-h/H_SCALE);
        float spd=p1[4];

        if (spd>0.0f) {
            float drag_a=(0.5f*rho*spd*spd*DRAG_CD*DRAG_S)/(current_mass);
            float inv_spd = 1.0f / spd;
            p1[1] -= (p1[1] * inv_spd) * drag_a * deltaTime;
            p1[2] -= (p1[2] * inv_spd) * drag_a * deltaTime;
            p1[3] -= (p1[3] * inv_spd) * drag_a * deltaTime;
        }


        p1[3]-=G*deltaTime;
        p1[4]=std::sqrt(p1[1]*p1[1] + p1[2]*p1[2] + p1[3]*p1[3]);
    }
    p1[6] += p1[1] * deltaTime;
    p1[7] += p1[2] * deltaTime;
    p1[8] += p1[3] * deltaTime;

    if (p1[8] < 0.0f) {p1[8]=0.0f;p->setParametrs(p1);destroy();return;}

    p->setParametrs(p1);
    time_alive += deltaTime;

    if (getDistanceToTarget(target) < KILL_RADIUS) {
        destroy();
        target->destroy();
    }
}


