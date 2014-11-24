#include "scene.hpp"
#include "polyroots.hpp"
#include <iostream>

SceneNode::SceneNode(const std::string& name)
  : m_name(name)
{
}

SceneNode::~SceneNode()
{
}

Intersection* SceneNode::intersect(Point3D rayP, Vector3D rayV, Matrix4x4 trans, MasterTempo* mt){
  int c = 0;
  Intersection* col = NULL;
  Intersection* tcol = NULL;
  for (ChildList::const_iterator iterator = m_children.begin(), end = m_children.end(); iterator != end; ++iterator) {
    tcol = (*iterator)->intersect(rayP, rayV, m_trans*trans, mt);
    
    if(col == NULL) col = tcol;
    else if(col != NULL && tcol != NULL){
      // compare both intersections and keep the closer one as col
      Point3D pt = tcol->getPoint();
      Point3D pc = col->getPoint();
      double ptn = (pt - rayP).normalize();
      double pcn = (pc - rayP).normalize();

      if(ptn < pcn){
      	col->setPoint(tcol->getPoint());
      	col->setNormal(tcol->getNormal());
      	col->setMaterial(tcol->getMaterial());
        col->setRefraction(tcol->isRefraction());
        if(tcol->isRefraction()) col->setRefAngle(tcol->getRefAngle());
      }

      free(tcol);
    }
    c++;
  }
  return col;
}

void SceneNode::rotate(char axis, double angle)
{
  std::cerr << "Stub: Rotate " << m_name << " around " << axis << " by " << angle << std::endl;
  // Fill me in
}

void SceneNode::scale(const Vector3D& amount)
{
  std::cerr << "Stub: Scale " << m_name << " by " << amount << std::endl;
  // Fill me in
}

void SceneNode::translate(const Vector3D& amount)
{
  std::cerr << "Stub: Translate " << m_name << " by " << amount << std::endl;
  // Fill me in
}

bool SceneNode::is_joint() const
{
  return false;
}

JointNode::JointNode(const std::string& name)
  : SceneNode(name)
{
}

JointNode::~JointNode()
{
}

bool JointNode::is_joint() const
{
  return true;
}

void JointNode::set_joint_x(double min, double init, double max)
{
  m_joint_x.min = min;
  m_joint_x.init = init;
  m_joint_x.max = max;
}

void JointNode::set_joint_y(double min, double init, double max)
{
  m_joint_y.min = min;
  m_joint_y.init = init;
  m_joint_y.max = max;
}

GeometryNode::GeometryNode(const std::string& name, Primitive* primitive)
  : SceneNode(name),
    m_primitive(primitive)
{
}

GeometryNode::~GeometryNode()
{
}
 

Intersection* GeometryNode::intersect(Point3D rayP, Vector3D rayV, Matrix4x4 trans, MasterTempo* mt){
  Intersection* inter = m_primitive->getIntersection(rayP, rayV, trans);
  if(inter != NULL) inter->setMaterial(m_material);
  return inter;
}
