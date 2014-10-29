#include "material.hpp"

Material::~Material()
{
}

PhongMaterial::PhongMaterial(const Colour& kd, const Colour& ks, double shininess)
  : m_kd(kd), m_ks(ks), m_shininess(shininess)
{
}

PhongMaterial::~PhongMaterial()
{
}

void PhongMaterial::apply_gl() const
{
  // Perform OpenGL calls necessary to set up this material.
}

Colour Material::getKS(){
  Colour b = Colour(0.0, 0.0, 0.0);
  return b;
}

Colour Material::getKD(){
  Colour b = Colour(0.0, 0.0, 0.0);
  return b;
}


double Material::getShininess(){
  return 0.0;
}
