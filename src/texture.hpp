#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <string>
#include "algebra.hpp"
#include "image.hpp"


class Texture{
public:
	Texture(std::string file);
	Vector3D getUV(double u, double v);

private:
	std::string filename;
	double* tData;
	int tHeight, tWidth;
	int tElems;
	Image tImage;
};



#endif