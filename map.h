#ifndef MAP_H
#define MAP_H
#include <vector>

class DefenderRocket;


struct Corner {
        float x=0.0f,y=0.0f,z=0.0f;
	Corner(float x, float y,float z);
	Corner();
	float getX() const;
	float getY() const;
	float getZ() const;
	void setX(float x);
	void setY(float y);
	void setZ(float z);
};

class Cell {
	protected:
		Corner* c1,*c2,*c3,*c4;
		bool is_have_defender_rocket;
		DefenderRocket* r;
	public:
		Cell(Corner* c1,Corner* c2,Corner* c3,Corner* c4,DefenderRocket* r=nullptr);
		Cell();
		std::vector<Corner*> getCorners() const;
		DefenderRocket* getDefenderRocket() const;
		Corner* getCenter() const;
		~Cell();
};

class Map {
	private:
		std::vector<Cell*> map;
		Corner* c_map1,*c_map2,*c_map3,*c_map4;
	public:
		Map(Corner* c_map1,Corner* c_map2,Corner* c_map3,Corner* c_map4, std::vector<Cell*> map);
		Cell* getCellNumber(int num) const;
		~Map();
};


#endif
