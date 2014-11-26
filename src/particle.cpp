#include "particle.hpp"
#include "algebra.hpp"


Particle::Particle(Point3D startPosition, Vector3D direction, int lifespan, int midiNote, double minSize, double maxSize, double attack, double release) :
	position(startPosition), direction(direction), lifespan(lifespan),
	trigger(midiNote), minSize(minSize), maxSize(maxSize), attack(attack), release(release) {
	
}


bool Particle::tick(MasterTempo* mt){
	lifespan--;
	if(lifespan < 1) return false;

	if(mt->getNoteStatus(trigger))
		curSize += attack;
	else
		curSize -= release;

	curSize = clamp(curSize, minSize, maxSize);
	position = position + direction;
	return true;
}