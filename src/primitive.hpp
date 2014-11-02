#ifndef CS488_PRIMITIVE_HPP
#define CS488_PRIMITIVE_HPP

#include "algebra.hpp"
#include "material.hpp"


class Intersection {
public:
  Intersection(Point3D iPoint, Vector3D normal, Material* material)
    : p(iPoint), n(normal), mat(material){
      refraction = false;
    };
  virtual ~Intersection();
  Point3D getPoint(){ return p; };
  Vector3D getNormal() { return n; };
  Material* getMaterial() { return mat; };
  bool isRefraction(){return refraction;};
  Vector3D getRefAngle(){return refAngle;};

  void setPoint(Point3D point){ p = point; };
  void setNormal(Vector3D normal){ n = normal; };
  void setMaterial(Material* material){ mat = material; };
  void setRefraction(bool ref){refraction = ref;};
  void setRefAngle(Vector3D r){refAngle = r;};

private:
  Point3D p;
  Vector3D n, refAngle;
  Material* mat;
  bool refraction;
};


class Primitive {
public:
  virtual ~Primitive();
  virtual Intersection* getIntersection(Point3D rayP, Vector3D rayV, Matrix4x4 trans);
};

class Sphere : public Primitive {
public:
  virtual ~Sphere();
};

class Cube : public Primitive {
public:
  virtual ~Cube();
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const Point3D& pos, double radius)
    : m_pos(pos), m_radius(radius)
  {
  }
  virtual ~NonhierSphere();

  virtual Intersection* getIntersection(Point3D rayP, Vector3D rayV, Matrix4x4 trans);

private:
  Point3D m_pos;
  double m_radius;
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const Point3D& pos, double size)
    : m_pos(pos), m_size(size)
  {
  }
  
  virtual ~NonhierBox();

  virtual Intersection* getIntersection(Point3D rayP, Vector3D rayV, Matrix4x4 trans);

private:
  Point3D m_pos;
  double m_size;
};

#endif
