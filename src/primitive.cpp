#include "primitive.hpp"
#include "polyroots.hpp"
#include <float.h>
#include <vector>

using namespace std;

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

void Primitive::tick(MasterTempo* mt){

}

NonhierTangleCube::~NonhierTangleCube(){}

Intersection* Primitive::getIntersection(Ray ray, Matrix4x4 trans){
  return NULL;
}

Intersection* NonhierBox::getIntersection(Ray ray, Matrix4x4 trans){
  //ray.point = trans*ray.point;//trans.invert() * ray.point;
  //ray.vector = trans*ray.vector;//trans.invert() * ray.vector;


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
  double u,v;

  for(int p = 0; p < 6; p++){
    hitTest = intersectPlane(ray, points[p], normals[p]);

    // test to see if the point is in the cube face portion of the plane
    int axis = p%3; // (0, 1, 2) = (x, y, z)
    Point3D inter = ray.point + hitTest*ray.vector;
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

      // use the current intersection point 'inter'..
      u = 0.5;
      v = 0.5;

      
      double z = Vector3D(0,0,1).dot(normal);
      double y = Vector3D(0,1,0).dot(normal);

      if(z > epsilon || z < -1*epsilon){
        // if on x-y plane:
        u = (inter[0] - bcorner[0]) / m_size;
        v = (inter[1] - bcorner[1]) / m_size;
      } else if(y > epsilon || y < -1*epsilon){
        // if on x-z plane:
        u = (inter[0] - bcorner[0]) / m_size;
        v = (inter[2] - bcorner[2]) / m_size;
      } else {
        // if on y-z plane:
        u = (inter[2] - bcorner[2]) / m_size;
        v = (inter[1] - bcorner[1]) / m_size;
      }
      v = 1 - v;

    }
  }
  
  if(t < 0.0)
    return NULL;

  Point3D hit = ray.point + t*ray.vector;

  Intersection *ans = (Intersection*)malloc(sizeof(Intersection));
  *ans = Intersection(hit, normal, NULL);
  ans->setTextureUV(u, v);
  return ans;
}

double heightField(double lat, double lng){
  // lat [0, PI]
  // lng [0, 2PI)
  return sin(lng*lat*3) * 30;
}

Vector3D getNormalAt(double lat, double lng, double r){
  double epsilon = 0.0001;

  //if(lat < epsilon && lng < epsilon) return Vector3D(0, 0, 1);

  // Input point in world coordinates
  // going from polar to world coordinates:
  // x = r * cos(lat)sin(lng)
  // y = r * sin(lat)sin(lng)
  // z = r * cos(lng)
  double h1 = 0;//heightField(lat, lng);
  double x1 = (r+h1)*cos(lat)*sin(lng);
  double y1 = (r+h1)*sin(lat)*sin(lng);
  double z1 = (r+h1)*cos(lng);
  Point3D p1 = Point3D(x1,y1,z1);
  Vector3D p1normal = p1 - Point3D(0,0,0);
  p1normal.normalize();

  // We now have a point and a normal defining the plane tangent to the sphere at p1
  // So we need two points on that plane to then project onto the sphere and use
  // to find the normal (EASY, right? >__>)
  Point3D p2, p3; // those will be the two points on the plane

  Vector3D normal = p1normal;


  // the normal can't point back into the sphere, must mean it's inverted
  if(p1normal.dot(normal) < 0) normal = -1*normal;
  return normal;
  // double lat2 = lat + epsilon;
  // double lng2 = lng;// + epsilon;
  // double lat3 = lat;// - epsilon;
  // double lng3 = lng + epsilon;

  
  // double h2 = heightField(lat2, lng2);
  // double h3 = heightField(lat3, lng3);

  

  // double x2 = (r+h2)*cos(lat2)*sin(lng2);
  // double y2 = (r+h2)*sin(lat2)*sin(lng2);
  // double z2 = (r+h2)*cos(lng2);
  // Point3D p2 = Point3D(x2,y2,z2);

  // double x3 = (r+h3)*cos(lat3)*sin(lng3);
  // double y3 = (r+h3)*sin(lat3)*sin(lng3);
  // double z3 = (r+h3)*cos(lng3);
  // Point3D p3 = Point3D(x3,y3,z3);

  // Vector3D n = (p2 - p1).cross(p3 - p1);
  // n.normalize();
  // n = -1*n;

  // if(n.normalize() == 0){
  //   cout << "\nERROR\nNormal at: (" << lat/M_PI << "xPI" << ", " << lng/M_PI << "xPI" << ")\n";
  //   cout << "Polar coordinates: \n" << lat2 << " - " << lng2 << "\n" << lat3 << " - " << lng3 << "\n";
  //   cout << "Points\n" << p1 << "\n" << p2 << "\n" << p3 << "\n";
  //   cout << "Normal: " << n << "\n";
  //   exit(1);
  // }

  // return n;
}

Intersection* intersectHeightMap(Ray ray, Matrix4x4 trans, Point3D m_pos, double m_radius){
  return NULL;
}

