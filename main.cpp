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
#include <vector>

// тут меняемые значения остальное на рандомах

const float MAP_WIDTH=5000.0f;
const float MAP_HEIGHT=5000.0f;
const int NUM_CELLS=10;
const float CELL_SIZE=100.0f;
const int NUM_ATTACKERS=NUM_CELLS;
const float START_Z=0.0f; // все в одной плоскости
const float MIN_CELL_DIST=200.0f;

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

bool isPointInCell(float x, float y, Cell* cell) {
    std::vector<Corner*> corners=cell->getCorners();
    float minX=corners[0]->getX(),maxX=corners[2]->getX();
    float minY=corners[0]->getY(),maxY=corners[2]->getY();
    return (x>=minX and x<=maxX and y>=minY and y<=maxY);
}


// Map* createFixedMap() {
//     float baseZ = 0.0f;

//     float fixedPositions[10][2] = {
//         {1000.0f, 1000.0f},
//         {1000.0f, 4000.0f},
//         {4000.0f, 1000.0f},
//         {4000.0f, 4000.0f},
//         {2500.0f, 1000.0f},
//         {2500.0f, 4000.0f},
//         {1000.0f, 2500.0f},
//         {4000.0f, 2500.0f},
//         {1500.0f, 2000.0f},
//         {3500.0f, 3000.0f}
//     };

//     Corner* c_map1 = new Corner(0.0f, 0.0f, baseZ);
//     Corner* c_map2 = new Corner(5000.0f, 0.0f, baseZ);
//     Corner* c_map3 = new Corner(5000.0f, 5000.0f, baseZ);
//     Corner* c_map4 = new Corner(0.0f, 5000.0f, baseZ);

//     std::vector<Cell*> cells;
//     for (int i = 0; i < 10; ++i) {
//         float cx = fixedPositions[i][0];
//         float cy = fixedPositions[i][1];
//         float size = 50.0f;

//         Corner* c1 = new Corner(cx - size / 2, cy - size / 2, baseZ);
//         Corner* c2 = new Corner(cx + size / 2, cy - size / 2, baseZ);
//         Corner* c3 = new Corner(cx + size / 2, cy + size / 2, baseZ);
//         Corner* c4 = new Corner(cx - size / 2, cy + size / 2, baseZ);

//         Cell* cell = new Cell(c1, c2, c3, c4);

//         Parametrs* p = new Parametrs(45.0f, 0.0f, 0.0f, 0.0f, 0.0f, 4000.0f, cx, cy, baseZ);
//         DefenderRocket* def = new DefenderRocket(100, 1000, 5000, cell, p, 15000.0f);
//         cell = new Cell(c1, c2, c3, c4, def);
//         cells.push_back(cell);
//     }
//     return new Map(c_map1, c_map2, c_map3, c_map4, cells);
// }

Map* createRandomMap(RandomGenerator& rng,float baseZ) {
    Corner* c_map1=new Corner(0.0f,0.0f,baseZ);
    Corner* c_map2=new Corner(MAP_WIDTH,0.0f,baseZ);
    Corner* c_map3=new Corner(MAP_WIDTH,MAP_HEIGHT,baseZ);
    Corner* c_map4=new Corner(0.0f,MAP_HEIGHT,baseZ);

    std::vector<Cell*> cells;
    std::vector<std::pair<float,float>> placed; // центры
    while((int)cells.size()<NUM_CELLS) {
        float cx=rng.getFloat(CELL_SIZE/2,MAP_WIDTH-CELL_SIZE/2);
        float cy=rng.getFloat(CELL_SIZE/2,MAP_HEIGHT-CELL_SIZE/2);
    
        bool temp=false; // дистаниция
        for (auto& pos:placed) {
            float dx=cx-pos.first;
            float dy=cy-pos.second;
            if(std::sqrt(dx*dx+ dy*dy)<MIN_CELL_DIST) {
                temp=true;
                break;
            }
        }
        if (temp) continue; // по новой

        placed.push_back({cx,cy});

        float half=CELL_SIZE/2.0f;
        Corner* c1=new Corner(cx-half, cy-half,baseZ);
        Corner* c2=new Corner(cx+half, cy-half,baseZ);
        Corner* c3=new Corner(cx+half, cy+half,baseZ);
        Corner* c4=new Corner(cx-half, cy+half,baseZ);

        Parametrs* p=new Parametrs(45.0f, 0.0f, 0.0f, 0.0f, 0.0f, 4000.0f, cx, cy, baseZ);
        new DefenderRocket(10,  1000, 5000, nullptr, p, 500000.0f);
        pv[6]=cx;
        pv[7]=cy;
        pv[8]=baseZ;
        p->setParametrs(pv);

        Cell* cell=new Cell(c1, c2, c3, c4, def);
        cells.push_back(cell);

    }

    return new Map(c_map1,c_map2,c_map3,c_map4,cells);
}

