#include "mesh.hpp"
#include <iostream>
#include <vector>

Mesh::Mesh(const std::vector<Point3D>& verts,
           const std::vector< std::vector<int> >& faces)
  : m_verts(verts),
    m_faces(faces)
{
  double tx, ty, tz, mx, my, mz;
  tx = verts.at(0)[0];
  mx = tx;
  ty = verts.at(0)[1];
  my = ty;
  tz = verts.at(0)[2];
  mz = tz;
  for(std::vector<Point3D>::const_iterator I = verts.begin();
      I != verts.end(); ++I){
    double x = (*I)[0];
    double y = (*I)[1];
    double z = (*I)[2];
    if(x < mx) mx = x;
    if(x > tx) tx = x;
    if(y < my) my = y;
    if(y > ty) ty = y;
    if(z < mz) mz = z;
    if(z > tz) tz = z;
  }
  Point3D t = Point3D(tx, ty, tz);
  Point3D m = Point3D(mx, my, mz);
  double radius = (t - m).length() / 2;
  Point3D pos = Point3D((tx+mx/2), (ty+my)/2, (tz+mz)/2);
  boundingSphere = new NonhierSphere(pos, radius);
}

std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  std::cerr << "mesh({";
  for (std::vector<Point3D>::const_iterator I = mesh.m_verts.begin(); I != mesh.m_verts.end(); ++I) {
    if (I != mesh.m_verts.begin()) std::cerr << ",\n      ";
    std::cerr << *I;
  }
  std::cerr << "},\n\n     {";
  
  for (std::vector<Mesh::Face>::const_iterator I = mesh.m_faces.begin(); I != mesh.m_faces.end(); ++I) {
    if (I != mesh.m_faces.begin()) std::cerr << ",\n      ";
    std::cerr << "[";
    for (Mesh::Face::const_iterator J = I->begin(); J != I->end(); ++J) {
      if (J != I->begin()) std::cerr << ", ";
      std::cerr << *J;
    }
    std::cerr << "]";
  }
  std::cerr << "});" << std::endl;
  return out;
}


Intersection* Mesh::getIntersection(Ray ray, Matrix4x4 trans){
  // vector<Face> (face = typedef vector<int> m_faces
  // vector<Point3D> m_verts

  double t = -1;
  double hitTest;
  Vector3D normal;
  Point3D point;

  if(boundingSphere->getIntersection(ray, trans) == NULL)
   return NULL;

  for(std::vector<Face>::const_iterator F = m_faces.begin();
      F != m_faces.end(); ++F){
    std::vector<int> points = (*F);
    std::vector<Point3D> vertices = std::vector<Point3D>();
    for(int i = 0; i < 3; i++){
      vertices.push_back(m_verts.at(points.at(i)));
    }

    Vector3D tnorm = (vertices.at(1) - vertices.at(0)).
      cross(vertices.at(2) - vertices.at(0));
    double area = tnorm.length();
    tnorm.normalize(); // not sure if necessary
    hitTest = intersectPlane(ray, vertices.at(0), tnorm);
    Point3D hit = ray.point + hitTest*ray.vector;

    bool inTri = true;
    double areaSum = 0.0;
    for(int i = 0; i < 3; i++){
      Vector3D cross = (vertices.at((i+1)%3) - hit).cross(vertices.at(i%3) - hit);
      areaSum += cross.length();
    }

    if(std::abs(areaSum - area) > 0.0001) inTri = false;

    if(inTri && hitTest > 0 && (t < 0 || t > hitTest)){
      t = hitTest;
      point = hit;
      normal = tnorm;
    }
  }

  if(t < 0)
    return NULL;

  Intersection *ans = (Intersection*)malloc(sizeof(Intersection));
  *ans = Intersection(point, normal, NULL);
  return ans;
}
