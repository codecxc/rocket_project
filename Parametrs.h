#ifndef PARAMETRS_H
#define PARAMETRS_H
#include <vector>
class Parametrs {
	protected:
		float cur_angle;
		float cur_vx;
		float cur_vy;
		float cur_vz;
		float cur_v;
		float fuel;
		float cur_x;
		float cur_y;
		float cur_z;
	public:
		Parametrs(float cur_angle,float cur_vx,float cur_vy,float cur_vz,float cur_v,float fuel,float cur_x,float cur_y,float cur_z);
		std::vector<float> getParametrs() const;
		void setParametrs(std::vector<float>& v);
};

#endif
