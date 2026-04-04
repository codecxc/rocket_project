#ifndef DEFENDERROCKET_H
#define DEFENDERROCKET_H

#include "Parametrs.h"

class Corner;
class Cell;

class DefenderRocket {
	protected:
		Corner* s;
		double start_massa;
        float fuel_to_mass; // m_fuel/massa
		float start_fuel; // %
		double maximal_speed;
		Cell* start_cell;
		Parametrs* p;
	public:
		DefenderRocket(double start_massa,double maximal_speed,float start_fuel, Cell* start_cell,float fuel_to_mass,Parametrs* p);
		~DefenderRocket();
		double getStartMassa() const;
		float getStartFuel() const;
        float getFuelToMass() const;
		double getMaximalSpeed() const;
		Cell* getStartCell() const;
		Parametrs* getP() const;
		void setP(std::vector<float>& v);

};

#endif
