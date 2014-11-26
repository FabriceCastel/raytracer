#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include "algebra.hpp"
#include "mastertempo.hpp"

class Particle{
public:
	Particle(Point3D startPosition, Vector3D direction, int lifespan, int midiNote, double minSize, double maxSize, double attack, double release);
	Point3D getCurrentPosition(){return position;};
	double getCurrentSize(){return curSize;};
	bool tick(MasterTempo* mt); // returns true iff particle still alive

private:
	Point3D position;
	Vector3D direction;
	int lifespan;
	int trigger;
	double minSize, maxSize, curSize;
	double attack, release;
};




#endif