void printMap(Map* map, int numCells) {
    for (int i = 0; i < numCells; ++i) {
        Cell* cell = map->getCellNumber(i);
        Corner* center = cell->getCenter();
        std::cout << "Ячейка " << i << ": " << center->getX() << " " << center->getY() << " " << center->getZ() << std::endl;
        delete center;
    }
}

// bool isPointInCell(float x, float y, Cell* cell) {
//     std::vector<Corner*> corners = cell->getCorners();
//     float minX = corners[0]->getX();
//     float maxX = corners[2]->getX();
//     float minY = corners[0]->getY();
//     float maxY = corners[2]->getY();
//     return (x >= minX && x <= maxX && y >= minY && y <= maxY);
// }

AtackRocket* createAtack(RandomGenerator& rng, Map* map, int numCells,float targetX, float targetY, float targetZ) {
    float x,y,z;
    z=START_Z;
    bool insideCell=true;
    // z = rng.getFloat(100.0f, 1000.0f);
    while (insideCell) {
        x=rng.getFloat(0.0f,MAP_WIDTH);
        y=rng.getFloat(0.0f,MAP_HEIGHT);
        insideCell=false;
        for (int i = 0; i<numCells; ++i) {
            if(isPointInCell(x,y,map->getCellNumber(i))) {
                insideCell=true;
                break;
            }
        }
    }
    return new AtackRocket(100, 1500, 250, x, y, z,targetX, targetY, targetZ,45.0f, 30, 500, 2.5, 6000, 0.05f, 0.3f);
}

int main() {
    // const int MAP_WIDTH = 5000;
    // const int MAP_HEIGHT = 5000;
    // const int NUM_CELLS = 10;
    RandomGenerator rng;
    const float baseZ=0.0f;

    Map* Map=createRandomMap(rng,baseZ);
    printMap(Map,NUM_CELLS);

    std::vector<AtackRocket*> attackers;
    attackers.reserve(NUM_ATTACKERS);

    for (int i=0; i<NUM_ATTACKERS;++i) {
        int targetCell=rng.getFloat(0,NUM_CELLS-1);
        Corner* tc=Map->getCellNumber(targetCell)->getCenter();
        float tx=tc->getX();
        float ty=tc->getY();
        float tz=tc->getZ();
        delete tc;

        AtackRocket* attacker=createAtack(rng,Map,NUM_CELLS,tx,ty,tz);
        attackers.push_back(attacker);
        
    }

    // цели
    std::vector<AtackRocket*> defenderTargets(NUM_CELLS, nullptr);
    for (int i=0;i<NUM_CELLS;++i) {
        defenderTargets[i]=attackers[i%NUM_ATTACKERS];
    }
    // пуск
    for(auto* a:attackers) a->launch();
    for(int i=0;i<NUM_CELLS;++i) {
        Map->getCellNumber(i)->getDefenderRocket()->launch(defenderTargets[i]);
    }  

    






    // для графика

    std::ofstream trajectoryFile("attackrockets.txt");
    trajectoryFile << "time,type,id,x,y,z\n";
    trajectoryFile << std::fixed << std::setprecision(6);

    float time = 0.0f;
    const float dt       = 0.01f;
    const float MAX_TIME = 300.0f;

    while (time < MAX_TIME) {
        bool anyActive = false;

        for (int id = 0; id < NUM_ATTACKERS; ++id) {
            AtackRocket* atk = attackers[id];
            if (atk->isActive()) {
                anyActive = true;
                trajectoryFile << time << ",attacker," << id << ","
                               << atk->getX() << "," << atk->getY() << "," << atk->getZ() << "\n";
                atk->update(dt);
            }
        }

        for (int i = 0; i < NUM_CELLS; ++i) {
            DefenderRocket* def=Map->getCellNumber(i)->getDefenderRocket();
            if (def->isActive()) {
                anyActive = true;
                trajectoryFile << time << ",defender," << i << ","
                               << def->getX() << "," << def->getY() << "," << def->getZ() << "\n";
                def->update(dt, defenderTargets[i]);
            }
        }

        if (!anyActive) break;
        time += dt;
    }

    trajectoryFile.close();
    int intercepted = 0;
    for (auto* atk : attackers) if (atk->isDestroyed()) ++intercepted;


    for (auto* atk : attackers) delete atk;
    delete Map;
    return 0;
}