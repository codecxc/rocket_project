#include "AtackRocket.h"
#include "DefenderRocket.h"
#include "map.h"
#include "Parametrs.h"
#include "Vec.cpp"
#include <random>
#include <ctime>
#include <iostream>
#include <fstream>
#include <iomanip>

class RandomGenerator {
private:
    std::mt19937 gen;
    
public:
    RandomGenerator() : gen(static_cast<unsigned int>(time(nullptr))) {}
    
    float getFloat(float min, float max) {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(gen);
    }
};



Map* createRandomMap(int width,int height,float size,int numCells,float startX,float startY,float baseZ) {
	RandomGenerator rng;


	Corner* c_map1=new Corner(startX, startY, baseZ);
    	Corner* c_map2=new Corner(startX+width,startY,baseZ);
    	Corner* c_map3=new Corner(startX+width,startY+height,baseZ);
    	Corner* c_map4=new Corner(startX,startY+height,baseZ);

	std::vector<Cell*> cells;
	for(int i=0;i<numCells;++i) {
		float cellX=startX+rng.getFloat(0.0f,width-size);
        	float cellY=startY+rng.getFloat(0.0f,height-size);
		
		Corner* c1=new Corner(cellX,cellY,baseZ);
		Corner* c2=new Corner(cellX+size,cellY,baseZ);
		Corner* c3=new Corner(cellX+size,cellY+size,baseZ);
		Corner* c4=new Corner(cellX,cellY+size,baseZ);

		Cell* cell=new Cell(c1,c2,c3,c4);
		
		Corner* center=cell->getCenter();
		Parametrs* p=new Parametrs(45.0f,0.0f,0.0f,0.0f,500.0f,100.0f,center->getX(),center->getY(),center->getZ());
		delete center;
		DefenderRocket* def=new DefenderRocket(150,700,100,cell,0.6f,p,8000.0f);
		cell=new Cell(c1,c2,c3,c4,def);
		cells.push_back(cell);
	}
	return new Map(c_map1, c_map2, c_map3, c_map4, cells);
}


void printMap(Map* map,int numCells) {
	for(int i=0;i<numCells;++i) {
		Cell* cell=map->getCellNumber(i);
		Corner* center=cell->getCenter();
		std::cout<<"Ячейка "<<i<<": "<<center->getX()<<" "<<center->getY()<<" "<<center->getZ()<<std::endl;
		delete center;
	}

}




bool isPointInCell(float x,float y,Cell* cell) {
	std::vector<Corner*> corners=cell->getCorners();
	float minX=corners[0]->getX();
    	float maxX=corners[2]->getX();
    	float minY=corners[0]->getY();
    	float maxY=corners[2]->getY();
	
	return (x>=minX and x<=maxX and y>=minY and y<=maxY);
}

AtackRocket* createAtack(Map* map,float mapStartX,float mapStartY,float mapWidth, float mapHeight,int numCells,float baseZ) {
	RandomGenerator rng;
	float x, y;
    	bool insideCell=true;
	while(insideCell==true) {
		x=mapStartX+rng.getFloat(0.0f,mapWidth);
        	y=mapStartY+rng.getFloat(0.0f,mapHeight);
		insideCell=false;
		for(int i=0;i<numCells;++i) {
			if(isPointInCell(x,y,map->getCellNumber(i))) {
				insideCell=true;
				break;
			}
		}
		
	}
	int targetCell=rng.getFloat(0,numCells);
	Corner* targetCenter=map->getCellNumber(targetCell)->getCenter();
	AtackRocket* attack=new AtackRocket(200,1000,100,x,y,baseZ,targetCenter->getX(), targetCenter->getY(), targetCenter->getZ(),45.0f,200,80,0.5,3000,0.5f,0.3f);
	delete targetCenter;
	return attack;
}


int main() {
	const int MAP_WIDTH=100;
	const int MAP_HEIGHT=100;
	const float CELL_SIZE=5.0f;
	const int NUM_CELLS=10;
	
	RandomGenerator rng;
    	float startX=rng.getFloat(-50.0f, 50.0f);
    	float startY=rng.getFloat(-50.0f, 50.0f);
    	float baseZ=rng.getFloat(0.0f, 5.0f);


	Map* Map=createRandomMap(MAP_WIDTH,MAP_HEIGHT,CELL_SIZE,NUM_CELLS,startX,startY,baseZ);
	printMap(Map,NUM_CELLS);
	AtackRocket* attacker=createAtack(Map,startX,startY,MAP_WIDTH,MAP_HEIGHT,NUM_CELLS,baseZ);
	std::cout<<"Атакующая ракета: "<< attacker->getX()<< " "<<attacker->getY()<<" "<<attacker->getZ()<<std::endl;
	std::cout << "Цель: "<<attacker->getTargetX()<<" "<<attacker->getTargetY()<<" "<<attacker->getTargetZ()<<std::endl;
	
	std::ofstream trajectoryFile("trajectories.csv");
	trajectoryFile << "time,type,id,x,y,z\n";
	trajectoryFile << std::fixed << std::setprecision(6);
	float time=0.0f;
	
	float dt=0.1f;
	attacker->launch();
	for(int i=0;i<NUM_CELLS;++i) {
		Map->getCellNumber(i)->getDefenderRocket()->launch();
	}
	while(attacker->isActive()) {


		trajectoryFile << time << ",attacker," << 0 << ","
                   << attacker->getX() << "," 
                   << attacker->getY() << "," 
                   << attacker->getZ() << "\n";
		

		attacker->update(dt);
		for(int i=0;i<NUM_CELLS;++i) {
			DefenderRocket* defender=Map->getCellNumber(i)->getDefenderRocket();
			defender->update(dt, attacker);
			
			trajectoryFile << time << ",defender," << i << ","
                       << defender->getX() << "," 
                       << defender->getY() << "," 
                       << defender->getZ() << "\n";

		}
		time+=dt;

	}

	trajectoryFile.close();
	delete attacker;
	delete Map;
	return 0;
}
