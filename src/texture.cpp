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
	int fx = (int)floor(u*tWidth);
	int fy = (int)floor(v*tHeight);
	//int cx = (int)ceil(u*tWidth);
	//int cy = (int)ceil(v*tHeight);

	int pix;
	pix = tElems * (tWidth*fy + fx);
	Vector3D ff = Vector3D(tData[pix], tData[pix+1], tData[pix+2]);
	//pix = tElems * (tWidth*cy + fx);
	//Vector3D fc = Vector3D(tData[pix], tData[pix+1], tData[pix+2]);
	//pix = tElems * (tWidth*fy + cx);
	//Vector3D cf = Vector3D(tData[pix], tData[pix+1], tData[pix+2]);
	//pix = tElems * (tWidth*cy + cx);
	//Vector3D cc = Vector3D(tData[pix], tData[pix+1], tData[pix+2]);

	// double wff = u*tWidth - fx + v*tHeight - fy;
	// double wfc = u*tWidth - fx + cy - v*tHeight;
	// double wcf = cx - u*tWidth + v*tHeight - fy;
	// double wcc = cx - u*tWidth + cy - v*tHeight;

	// double tw = wff + wfc + wcf + wcc;

	// wff = wff/tw;
	// wfc = wfc/tw;
	// wcf = wcf/tw;
	// wcc = wcc/tw;

	//cout << "\n" << wff << "\n" << wfc << "\n" << wcf << "\n" << wcc << "\n";
	//exit(1);

	return ff;//wcc*ff + wcf*fc + wfc*cf + wff*cc;
}