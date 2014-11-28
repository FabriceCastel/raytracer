#include "texture.hpp"

using namespace std;

Texture::Texture(string name) :
	filename(name){

	if(tImage.loadPng(filename) == false){
		std::cout << "ERROR: Failed to load texture \'" << filename << "\'\n";
		return;
	}
	tWidth = tImage.width();
	tHeight = tImage.height();
	tElems = tImage.elements();
	tData = tImage.data();
}

Vector3D Texture::getUV(double u, double v){
	int cx = (int)floor(u*tWidth);
	int cy = (int)floor(v*tHeight);

	int pixelIndex = tElems * (tWidth*cy + cx);

	double r = tData[pixelIndex    ];
	double g = tData[pixelIndex + 1];
	double b = tData[pixelIndex + 2];

	return Vector3D(r, g, b);
}