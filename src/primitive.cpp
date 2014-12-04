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

Intersection* Primitive::getIntersection(Point3D rayP, Vector3D rayV, Matrix4x4 trans){
  return NULL;
}

Intersection* NonhierBox::getIntersection(Point3D rayP, Vector3D rayV, Matrix4x4 trans){
  //rayP = trans*rayP;//trans.invert() * rayP;
  //rayV = trans*rayV;//trans.invert() * rayV;


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

  Point3D hit = rayP + t*rayV;

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

Intersection* intersectHeightMap(Point3D rayP, Vector3D rayV, Matrix4x4 trans, Point3D m_pos, double m_radius){
  // Translate everything so that the sphere is centred on the origin
  Point3D eye = rayP + (Point3D(0.0, 0.0, 0.0) - m_pos);
  Point3D sphereCentre = Point3D(0.0, 0.0, 0.0);
  Vector3D ray = rayV;
  ray.normalize();

  double epsilon = 0.3;
  int maxStepsBeforeBailout = 30;
  double intermediateRayResolution = 0.1;//0.2;
  double distanceBetweenEyeAndSphereCentre = abs((eye - sphereCentre).normalize());

  Point3D CoPs; // the point closest to the eye on the underlying (sphere) surface
  Vector3D centreToSurface = eye - sphereCentre;
  centreToSurface.normalize();
  CoPs = sphereCentre + m_radius*centreToSurface;

  // Get the distance between CoPs and its "sibling" on the eye ray
  // This isn't too too important, it's to figure out how many intermediate rays
  // we're going to use (bigger distance = more rays) to keep a consistant
  // degree of precision
  Vector3D eyeToCoPs = CoPs - eye;
  double dEyeToCoPs = eyeToCoPs.normalize();
  Point3D sibling = eye + (dEyeToCoPs * ray);
  double CoPsToSibling = abs((CoPs - sibling).normalize());

  vector<Vector3D> intermediateRays = vector<Vector3D>();
  
  int numberOfIntermediateRays = (int)(CoPsToSibling * intermediateRayResolution);
  if(numberOfIntermediateRays < 2) numberOfIntermediateRays = 2;

  for(int inter = 0; inter <= numberOfIntermediateRays; inter++){
    double copW = ((float)inter) / ((float)numberOfIntermediateRays);
    double rayW = 1.0 - copW;
    double x = ray[0]*rayW + eyeToCoPs[0]*copW;
    double y = ray[1]*rayW + eyeToCoPs[1]*copW;
    double z = ray[2]*rayW + eyeToCoPs[2]*copW;
    Vector3D nextRay = Vector3D(x, y, z);
    nextRay.normalize();
    intermediateRays.push_back(nextRay);
  }
  intermediateRays.push_back(eyeToCoPs);


  Vector3D currentRay = intermediateRays.back();
  double CoPsLat = xyToLat(CoPs[0], CoPs[1]);
  double CoPsLng = zrToLng(CoPs[2], m_radius);
  double CoPsH = heightField(CoPsLat, CoPsLng);
  Vector3D centreToCoPs = CoPs - sphereCentre;
  centreToCoPs.normalize();
  Point3D soln = sphereCentre + (m_radius+CoPsH)*centreToCoPs;

  intermediateRays.pop_back();
  while(!intermediateRays.empty()){
    currentRay = intermediateRays.back();

    double distFromRay = epsilon + 1; // bogus value to make the following while loop run
    int refineIterations = 0;

    // now keep refining the soln until you're within epsilon of the ray or you bail out
    while(distFromRay > epsilon){


      // using prev soln, pick a soln projection point on the current ray (closest point to prev soln)
      Vector3D pma = soln - eye;
      double t = currentRay.dot(pma);//pma.dot(currentRay);
      Point3D solnProj = eye + t*currentRay;


      // figure out where the point on the ray projects to on the underlying surface to get its height
      Vector3D c2s = solnProj - sphereCentre;
      c2s.normalize();
      soln = sphereCentre + m_radius*c2s;


      double projLat = xyToLat(soln[0], soln[1]);
      double projLng = zrToLng(soln[2], m_radius);
      soln = sphereCentre + (m_radius+heightField(projLat, projLng))*c2s;


      distFromRay = abs((soln - solnProj).normalize());
      
      if(refineIterations > maxStepsBeforeBailout){
        return NULL; // no intersection
      }
      refineIterations++;
    }


    intermediateRays.pop_back();
  }

  //exit(1);

  // Adjust soln; translate it back to its corresct location in the scene
  Vector3D nSol = getNormalAt(xyToLat(soln[0], soln[1]), zrToLng(soln[2], m_radius), m_radius);
  

  //cout << soln << "\n";
  //exit(1);

  Vector3D solnN = soln - Point3D(0,0,0);
  solnN.normalize();

  soln = soln + (m_pos - Point3D(0,0,0));

  Intersection *ans = (Intersection*)malloc(sizeof(Intersection));
  *ans = Intersection(soln, nSol, NULL);

  return ans;
}

Intersection* NonhierSphere::getIntersection(Point3D rayP, Vector3D rayV, Matrix4x4 trans){
  rayV.normalize(); // just in case
  Vector3D v = rayP - m_pos;

  //return intersectHeightMap(rayP, rayV, trans, m_pos, m_radius);

  //if((-1 * v).dot(rayV) <= m_radius) return NULL; // geometry is behind the ray
 
  double a = rayV.dot(rayV);
  double b = 2*(rayV.dot(v));
  double c = v.dot(v) - (m_radius * m_radius);


  //if(rayV.dot(v)*rayV.dot(v) - v.dot(v) + m_radius*m_radius < 0) return NULL;

  double roots[2] = {0.0, 0.0};
  int rootCount = quadraticRoots(a, b, c, roots);

  //std::cout << "\nRoots: " << rootCount;

  if(rootCount == 0) return NULL;

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
  // Vector3D rf = refraction(1.6, ans->getNormal(), rayP, ans->getPoint());
  // ans->setRefAngle(rf);

  //Intersection* a2 = intersectHeightMap(rayP, rayV, trans, m_pos, m_radius);

  //if(a2 != NULL) cout << "\nPOINT  - Expected: " << ans->getPoint() << "\nFucked up value: " << a2->getPoint() << "\n";
  //if(a2 != NULL) cout << "\nNORMAL - Expected: " << ans->getNormal() << "\nFucked up value: " << a2->getNormal() << "\n";

  //std::cout << "\nOrigin: " << rayP << "\nNormal: " << ans->getNormal() << "\nIntersection point: " << ans->getPoint() << "\nRefraction Angle: " << rf;

  return ans;
}


Intersection* NonhierTangleCube::getIntersection(Point3D rayP, Vector3D rayV, Matrix4x4 trans){
  rayV.normalize();

  //double scale = 1.0 / m_radius;  

  double roots[4];
  double k = std::pow(rayV[0], 4) +
             std::pow(rayV[1], 4) +
             std::pow(rayV[2], 4);

  k *= 1 + ((shapeDistortion-0.1) * 0.00008);

  double a = 0;
  double b = 0;
  double c = 0;
  double d = 0;
  for(int i = 0; i < 3; i++){
    a += 4 * pow(rayV[i], 3) * rayP[i];
    b += 6 * pow(rayV[i], 2) * pow(rayP[i], 2) - 5*pow(rayV[i], 2);
    c += 4 * rayV[i] * pow(rayP[i], 3) - 10*rayP[i]*rayV[i];
    d += pow(rayP[i], 4) - 5*pow(rayP[i], 2);
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

  Point3D sPoint = rayP + t*rayV;
  Vector3D sNormal = Vector3D(0,0,0);

  for(int i = 0; i < 3; i++){
    sNormal[i] = 4*pow(sPoint[i], 3) - 10*sPoint[i];
  }

  if(sNormal.dot(rayV) > 0) sNormal = -1 * sNormal;
  sNormal.normalize();

  Intersection *ans = (Intersection*)malloc(sizeof(Intersection));
  *ans = Intersection(sPoint, sNormal, NULL);

  ans->setRefraction(true);
  Vector3D rf = refraction(1.6, ans->getNormal(), rayP, ans->getPoint());
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
