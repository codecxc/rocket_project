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

Map* createFixedMap() {
    float baseZ = 0.0f;

    float fixedPositions[10][2] = {
        {1000.0f, 1000.0f},
        {1000.0f, 4000.0f},
        {4000.0f, 1000.0f},
        {4000.0f, 4000.0f},
        {2500.0f, 1000.0f},
        {2500.0f, 4000.0f},
        {1000.0f, 2500.0f},
        {4000.0f, 2500.0f},
        {1500.0f, 2000.0f},
        {3500.0f, 3000.0f}
    };

    Corner* c_map1 = new Corner(0.0f, 0.0f, baseZ);
    Corner* c_map2 = new Corner(5000.0f, 0.0f, baseZ);
    Corner* c_map3 = new Corner(5000.0f, 5000.0f, baseZ);
    Corner* c_map4 = new Corner(0.0f, 5000.0f, baseZ);

    std::vector<Cell*> cells;
    for (int i = 0; i < 10; ++i) {
        float cx = fixedPositions[i][0];
        float cy = fixedPositions[i][1];
        float size = 50.0f;

        Corner* c1 = new Corner(cx - size / 2, cy - size / 2, baseZ);
        Corner* c2 = new Corner(cx + size / 2, cy - size / 2, baseZ);
        Corner* c3 = new Corner(cx + size / 2, cy + size / 2, baseZ);
        Corner* c4 = new Corner(cx - size / 2, cy + size / 2, baseZ);

        Cell* cell = new Cell(c1, c2, c3, c4);

        Parametrs* p = new Parametrs(45.0f, 0.0f, 0.0f, 0.0f, 0.0f, 4000.0f, cx, cy, baseZ);
        DefenderRocket* def = new DefenderRocket(100, 1000, 5000, cell, p, 15000.0f);
        cell = new Cell(c1, c2, c3, c4, def);
        cells.push_back(cell);
    }
    return new Map(c_map1, c_map2, c_map3, c_map4, cells);
}

void printMap(Map* map, int numCells) {
    for (int i = 0; i < numCells; ++i) {
        Cell* cell = map->getCellNumber(i);
        Corner* center = cell->getCenter();
        std::cout << "Ячейка " << i << ": " << center->getX() << " " << center->getY() << " " << center->getZ() << std::endl;
        delete center;
    }
}

bool isPointInCell(float x, float y, Cell* cell) {
    std::vector<Corner*> corners = cell->getCorners();
    float minX = corners[0]->getX();
    float maxX = corners[2]->getX();
    float minY = corners[0]->getY();
    float maxY = corners[2]->getY();
    return (x >= minX && x <= maxX && y >= minY && y <= maxY);
}

AtackRocket* createAtack(RandomGenerator& rng, Map* map, float mapStartX, float mapStartY, float mapWidth, float mapHeight, int numCells, float targetX, float targetY, float targetZ) {
    float x, y, z;
    bool insideCell = true;
    z = rng.getFloat(100.0f, 1000.0f);
    while (insideCell) {
        x = mapStartX + rng.getFloat(0.0f, mapWidth);
        y = mapStartY + rng.getFloat(0.0f, mapHeight);
        insideCell = false;
        for (int i = 0; i < numCells; ++i) {
            if (isPointInCell(x, y, map->getCellNumber(i))) {
                insideCell = true;
                break;
            }
        }
    }
    return new AtackRocket(100, 1500, 250, x, y, z, targetX, targetY, targetZ, 45.0f, 30, 500, 2.5, 6000, 0.05f, 0.3f);
}

int main() {
    const int MAP_WIDTH = 5000;
    const int MAP_HEIGHT = 5000;
    const int NUM_CELLS = 10;

    RandomGenerator rng;
    float startX = rng.getFloat(-50.0f, 50.0f);
    float startY = rng.getFloat(-50.0f, 50.0f);

    Map* Map = createFixedMap();
    printMap(Map, NUM_CELLS);

    int numAttackers = 10;
    std::vector<AtackRocket*> attackers;
    attackers.reserve(numAttackers);

    for (int id = 0; id < numAttackers; ++id) {
        int targetCell = static_cast<int>(rng.getFloat(0.0f, NUM_CELLS));
        Corner* targetCenter = Map->getCellNumber(targetCell)->getCenter();
        float tx = targetCenter->getX();
        float ty = targetCenter->getY();
        float tz = targetCenter->getZ();
        delete targetCenter;

        AtackRocket* attacker = createAtack(rng, Map, startX, startY, MAP_WIDTH, MAP_HEIGHT, NUM_CELLS, tx, ty, tz);
        attackers.push_back(attacker);
        std::cout << "Атакующая ракета id=" << id
            << ": старт (" << attacker->getX() << "," << attacker->getY() << "," << attacker->getZ() << ")"
            << ", цель (" << tx << "," << ty << "," << tz << ")\n";
    }

    std::ofstream trajectoryFile("attackrockets.txt");
    trajectoryFile << "time,type,id,x,y,z\n";
    trajectoryFile << std::fixed << std::setprecision(6);

    float time = 0.0f;
    float dt = 0.01f;

    for (auto* attacker : attackers) {
        attacker->launch();
    }

    std::vector<AtackRocket*> defenderTargets(NUM_CELLS, nullptr);
    for (int i = 0; i < NUM_CELLS; ++i) {
        int targetIdx = static_cast<int>(rng.getFloat(0.0f, attackers.size()));
        AtackRocket* target = attackers[targetIdx];
        defenderTargets[i] = target;
        Map->getCellNumber(i)->getDefenderRocket()->launch(target);
    }

    bool anyActive = true;
    while (anyActive) {
        anyActive = false;
        for (int id = 0; id < attackers.size(); ++id) {
            AtackRocket* atk = attackers[id];
            if (atk->isActive()) {
                anyActive = true;
                trajectoryFile << time << ",attacker," << id << ","
                    << atk->getX() << ","
                    << atk->getY() << ","
                    << atk->getZ() << "\n";
                atk->update(dt);
            }
        }
        for (int i = 0; i < NUM_CELLS; ++i) {
            DefenderRocket* defender = Map->getCellNumber(i)->getDefenderRocket();
            if (defender->isActive()) {
                defender->update(dt, defenderTargets[i]);
                trajectoryFile << time << ",defender," << i << ","
                    << defender->getX() << ","
                    << defender->getY() << ","
                    << defender->getZ() << "\n";
            }
        }
        time += dt;
    }

    trajectoryFile.close();
    for (auto* atk : attackers) {
        delete atk;
    }
    delete Map;
    return 0;
}