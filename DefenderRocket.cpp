#include "DefenderRocket.h"
#include "map.h"


DefenderRocket::DefenderRocket(double start_massa,double maximal_speed,float start_fuel, Cell* start_cell,float fuel_to_mass,Parametrs* p):p(p),start_massa(start_massa),maximal_speed(maximal_speed),start_fuel(start_fuel),start_cell(start_cell),fuel_to_mass(fuel_to_mass) {
	if(s!=nullptr) {
		s=start_cell->getCenter();
	}
	else {s=nullptr;}
}
DefenderRocket::~DefenderRocket() {
	// delete s;
}

double DefenderRocket::getStartMassa() const {
	return start_massa;
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
