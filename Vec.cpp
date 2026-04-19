#include <cmath>

class Vec {
	protected:
		float x,y,z;
	public:
		Vec():x(0),y(0),z(0) {}
		Vec(float x,float y,float z):x(x),y(y),z(z) {}
		float getX() const {return x;}
		float getY() const {return y;}
		float getZ() const {return z;}
		Vec operator+(const Vec& v) const {
			return Vec(x+v.getX(),y+v.getY(),z+v.getZ());
		}
		Vec operator-(const Vec& v) const {
                        return Vec(x-v.getX(),y-v.getY(),z-v.getZ());
                }
		Vec operator*(float a) const {
                        return Vec(x*a,y*a,z*a);
                }
		Vec operator/(float a) const {
                        return Vec(x/a,y/a,z/a);
                }
		float getLength() const {
			return std::sqrt(x*x+y*y+z*z);
		}
		float scalar(Vec& v) const {
			return (v.getX()*x+v.getY()*y+v.getZ()*z);
		}
		Vec cross(Vec& v) const {
			return Vec(y*v.getZ()-z*v.getY(),-x*v.getZ()+z*v.getX(),x*v.getY()-y*v.getX());
                }
		Vec normalized() const {
			float len=this->getLength();
			if(len!=0.0f) return (*this)/len;
			return *this;
		}
};
