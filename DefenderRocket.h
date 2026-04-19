#ifndef DEFENDERROCKET_H
#define DEFENDERROCKET_H

#include "Parametrs.h"

class Corner;
class Cell;
class AtackRocket;

class DefenderRocket {
	protected:
		Corner* s;
		double start_mass;
        	float fuel_to_mass; // m_fuel/massa
		float start_fuel; // %
		double maximal_speed;
		Cell* start_cell;
		Parametrs* p;
		
		float f;
		bool is_active;
		bool is_destroyed;
		double time_alive;
		float current_fuel;
		double current_mass;
	public:
		DefenderRocket(double start_massa,double maximal_speed,float start_fuel, Cell* start_cell,float fuel_to_mass,Parametrs* p,float f);
		~DefenderRocket();
		double getStartMass() const;
		float getStartFuel() const;
        	float getFuelToMass() const;
		double getMaximalSpeed() const;
		Cell* getStartCell() const;
		Parametrs* getP() const;
		void setP(std::vector<float>& v);
		
		float getX() const;
		float getY() const;
		float getZ() const;
		float getVX() const;
    		float getVY() const;
    		float getVZ() const;
		bool isActive() const;
    		bool isDestroyed() const;
    		double getTimeAlive() const;
		double getCurrentSpeed() const;
    		float getFuel() const;
		void updateFuel(float deltaTime,float a);
    		double getCurrentMass() const;
		void launch();

		void destroy();
		void update(float deltaTime,AtackRocket* target);
		void calculateTrajectory(AtackRocket* target,float dt);
		double getDistanceToTarget(AtackRocket* target) const;
};

#endif
