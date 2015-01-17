#ifndef CS488_PRIMITIVE_HPP
#define CS488_PRIMITIVE_HPP

#include "algebra.hpp"
#include "material.hpp"
#include "mastertempo.hpp"

class Intersection {
public:
  Intersection(){
    initialized = false;
  }
  Intersection(Point3D iPoint, Vector3D normal, Material* material)
    : p(iPoint), n(normal), mat(material){
      refraction = false;
      ht = false;
      texture = Vector3D(0,0,0);
      u = -1;
      v = -1;
      initialized = true;
    };
  virtual ~Intersection();
  Point3D getPoint(){ return p; };
  Vector3D getNormal() { return n; };
  Material* getMaterial() { return mat; };
  bool isRefraction(){return refraction;};
  Vector3D getRefAngle(){return refAngle;};
  
  void setPoint(Point3D point){ 
    p = point;
    initialized = true;
  };
  void setNormal(Vector3D normal){
    n = normal;
    initialized = true;
  };
  void setMaterial(Material* material){ mat = material; };
  void setRefraction(bool ref){refraction = ref;};
  void setRefAngle(Vector3D r){refAngle = r;};
  void setTexture(Vector3D c){texture = c; ht = true;};
  void setTextureUV(double uc, double vc){u = uc; v = vc;};
  bool hasTexture(){return ht;};
  Vector3D getTexture(){return texture;};
  double getU(){return u;};
  double getV(){return v;};
  void clearTexture(){ht = false;};
  bool isValid(){return initialized;};

private:
  bool initialized = false;
  Point3D p;
  Vector3D n, refAngle;
  Material* mat;
  bool refraction;
  bool ht;
  Vector3D texture;
  double u, v;
};


class Primitive {
public:
  virtual ~Primitive();
  virtual void getIntersection(Ray ray, Intersection &inter, Matrix4x4 trans);
  virtual void tick(MasterTempo* mt);
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

  virtual void getIntersection(Ray ray, Intersection &inter, Matrix4x4 trans);

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

  virtual void getIntersection(Ray ray, Intersection &inter, Matrix4x4 trans);

private:
  Point3D m_pos;
  double m_size;
};

class NonhierTangleCube : public Primitive {
public:
  NonhierTangleCube(const Point3D& pos, double radius) :
    m_pos(pos), m_radius(radius), shapeDistortion(0.3), sdVel(0){}

  virtual ~NonhierTangleCube();

  virtual void getIntersection(Ray ray, Intersection &inter, Matrix4x4 trans);
  virtual void tick(MasterTempo* mt);

private:
  Point3D m_pos;
  double m_radius;
  double shapeDistortion;
  double sdVel;
};

#endif