Intersection* NonhierSphere::getIntersection(Ray ray, Matrix4x4 trans){
  ray.vector.normalize(); // just in case
  Vector3D v = ray.point - m_pos;

  //return intersectHeightMap(ray.point, ray.vector, trans, m_pos, m_radius);

  if((-1 * v).dot(ray.vector) <= m_radius) return NULL; // geometry is behind the ray
 
  double a = ray.vector.dot(ray.vector);
  double b = 2*(ray.vector.dot(v));
  double c = v.dot(v) - (m_radius * m_radius);


  //if(ray.vector.dot(v)*ray.vector.dot(v) - v.dot(v) + m_radius*m_radius < 0) return NULL;

  double roots[2] = {0.0, 0.0};
  int rootCount = quadraticRoots(a, b, c, roots);

  //std::cout << "\nRoots: " << rootCount;

  if(rootCount == 0) return NULL;

  Intersection *ans = (Intersection*)malloc(sizeof(Intersection));
  *ans = Intersection(Point3D(0.0, 0.0, 0.0), Vector3D(0.0, 0.0, 0.0), NULL);
  

  if(rootCount == 1 && roots[0] > 0){
    Point3D p = ray.point + roots[0]*ray.vector;
    ans->setPoint(p);
    ans->setNormal(p - m_pos);
    return ans;
  }

  Point3D nray = Point3D(-ray.point[0], -ray.point[1], -ray.point[2]);
  Point3D ap = nray - roots[0]*ray.vector;
  Point3D bp = nray - roots[1]*ray.vector;
  ap = Point3D(-1*ap[0], -1*ap[1], -1*ap[2]);
  bp = Point3D(-1*bp[0], -1*bp[1], -1*bp[2]);

  double epsilon = 0.0001;
  if(roots[0] < epsilon && roots[1] < epsilon) return NULL;

  //std::cout << "\n0: " << roots[0] << "\n1: " << roots[1];

  if((roots[0] < roots[1] && roots[0] > 0) || roots[1] < 0.0001){
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

  //if(false) return ans; // IF THE OBJECT IS NOT TRANSPARENT

  // ans->setRefraction(true);
  // Vector3D rf = refraction(1.6, ans->getNormal(), ray.point, ans->getPoint());
  // ans->setRefAngle(rf);

  //Intersection* a2 = intersectHeightMap(ray.point, ray.vector, trans, m_pos, m_radius);

  //if(a2 != NULL) cout << "\nPOINT  - Expected: " << ans->getPoint() << "\nFucked up value: " << a2->getPoint() << "\n";
  //if(a2 != NULL) cout << "\nNORMAL - Expected: " << ans->getNormal() << "\nFucked up value: " << a2->getNormal() << "\n";

  //std::cout << "\nOrigin: " << ray.point << "\nNormal: " << ans->getNormal() << "\nIntersection point: " << ans->getPoint() << "\nRefraction Angle: " << rf;

  return ans;
}


Intersection* NonhierTangleCube::getIntersection(Ray ray, Matrix4x4 trans){
  ray.vector.normalize();

  //double scale = 1.0 / m_radius;  

  double roots[4];
  double k = std::pow(ray.vector[0], 4) +
             std::pow(ray.vector[1], 4) +
             std::pow(ray.vector[2], 4);

  k *= 1 + ((shapeDistortion-0.1) * 0.00008);

  double a = 0;
  double b = 0;
  double c = 0;
  double d = 0;
  for(int i = 0; i < 3; i++){
    a += 4 * pow(ray.vector[i], 3) * ray.point[i];
    b += 6 * pow(ray.vector[i], 2) * pow(ray.point[i], 2) - 5*pow(ray.vector[i], 2);
    c += 4 * ray.vector[i] * pow(ray.point[i], 3) - 10*ray.point[i]*ray.vector[i];
    d += pow(ray.point[i], 4) - 5*pow(ray.point[i], 2);
  }
  d += 10;//11.8;

  a /= k; b /= k; c /= k; d /= k;

  int rootCount = quarticRoots(a, b, c, d, roots);
  if(rootCount == 0) return NULL;


  double t = roots[0];
  for(int i = 1; i < rootCount; i++){
    if(roots[i] > 0 && roots[i] < t){
      t = roots[i];
    }
  }

  if(t <= 0) return NULL;

  Point3D sPoint = ray.point + t*ray.vector;
  Vector3D sNormal = Vector3D(0,0,0);

  for(int i = 0; i < 3; i++){
    sNormal[i] = 4*pow(sPoint[i], 3) - 10*sPoint[i];
  }

  if(sNormal.dot(ray.vector) > 0) sNormal = -1 * sNormal;
  sNormal.normalize();

  Intersection *ans = (Intersection*)malloc(sizeof(Intersection));
  *ans = Intersection(sPoint, sNormal, NULL);

  ans->setRefraction(true);
  Vector3D rf = refraction(1.6, ans->getNormal(), ray.point, ans->getPoint());
  ans->setRefAngle(rf);

  return ans;
}

void NonhierTangleCube::tick(MasterTempo* mt){

  if(mt->getNoteStatus(3)){

    if(sdVel == 0){
      sdVel = 0.1*(1.0 / RAND_MAX)*(std::rand() - 0.5*RAND_MAX);// * 0.01
      while(abs(sdVel) < 0.03) sdVel *= 2;
      //cout << "\n\n" << sdVel << "\n\n";
      if(shapeDistortion > 0.7 && sdVel > 0) sdVel *= -0.8;
      //if(shapeDistortion < -0.55 && sdVel < 0) sdVel *= -0.8;
      else if(shapeDistortion < 0.4 && sdVel < 0) sdVel *= -0.2;
    }

    shapeDistortion += sdVel;
    
  } else {
    // if(shapeDistortion > 0.8 && sdVel > 0){
    //   sdVel /= 2;
    //   shapeDistortion += sdVel;
    //   //if(shapeDistortion > 0.8) shapeDistortion = 0.8;
    // }
    if(abs(sdVel) < 0.001)
      sdVel = 0;
    else{
      shapeDistortion += sdVel;
      sdVel /= 2.4;
    }
  }
  //cout << "\nEND OF TICK: sdVel = " << sdVel << " - shapeDistortion = " << shapeDistortion << "\n";
}
