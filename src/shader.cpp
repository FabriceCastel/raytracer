#include "shader.hpp"
#include "material.hpp"

Vector3D shade(Vector3D fc, std::list<Light*> lights, Colour ambient, Intersection col, Point3D eye, SceneNode* root, MasterTempo* mt){
	Material *mat = col.getMaterial();
	Colour ks = mat->getKS();
	Colour kd = mat->getKD();
	if(col.hasTexture()){
		Vector3D nkd = col.getTexture();
		kd = Colour(nkd[0], nkd[1], nkd[2]);
	}
	int shininess = (int)mat->getShininess();
	
	Point3D point = col.getPoint();
	Vector3D normal = col.getNormal();
	normal.normalize();

	fc = Vector3D(ambient.R()*kd.R(), ambient.G()*kd.G(), ambient.B()*kd.B());

	for (std::list<Light*>::iterator I = lights.begin(); I != lights.end(); ++I) {
		Point3D lpos = (*I)->position;

		// cast ray from point to light and see if it
		// intersects anything before the light (= shadow)
		double fp2 = 0.02;
		Point3D point2 = Point3D(point[0] + fp2*normal[0],
			point[1] + fp2*normal[1],
			point[2] + fp2*normal[2]);
		Vector3D pointToLight = lpos - point;
		pointToLight.normalize();

		Intersection shadow = Intersection();
		
		Ray r = {point2, lpos - point2};
		root->intersect(r, shadow, Matrix4x4(), mt);
		if(!shadow.isValid()){
			Colour lightColour = (*I)->colour;
			Vector3D lcol = Vector3D(lightColour.R(), lightColour.G(), lightColour.B());
	    	
	    	Vector3D tnorm = normal;
	    	tnorm.normalize();
	    	Vector3D s = lpos - point;
	    	s.normalize();

	    	double sdn = std::max(s.dot(tnorm), 0.0);
	    	Vector3D diffuse = sdn * Vector3D(lcol[0] * kd.R(),
	    		lcol[1] * kd.G(),
	    		lcol[2] * kd.B());

	    	// BLINN SPEC
	    	Vector3D lightDirection = lpos - point;
	    	lightDirection.normalize();
	    	float cosAngInsidence = normal.dot(lightDirection);
	    	Vector3D blinnSpec = Vector3D(0,0,0);
	    	Vector3D specColour = Vector3D(ks.R()*lcol[0],ks.G()*lcol[1],ks.B()*lcol[2]);
	    	Vector3D halfAngle = (lpos - point) + (eye - point);
	    	Vector3D hacp = halfAngle;
	    	halfAngle.normalize();
	    	halfAngle = (1.0/hacp.normalize()) * halfAngle;
	    	halfAngle.normalize();
	    	double blinnTerm = halfAngle.dot(normal);
	    	if(blinnTerm < 0 || cosAngInsidence == 0.0) blinnTerm = 0;
	    	else if(blinnTerm > 1) blinnTerm = 1;
	    	blinnSpec = std::pow(blinnTerm, shininess) * specColour;


	    	fc = fc + diffuse + (shininess > 0 ? blinnSpec : 0*blinnSpec);
	    }
	}
	return fc;
}