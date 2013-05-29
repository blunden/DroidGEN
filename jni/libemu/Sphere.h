
#ifndef SPHERE_H_
#define SPHERE_H_

class Sphere
{
public:
     Sphere() :
          _x(0), _y(0), _z(0), _radius(0)
     {

     }


     Sphere(float x, float y, float z, float r) :
          _x(x), _y(y), _z(z), _radius(r)
     {

     }

     ~Sphere()
     {

     }

     float getX() const { return _x; }
     float getY() const { return _y; }
     float getZ() const { return _z; }
     float getRadius() const { return _radius; }

     void setX(float x) { _x = x; }
     void setY(float y) { _y = y; }
     void setZ(float z) { _z = z; }
     void setRadius(float r) { _radius = r; }

     bool intersects(const Sphere* s)
     {
          float squaredDistance =      ((this->_x - s->getX()) * (this->_x - s->getX())) +
                                       ((this->_y - s->getY()) * (this->_y - s->getY())) +
                                       ((this->_z - s->getZ()) * (this->_z - s->getZ()));
          if (squaredDistance <= (this->_radius + s->getRadius()) * (this->_radius + s->getRadius()))
          {
               return true;
          }

          return false;
     }

     bool contains(float x, float y, float z)
     {
          float squaredDistance =      ((this->_x - x) * (this->_x - x)) +
                                       ((this->_y - y) * (this->_y - y)) +
                                       ((this->_z - z) * (this->_z - z));
          if (squaredDistance < (this->_radius) * (this->_radius))
          {
               return true;
          }

          return false;
     }

private:
     float _x;
     float _y;
     float _z;
     float _radius;
};

#endif /* SPHERE_H_ */
