#ifndef DEFENDERROCKET_H
#define DEFENDERROCKET_H

#include "Parametrs.h"

class Corner;
class Cell;
class AtackRocket;

class DefenderRocket {
	protected:
		Corner* s;
		double mass; // масса каркаса
//       	float fuel_to_mass; // m_fuel/massa
		float start_fuel;
		Cell* start_cell;
		Parametrs* p;
		
		float f;
		bool is_active;
		bool is_destroyed;
		double time_alive;
		float current_fuel;
	public:
		DefenderRocket(double mass,float start_fuel, Cell* start_cell,Parametrs* p,float f);
		~DefenderRocket();
		double getMass() const;
		float getStartFuel() const;
//        	float getFuelToMass() const;
		// double getMaximalSpeed() const;
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
		void launch(AtackRocket* target);

		void destroy();
		void update(float deltaTime,AtackRocket* target);
		void calculateTrajectory(AtackRocket* target,float dt);
		double getDistanceToTarget(AtackRocket* target) const;


		void calculateV(AtackRocket* target, float dt);
		void applyBraking(AtackRocket* target, float dt);

};

#endif
