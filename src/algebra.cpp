//---------------------------------------------------------------------------
//
// CS488 -- Introduction to Computer Graphics
//
// algebra.hpp/algebra.cpp
//
// Classes and functions for manipulating points, vectors, matrices, 
// and colours.  You probably won't need to modify anything in these
// two files.
//
// University of Waterloo Computer Graphics Lab / 2003
//
//---------------------------------------------------------------------------

#include "algebra.hpp"
#include <math.h>

Matrix4x4 Matrix4x4::rotateX(double angle){
  double ang = M_PI * angle/180.0;
  Matrix4x4 rotation(Vector4D(1,0,0,0),
                     Vector4D(0,cos(-ang),-sin(-ang),0),
                     Vector4D(0,sin(ang),cos(-ang),0),
                     Vector4D(0,0,0,1));
  return rotation*(*this);
}

double clamp(double val, double lowerBound, double upperBound){
  if(val > upperBound) val = upperBound;
  if(val < lowerBound) val = lowerBound;
  return val;
}

double smoothstep(double low, double high, double x){
  double mx = clamp((x-low)/(high-low), 0.0, 1.0);
  return mx*mx*(3-2*mx);
}

double xyToLat(double x, double y){
  double lat = atan2(y, x);
  if(isnan(lat)){
    std::cerr << "\nERROR: Not a number: atan2(" << x << ", " << y << ")\n";
    exit(1);
  }
  return lat;
}

double zrToLng(double z, double r){
  if(z > r){
    z = r; // fix numerical issues
  }
  double lng = std::acos(z/r);
  if(isnan(lng)){
    std::cerr << "\nERROR: Not a number: acos(" << z << ", " << r << ")\n";
    exit(1);
  }
  return lng;
}

Vector3D refraction(double refIdx, Vector3D normal, Point3D rayOrigin, Point3D point){
  Vector3D ray = point - rayOrigin;
  ray.normalize();
  double refx = 1.0;
  if(normal.dot(ray) < 0){ // the ray is going INTO the object
    refx = 1.0 / refIdx;
  } else {
    refx = refIdx;
    normal = -1 * normal;
  }
  double rootC = sqrt(1 - refx * refx * (1 - (normal.dot(-1 * ray))));
  Vector3D transmissive = (refx * (normal.dot(-1 * ray)) - rootC) * normal - (refx * -1 * ray);
  return transmissive;
}

double intersectPlane(Ray ray, Point3D planeP, Vector3D planeN){
  if(std::abs(ray.vector.dot(planeN)) < 0.000001)
    return 0.0; // the plane and ray are parallel
  return (planeP - ray.point).dot(planeN) / ray.vector.dot(planeN);
}

double Vector3D::normalize()
{
  double denom = 1.0;
  double x = (v_[0] > 0.0) ? v_[0] : -v_[0];
  double y = (v_[1] > 0.0) ? v_[1] : -v_[1];
  double z = (v_[2] > 0.0) ? v_[2] : -v_[2];

  if(x > y) {
    if(x > z) {
      if(1.0 + x > 1.0) {
        y = y / x;
        z = z / x;
        denom = 1.0 / (x * sqrt(1.0 + y*y + z*z));
      }
    } else { /* z > x > y */ 
      if(1.0 + z > 1.0) {
        y = y / z;
        x = x / z;
        denom = 1.0 / (z * sqrt(1.0 + y*y + x*x));
      }
    }
  } else {
    if(y > z) {
      if(1.0 + y > 1.0) {
        z = z / y;
        x = x / y;
        denom = 1.0 / (y * sqrt(1.0 + z*z + x*x));
      }
    } else { /* x < y < z */
      if(1.0 + z > 1.0) {
        y = y / z;
        x = x / z;
        denom = 1.0 / (z * sqrt(1.0 + y*y + x*x));
      }
    }
  }

  if(1.0 + x + y + z > 1.0) {
    v_[0] *= denom;
    v_[1] *= denom;
    v_[2] *= denom;
    return 1.0 / denom;
  }

  return 0.0;
}

/*
 * Define some helper functions for matrix inversion.
 */

static void swaprows(Matrix4x4& a, size_t r1, size_t r2)
{
  std::swap(a[r1][0], a[r2][0]);
  std::swap(a[r1][1], a[r2][1]);
  std::swap(a[r1][2], a[r2][2]);
  std::swap(a[r1][3], a[r2][3]);
}

static void dividerow(Matrix4x4& a, size_t r, double fac)
{
  a[r][0] /= fac;
  a[r][1] /= fac;
  a[r][2] /= fac;
  a[r][3] /= fac;
}

static void submultrow(Matrix4x4& a, size_t dest, size_t src, double fac)
{
  a[dest][0] -= fac * a[src][0];
  a[dest][1] -= fac * a[src][1];
  a[dest][2] -= fac * a[src][2];
  a[dest][3] -= fac * a[src][3];
}

/*
 * invertMatrix
 *
 * I lifted this code from the skeleton code of a raytracer assignment
 * from a different school.  I taught that course too, so I figured it
 * would be okay.
 */
Matrix4x4 Matrix4x4::invert() const
{
  /* The algorithm is plain old Gauss-Jordan elimination 
     with partial pivoting. */

  Matrix4x4 a(*this);
  Matrix4x4 ret;

  /* Loop over cols of a from left to right, 
     eliminating above and below diag */

  /* Find largest pivot in column j among rows j..3 */
  for(size_t j = 0; j < 4; ++j) { 
    size_t i1 = j; /* Row with largest pivot candidate */
    for(size_t i = j + 1; i < 4; ++i) {
      if(fabs(a[i][j]) > fabs(a[i1][j])) {
        i1 = i;
      }
    }

    /* Swap rows i1 and j in a and ret to put pivot on diagonal */
    swaprows(a, i1, j);
    swaprows(ret, i1, j);

    /* Scale row j to have a unit diagonal */
    if(a[j][j] == 0.0) {
      // Theoretically throw an exception.
      return ret;
    }

    dividerow(ret, j, a[j][j]);
    dividerow(a, j, a[j][j]);

    /* Eliminate off-diagonal elems in col j of a, doing identical 
       ops to b */
    for(size_t i = 0; i < 4; ++i) {
      if(i != j) {
        submultrow(ret, i, j, a[i][j]);
        submultrow(a, i, j, a[i][j]);
      }
    }
  }

  return ret;
}
