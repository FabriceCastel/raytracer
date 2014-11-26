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

void SceneNode::tick(MasterTempo* mt){
  for (ChildList::const_iterator iterator = m_children.begin(), end = m_children.end(); iterator != end; ++iterator) {
    (*iterator)->tick(mt);
  }
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
  if(std::strcmp(m_name.c_str(), "wall") == 0){
    if(mt->getNoteStatus(2)){
      m_material->setKD(Colour(0.05,0.05,0.05));
    } else {
      m_material->setKD(Colour(0.5, 0.7, 0.5));
    }
  }
  Intersection* inter = m_primitive->getIntersection(rayP, rayV, trans);
  if(inter != NULL) inter->setMaterial(m_material);
  return inter;
}


ParticleSystem::ParticleSystem(const std::string& name, Vector3D gravity, Point3D spawnZoneCorner, double spawnZoneSize, double spawnDensity) :
  SceneNode(name), gravity(gravity), spawnZoneCorner(spawnZoneCorner), spawnZoneSize(spawnZoneSize), spawnDensity(spawnDensity) {
    currentFrame = 1;
}

void ParticleSystem::tick(MasterTempo* mt){
  int newParticleCount = (int)((std::rand()%2)*(std::rand()%2)*(std::rand()%2));//((int)(1.0/spawnDensity)));
  for(int i = 0; i < newParticleCount; i++){
    double xrand = (double)std::rand() * spawnZoneSize;
    double yrand = 0;//(double)std::rand() * spawnZoneSize;
    double zrand = (double)std::rand() * spawnZoneSize;
    xrand /= RAND_MAX;
    //yrand /= RAND_MAX;
    zrand /= RAND_MAX;
    Vector3D randStart = Vector3D(xrand, yrand, zrand);
    Point3D startPosition = spawnZoneCorner + randStart;
    Vector3D direction = (1.0 + ((float)std::rand()*2)/RAND_MAX) * gravity;
    int lifespan = 200;
    int midiNote = 6 + std::rand()%2;
    //cout << "\n" << midiNote << "\n";
    double minSize = 0;
    double maxSize = 10;
    double attack = 5;
    double release = 1;
    Particle particle(startPosition, direction, lifespan, midiNote, minSize, maxSize, attack, release);
    particles.push_front(particle);
  }

  bool particleAlive = true;

  std::list<Particle>::iterator I = particles.begin();
  while(I != particles.end()){
    particleAlive = I->tick(mt);
    if(!particleAlive){
      particles.erase(I++);
    } else {
      ++I;
    }
  }
}

Intersection* ParticleSystem::intersect(Point3D rayP, Vector3D rayV, Matrix4x4 trans, MasterTempo* mt){

  Intersection* col = NULL;
  Intersection* tcol = NULL;
  int c = 0;

  std::list<Particle>::iterator I = particles.begin();
  while(I != particles.end()){
    ++I;
    double rad = (*I).getCurrentSize();
    if(rad <= 0) continue;

    Point3D pos = (*I).getCurrentPosition();
    NonhierSphere particle = NonhierSphere(pos, rad);
    tcol = particle.getIntersection(rayP, rayV, trans);
    
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
      c++;
    }
  }
  //PhongMaterial* mat = (PhongMaterial*)malloc(sizeof(PhongMaterial));
  if(col != NULL){
    // double r = 1.0*std::rand();
    // double g = 1.0*std::rand();
    // double b = 1.0*std::rand();
    // r /= RAND_MAX;
    // g /= RAND_MAX;
    // b /= RAND_MAX;
    PhongMaterial* mat = new PhongMaterial(Colour(1,0.4,0.5), Colour(1,1,1), 200.0);
    col->setMaterial(mat);
  }
  return col;
}