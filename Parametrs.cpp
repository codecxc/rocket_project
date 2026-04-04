#include "Parametrs.h"


Parametrs::Parametrs(float cur_angle,float cur_vx,float cur_vy,float cur_vz,float cur_v,float fuel,float cur_x,float cur_y,float cur_z):cur_angle(cur_angle),cur_vx(cur_vx),cur_vy(cur_vy),cur_vz(cur_vz),cur_v(cur_v),fuel(fuel),cur_x(cur_x),cur_y(cur_y),cur_z(cur_z) {}
std::vector<float>Parametrs::getParametrs() const {
	std::vector<float> p;
	p.push_back(cur_angle);
	p.push_back(cur_vx);
	p.push_back(cur_vy);
	p.push_back(cur_vz);
	p.push_back(cur_v);
	p.push_back(fuel);
	p.push_back(cur_x);
	p.push_back(cur_y);
	p.push_back(cur_z);
	return p;
}

void Parametrs::setParametrs(std::vector<float>& v) {
	cur_angle=v[0];
	cur_vx=v[1];
	cur_vy=v[2];
	cur_vz=v[3];
	cur_v=v[4];
	fuel=v[5];
	cur_x=v[6];
	cur_y=v[7];
	cur_z=v[8];
}
