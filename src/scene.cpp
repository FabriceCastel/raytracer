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
        if(tcol->hasTexture()){
          col->setTexture(tcol->getTexture());
          col->setTextureUV(tcol->getU(), tcol->getV());
        } else {
          col->clearTexture();
        }
      }

      free(tcol);
    }
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
 

void GeometryNode::tick(MasterTempo* mt){
  //if(std::strcmp(m_name.c_str(), "tc") == 0){
    m_primitive->tick(mt);
  //}
}

Intersection* GeometryNode::intersect(Point3D rayP, Vector3D rayV, Matrix4x4 trans, MasterTempo* mt){
  // if(std::strcmp(m_name.c_str(), "wall") == 0){
  //   if(mt->getNoteStatus(2)){
  //     m_material->setKD(Colour(0.05,0.05,0.05));
  //   } else {
  //     m_material->setKD(Colour(0.5, 0.7, 0.5));
  //   }
  // }

  Intersection* inter = m_primitive->getIntersection(rayP, rayV, trans);

  if(inter == NULL) return NULL;
  

  if(std::strcmp(m_name.c_str(), "s1") == 0){
    inter->setRefraction(true);
    Vector3D rf = refraction(1.6, inter->getNormal(), rayP, inter->getPoint());
    inter->setRefAngle(rf);
  }
  
  inter->setMaterial(m_material);
  if(m_texture != NULL && (!hasMidiTrigger() || mt->getNoteStatus(getMidiTrigger()))){
    double u = inter->getU();
    double v = inter->getV();
    inter->setTexture(m_texture->getUV(u, v));
  }
  return inter;
}


ParticleSystem::ParticleSystem(const std::string& name, Vector3D gravity, Point3D spawnZoneCorner, double spawnZoneSize, double spawnDensity) :
  SceneNode(name), gravity(gravity), spawnZoneCorner(spawnZoneCorner), spawnZoneSize(spawnZoneSize), spawnDensity(spawnDensity) {
    currentFrame = 1;
}

void ParticleSystem::tick(MasterTempo* mt){
  int newParticleCount = 0;
  while(std::rand() < spawnDensity * RAND_MAX){
    newParticleCount++;
    //cout << std::rand() << " _ " << spawnDensity << " * " << RAND_MAX << "\n";
  }
  for(int i = 0; i < newParticleCount; i++){
    double xrand = (double)std::rand() * spawnZoneSize;
    double yrand = 0.01 * (double)std::rand() * spawnZoneSize;
    double zrand = (double)std::rand() * spawnZoneSize;
    xrand /= RAND_MAX;
    yrand /= RAND_MAX;
    zrand /= RAND_MAX;
    Vector3D randStart = Vector3D(xrand, yrand, zrand);
    Point3D startPosition = spawnZoneCorner + randStart;
    Vector3D direction = (0.5 + ((double)std::rand()*3.5)/RAND_MAX) * gravity;
    int lifespan = 300;
    int midiNote = 6 + std::rand()%2;
    double minSize = 0;
    double maxSize = 0.1;
    double attack = 0.1;
    double release = 0.02;
    Particle particle(startPosition, direction, lifespan, midiNote, minSize, maxSize, attack, release);
    particles.push_front(particle);
  }

  bool particleAlive = true;

  std::list<Particle>::iterator I = particles.begin();
  while(I != particles.end()){
    particleAlive = I->tick(mt);
    //particleAlive = (I->getCurrentPosition()[1] < 800);
    if(!particleAlive){
      particles.erase(I++);
    } else {
      ++I;
    }
  }
  // if(particles.size() > 2){
  //   particleAlive = I->tick(mt);
  //   if(!particleAlive){
  //     particles.erase(I);
  //   }
  // }
}

Intersection* ParticleSystem::intersect(Point3D rayP, Vector3D rayV, Matrix4x4 trans, MasterTempo* mt){

  Intersection* col = NULL;
  Intersection* tcol = NULL;
  int c = 0;

  std::list<Particle>::iterator I = particles.begin();
  while(I != particles.end()){
    //++I;
    double rad = (*I).getCurrentSize();
    if(rad <= 0){
      ++I;
      continue;
    }

    Point3D pos = (*I).getCurrentPosition();
    NonhierSphere particle = NonhierSphere(pos, rad);
    tcol = particle.getIntersection(rayP, rayV, trans);

    ++I;
    
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

  if(col != NULL){
    PhongMaterial* mat = new PhongMaterial(Colour(1,1,1), Colour(1,1,1), 200.0);
    col->setMaterial(mat);
  }
  return col;
}