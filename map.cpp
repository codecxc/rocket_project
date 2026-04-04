#include "map.h"

Corner::Corner(float x, float y, float z):x(x),y(y),z(z) {}
Corner::Corner():x(0.0f),y(0.0f),z(0.0f) {}
float Corner::getX() const {
	return x;
}
float Corner::getY() const {
        return y;
}
float Corner::getZ() const {
        return z;
}
void Corner::setX(float x) {
        this->x=x;
}
void Corner::setY(float y) {
        this->y=y;
}
void Corner::setZ(float z) {
        this->z=z;
}

Cell::Cell(Corner* c1,Corner* c2,Corner* c3,Corner* c4,DefenderRocket* r=nullptr):c1(c1),c2(c2),c3(c3),c4(c4),r(r) {
	if(r!=nullptr) this->is_have_defender_rocket=true;
	else this->is_have_defender_rocket=false;
}
Cell::Cell():is_have_defender_rocket(false),r(nullptr),c1(nullptr),c2(nullptr),c3(nullptr),c4(nullptr) {}
Cell::~Cell() {
	delete r;
	delete c1;
	delete c2;
	delete c3;
	delete c4;
}
std::vector<Corner*> Cell::getCorners() const {
	std::vector<Corners*> v;
	v.push_back(c1);
	v.push_back(c2);
	v.push_back(c3);
	v.push_back(c4);
	return v;

DefenderRocket* Cell::getDefenderRocket() const {
	return r;
}

Corner* Cell::getCenter() const {
	Corner* c=new Corner((c1->x+c2->x+c3->x+c4->x)/4.0,(c1->y+c2->y+c3->y+c4->y)/4.0,(c1->z+c2->z+c3->z+c4->z)/4.0));
	return c;
}

Map::Map(Corner* c_map1,Corner* c_map2,Corner* c_map3,Corner* c_map4, std::vector<Cell*> map):c_map1(c_map1),c_map2(c_map2),c_map3(c_map3),c_map4(c_map4),map(map) {}
Corner* Map::getCornerNumber(int num) const {
	return map[num];

}
Map::~Map() {
	delete c_map2;
	delete c_map1;
	delete c_map3;
	delete c_map4;
	for(Cell* s:map) {delete s;}
}
