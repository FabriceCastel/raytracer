#ifndef SCENE_HPP
#define SCENE_HPP

#include <list>
#include "algebra.hpp"
#include "primitive.hpp"
#include "material.hpp"
#include "mastertempo.hpp"
#include "particle.hpp"
#include "texture.hpp"


class SceneNode {
public:
  SceneNode(const std::string& name);
  virtual ~SceneNode();

  const Matrix4x4& get_transform() const { return m_trans; }
  const Matrix4x4& get_inverse() const { return m_invtrans; }
  
  void set_transform(const Matrix4x4& m)
  {
    m_trans = m;
    m_invtrans = m.invert();
  }

  void set_transform(const Matrix4x4& m, const Matrix4x4& i)
  {
    m_trans = m;
    m_invtrans = i;
  }

  void add_child(SceneNode* child)
  {
    m_children.push_back(child);
  }

  void remove_child(SceneNode* child)
  {
    m_children.remove(child);
  }

  virtual void intersect(Ray ray, Intersection &inter, Matrix4x4 trans, MasterTempo* mt);
  virtual void tick(MasterTempo* mt);

  // Callbacks to be implemented.
  // These will be called from Lua.
  void rotate(char axis, double angle);
  void scale(const Vector3D& amount);
  void translate(const Vector3D& amount);

  // Returns true if and only if this node is a JointNode
  virtual bool is_joint() const;
  
protected:
  
  // Useful for picking
  int m_id;
  std::string m_name;

  // Transformations
  Matrix4x4 m_trans;
  Matrix4x4 m_invtrans;

  // Hierarchy
  typedef std::list<SceneNode*> ChildList;
  ChildList m_children;
};

class JointNode : public SceneNode {
public:
  JointNode(const std::string& name);
  virtual ~JointNode();

  virtual bool is_joint() const;

  void set_joint_x(double min, double init, double max);
  void set_joint_y(double min, double init, double max);

  struct JointRange {
    double min, init, max;
  };

  
protected:

  JointRange m_joint_x, m_joint_y;
};

class GeometryNode : public SceneNode {
public:
  GeometryNode(const std::string& name,
               Primitive* primitive);
  virtual ~GeometryNode();

  virtual void intersect(Ray ray, Intersection &inter, Matrix4x4 trans, MasterTempo* mt);

  const Material* get_material() const;
  Material* get_material();

  void set_material(Material* material)
  {
    m_material = material;
  }

  void set_texture(Texture* texture){
    m_texture = texture;
    m_trigger = -1;
  }

  void set_texture(Texture* texture, int trigger){
    m_texture = texture;
    m_trigger = trigger;
  }

  bool hasMidiTrigger(){return (m_trigger != -1);};
  int getMidiTrigger(){return m_trigger;};
  virtual void tick(MasterTempo* mt);

protected:
  Material* m_material;
  Texture* m_texture;
  int m_trigger;
  Primitive* m_primitive;
};

class ParticleSystem : public SceneNode {
public:
  ParticleSystem(const std::string& name, Vector3D gravity, Point3D spawnZoneCorner, double spawnZoneSize, double spawnDensity);
  virtual void intersect(Ray ray, Intersection &inter, Matrix4x4 trans, MasterTempo* mt);
  virtual void tick(MasterTempo* mt);

private:
  Vector3D gravity;
  Point3D spawnZoneCorner;
  double spawnZoneSize;
  double spawnDensity;
  int currentFrame;
  std::list<Particle> particles;
};

#endif
