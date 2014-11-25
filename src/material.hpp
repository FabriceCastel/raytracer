#ifndef CS488_MATERIAL_HPP
#define CS488_MATERIAL_HPP

#include "algebra.hpp"

class Material {
public:
  virtual ~Material();
  virtual void apply_gl() const = 0;
  virtual Colour getKS();
  virtual Colour getKD();
  virtual double getShininess();

  virtual void setKD(Colour col);

protected:
  Material()
  {
  }
};

class PhongMaterial : public Material {
public:
  PhongMaterial(const Colour& kd, const Colour& ks, double shininess);
  virtual ~PhongMaterial();

  virtual void apply_gl() const;
  virtual Colour getKS(){ return m_ks; };
  virtual Colour getKD(){ return m_kd; };
  virtual double getShininess(){ return m_shininess; };
  virtual void setKD(Colour col){m_kd = col;};

private:
  Colour m_kd;
  Colour m_ks;

  double m_shininess;
};


#endif
