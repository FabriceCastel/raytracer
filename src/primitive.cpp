#include "primitive.hpp"
#include "polyroots.hpp"
#include <float.h>

Intersection::~Intersection(){}

Primitive::~Primitive()
{
}

Sphere::~Sphere()
{
}

Cube::~Cube()
{
}

NonhierSphere::~NonhierSphere()
{
}

NonhierBox::~NonhierBox()
{
}

Intersection* Primitive::getIntersection(Point3D rayP, Vector3D rayV, Matrix4x4 trans){
  return NULL;
}

Intersection* NonhierBox::getIntersection(Point3D rayP, Vector3D rayV, Matrix4x4 trans){
  //m_pos
  //m_size
  // The box has one corner at m_pos and the opposite corner at m_pos + vec3(m_size)
  Point3D bcorner = m_pos;
  Point3D tcorner = Point3D(m_pos[0] + m_size,
			    m_pos[1] + m_size,
			    m_pos[2] + m_size);

  Vector3D normals[6] = {
    Vector3D(1, 0, 0),
    Vector3D(0, 1, 0),
    Vector3D(0, 0, 1),
    Vector3D(-1, 0, 0),
    Vector3D(0, -1, 0),
    Vector3D(0, 0, -1)};

  Point3D points[6] = {
    tcorner,
    tcorner,
    tcorner,
    bcorner,
    bcorner,
    bcorner};

  double t = -1;
  double hitTest;
  Vector3D normal;

  for(int p = 0; p < 6; p++){
    hitTest = intersectPlane(rayP, rayV, points[p], normals[p]);

    // test to see if the point is in the cube face portion of the plane
    int axis = p%3; // (0, 1, 2) = (x, y, z)
    Point3D inter = rayP + hitTest*rayV;
    bool inSquare = true;

    double epsilon = 0.00001;
    if(inter[0] < bcorner[0] - epsilon ||
       inter[0] > tcorner[0] + epsilon ||
       inter[1] < bcorner[1] - epsilon ||
       inter[1] > tcorner[1] + epsilon ||
       inter[2] < bcorner[2] - epsilon ||
       inter[2] > tcorner[2] + epsilon)
      inSquare = false;

    if(inSquare && hitTest >= 0.0 &&
       (t < 0 || hitTest < t)){
      t = hitTest;
      normal = normals[p];
    }
  }
  
  if(t < 0.0)
    return NULL;

  Point3D hit = rayP + t*rayV;

  Intersection *ans = (Intersection*)malloc(sizeof(Intersection));
  *ans = Intersection(hit, normal, NULL);
  return ans;
}

Intersection* NonhierSphere::getIntersection(Point3D rayP, Vector3D rayV, Matrix4x4 trans){
  rayV.normalize(); // just in case
  Vector3D v = rayP - m_pos;

  if((-1 * v).dot(rayV) <= 0) return NULL; // geometry is behind the ray
 
  double a = rayV.dot(rayV);
  double b = 2*(rayV.dot(v));
  double c = v.dot(v) - (m_radius * m_radius);

  if(rayV.dot(v)*rayV.dot(v) - v.dot(v) + m_radius*m_radius < 0) return NULL;

  double roots[2] = {0.0, 0.0};
  int rootCount = quadraticRoots(a, b, c, roots);

  Intersection *ans = (Intersection*)malloc(sizeof(Intersection));
  *ans = Intersection(Point3D(0.0, 0.0, 0.0), Vector3D(0.0, 0.0, 0.0), NULL);
  
  if(rootCount == 1 && roots[0] > 0){
    Point3D p = rayP + roots[0]*rayV;
    ans->setPoint(p);
    ans->setNormal(p - m_pos);
    return ans;
  }

  Point3D nrayP = Point3D(-rayP[0], -rayP[1], -rayP[2]);
  Point3D ap = nrayP - roots[0]*rayV;
  Point3D bp = nrayP - roots[1]*rayV;
  ap = Point3D(-1*ap[0], -1*ap[1], -1*ap[2]);
  bp = Point3D(-1*bp[0], -1*bp[1], -1*bp[2]);

  if(roots[0] < 0 && roots[1] < 0) return NULL;

  if(roots[0] < roots[1] && roots[0] > 0){
    ans->setPoint(ap);
    Vector3D n = ap - m_pos;
    n.normalize();
    ans->setNormal(n);
  } else {
    ans->setPoint(bp);
    Vector3D n = bp - m_pos;
    n.normalize();
    ans->setNormal(n);
  }
  return ans;
}
