#ifndef CS488_A4_HPP
#define CS488_A4_HPP

#include <string>
#include "algebra.hpp"
#include "scene.hpp"
#include "light.hpp"
//#include <pthread.h>

void render(// What to render
               SceneNode* root,
               // Where to output the image
               const std::string& filename,
               // Image size
               int width, int height,
               // Viewing parameters
               const Point3D& eye, const Vector3D& view,
               const Vector3D& up, double fov,
               // Lighting parameters
               const Colour& ambient,
               const std::list<Light*>& lights
               );

void *renderNextStrip(void *params);
int getNextRow();

class StripRenderParams{
public:
     StripRenderParams(SceneNode* root, int width, int height,
               Point3D eye, Vector3D view, Vector3D up, double fov,
               Colour ambient, std::list<Light*> lights, MasterTempo* mt,
               double* rbuffer, Matrix4x4 world) : 
          root(root), width(width), height(height),
          eye(eye), view(view), up(up), fov(fov),
          ambient(ambient), lights(lights), mt(mt),
          rbuffer(rbuffer), world(world), nextRow(0) {}

     SceneNode* getRoot(){return root;};
     int getWidth(){return width;};
     int getHeight(){return height;};
     Point3D getEye(){return eye;};
     Vector3D getView(){return view;};
     Vector3D getUp(){return up;};
     double getFov(){return fov;};
     Colour getAmbient(){return ambient;};
     std::list<Light*> getLights(){return lights;};
     MasterTempo* getMt(){return mt;};
     double* getRbuffer(){return rbuffer;};
     Matrix4x4 getWorld(){return world;};

     int getNextRow(){
          mutex mtx;
          mtx.lock();
          int row = nextRow++;
          mtx.unlock();
          return row;
     }

private:
     SceneNode* root;
     int width, height;
     Point3D eye;
     Vector3D view, up;
     double fov;
     Colour ambient;
     std::list<Light*> lights;
     MasterTempo* mt;
     double* rbuffer;
     Matrix4x4 world;

     int nextRow;
};

Vector3D shade(Vector3D fc, std::list<Light*> lights, Colour ambient, Intersection* col, Point3D eye, SceneNode* root, MasterTempo* mt);


#endif
