#ifndef CS488_A4_HPP
#define CS488_A4_HPP

#include <string>
#include "algebra.hpp"
#include "scene.hpp"
#include "light.hpp"

void a4_render(// What to render
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


Vector3D shade(Vector3D fc, std::list<Light*> lights, Intersection* col, Point3D eye, SceneNode* root, MasterTempo* mt);

void applySinCityFilter(double* rbuf, int height, int width, double filterWeight);

#endif
