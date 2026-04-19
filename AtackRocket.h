#ifndef ATACKROCKET_H
#define ATACKROCKET_H

class AtackRocket {
	protected:
		double mass;
		
		double max_height;
		double cur_height;
		double start_speed;
		float cur_angle;
		float* xyz;
		float* velocity;
		float* target_xyz;
		bool is_active;
		bool is_destroyed;
		double time_alive;
		
		double max_time_alive;
		double mass_fuel0;
		double mass_fuel;
		double fuel_rashod;
		double f; // сила
		float* f_direction;

		float s;
		float drag;
	public:
		AtackRocket();
		AtackRocket(double mass, double max_height, double start_speed,float start_x, float start_y, float start_z,float target_x, float target_y, float target_z,float cur_angle, double max_time_alive, double mass_fuel0,double fuel_rashod, double f,float s, float drag);
		~AtackRocket();
		double getMass() const; // масса корпуса
		double getMassFuel() const; // масса топлива
		void setMassFuel(double fuel_mass);
		double getFuelRashod() const;
		double getF() const;
		float* getFDirection() const;
		void setFuelRashod(double fuel_rashod);
		void setF(double f);
		float getS() const;
		float getDrag() const;
		void setS(float s);
		void setDrag(float drag);
		void setFDirection(float* f_direction);
		double getMaxTimeAlive() const; //
    		double getMaxHeight() const;
    		double getCurHeight() const;
    		double getStartSpeed() const;
    		float getCurAngle() const;
    		float getX() const;
    		float getY() const;
    		float getZ() const;
    		float getVX() const;
    		float getVY() const;
    		float getVZ() const;
    		double getCurrentSpeed() const;
    		float getTargetX() const;
    		float getTargetY() const;
    		float getTargetZ() const;

    		bool isActive() const;
    		bool isDestroyed() const;
    		double getTimeAlive() const;
		

		void setMass(double mass);
    		void setMaxHeight(double height);
    		void setCurHeight(double height);
    		void setStartSpeed(double speed);
    		void setCurAngle(float angle);
    		void setX(float x);
    		void setY(float y);
    		void setZ(float z);
    		void setVX(float vx);
    		void setVY(float vy);
    		void setVZ(float vz);
    		void setTarget(float x, float y, float z);
    		void setActive(bool active);
    		void setDestroyed(bool destroyed);
    		void setTimeAlive(double time);


		void launch();
		void destroy();
		double getDistanceToTarget() const;
		void update(float deltaTime);
		void calculateTrajectory();

};

#endif
