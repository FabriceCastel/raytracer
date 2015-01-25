#ifndef SHADER_HPP
#define SHADER_HPP

#include "algebra.hpp"
#include "light.hpp"
#include "primitive.hpp"
#include "scene.hpp"
#include <list>


Vector3D shade(Vector3D fc, std::list<Light*> lights, Colour ambient, Intersection col, Point3D eye, SceneNode* root, MasterTempo* mt);


#endif