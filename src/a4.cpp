#include "a4.hpp"
#include "image.hpp"
#include "mastertempo.hpp"
#include <ctime>
#include <stdlib.h>
#include <sstream>
#include <pthread.h>

#define NUM_THREADS 8

void printProgBar( int percent ){
	std::string bar;

	for(int i = 0; i < 50; i++){
		if( i < (percent/2)){
			bar.replace(i,1,"=");
		}else if( i == (percent/2)){
			bar.replace(i,1,">");
		}else{
			bar.replace(i,1," ");
		}
	}

	std::cout<< "\r" "[" << bar << "] ";
	std::cout.width( 3 );
	std::cout<< percent << "%     " << std::flush;
}

//#define FRAME_COUNT (30 * 71.7)

void *renderNextStrip(void *params){
	StripRenderParams* renderParams = (StripRenderParams*) params;

    int height = renderParams->getHeight();

	int y = renderParams->getNextRow();
	MasterTempo* mt = renderParams->getMt();
	SceneNode* root = renderParams->getRoot();
    int width = renderParams->getWidth();
    Point3D eye = renderParams->getEye();
    Vector3D view = renderParams->getView();
    Vector3D up = renderParams->getUp();
    double fov = renderParams->getFov();
    Colour ambient = renderParams->getAmbient();
    std::list<Light*> lights = renderParams->getLights();
    double* rbuffer = renderParams->getRbuffer();
    Matrix4x4 world = renderParams->getWorld();

	while(y < height){
		//cout << "Rendering " << y << "\n";
		for(int x = 0; x < width; x++){
			int rbufferindex = 3*(y*width + x);
			double fovx = M_PI * ((double)fov/360.0);
			double fovy = (double)height/width * fovx;

			Point3D pixel = Point3D();
			pixel[0] = (2.0*x - width)/width * tan(fovx);
			pixel[1] = (-1 * (2.0*y - height)/height) * tan(fovy);
			pixel[2] = eye[2] - 1.0;

			Vector3D v = pixel - eye;

			v.normalize();
			bool rayWasRefracted = false;

			Intersection* col = root->intersect(pixel, v, world, mt);
			Intersection* initHit = NULL;

			if(col == NULL){
				rbuffer[rbufferindex++] = 0;
				rbuffer[rbufferindex++] = 0;
				rbuffer[rbufferindex++] = 0;
			} else {
				initHit = (Intersection*)malloc(sizeof(Intersection));
				*initHit = Intersection(col->getPoint(), col->getNormal(), col->getMaterial());
				initHit->setRefraction(col->isRefraction());
				initHit->setRefAngle(col->getRefAngle());
				if(col->hasTexture()){
					initHit->setTexture(col->getTexture());
					initHit->setTextureUV(col->getU(), col->getV());
				}

				int refractionBailout = 6;
				int curRefCount = 0;
				while(col != NULL && col->isRefraction()){
					Point3D point = col->getPoint();
					Vector3D normal = col->getNormal();
					normal.normalize();
					Vector3D refAngle = col->getRefAngle();
					free(col);
					col = root->intersect(point, refAngle, world, mt);
					rayWasRefracted = true;
					curRefCount++;
					if(curRefCount > refractionBailout) break;
				}

				Vector3D fc = Vector3D(1,1,1);

				if(col != NULL) {
					Material *mat = col->getMaterial();
					Colour kd = mat->getKD();

					fc = Vector3D(0,0,0);

					fc = shade(fc, lights, ambient, col, eye, root, mt);
					fc.cap(1.0);
					free(col);
				}


				if(rayWasRefracted){
					double opacityFactor = initHit->getNormal().dot(v);
					if(opacityFactor < 0) opacityFactor *= -1;

					opacityFactor = 1 - opacityFactor;
					opacityFactor = pow(opacityFactor, 3) + pow(opacityFactor + 0.1, 5);
					opacityFactor /= 2.0;
					if(opacityFactor > 1) opacityFactor = 1;
					
					double transparancy = 0.9 * (1.0 - opacityFactor);
					Colour glassKD = initHit->getMaterial()->getKD();
					Vector3D glassDiff = Vector3D(glassKD.R(), glassKD.G(), glassKD.B());
					Vector3D glassSpec = Vector3D(0.0, 0.0, 0.0);
					glassDiff = shade(glassDiff, lights, ambient, initHit, eye, root, mt);

					//
					// Hacky way of calculating ONLY the specular part (use black diffuse and white spec)
					//
					Intersection glassSpecI = (*initHit);
					const Colour black = Colour(0.0, 0.0, 0.0);
					const Colour white = Colour(1.0, 1.0, 1.0);
					double shine = initHit->getMaterial()->getShininess();
					Material* onlySpec = (Material*) new PhongMaterial(black, white, shine);
					glassSpecI.setMaterial(onlySpec);
					glassSpec = shade(glassSpec, lights, ambient, &glassSpecI, eye, root, mt);
					delete(onlySpec);
					//
					// </hack>
					//

					glassDiff.cap(1.0);
					fc = (transparancy*fc) + ((1.0-transparancy)*glassDiff) + glassSpec;

					// Sphere has radius 200
					//fc = (1.0 / (glassTraversed / 400.0)) * Vector3D(1.0, 1.0, 1.0);
				}

				fc.cap(1.0);

				rbuffer[rbufferindex++] = fc[0];
				rbuffer[rbufferindex++] = fc[1];
				rbuffer[rbufferindex++] = fc[2];

			}
			
			free(initHit);
		}
		y = renderParams->getNextRow();
	}
	pthread_exit(NULL);
	return NULL;
}

void render(// What to render
	SceneNode* root,
               // Where to output the image
	const std::string& filename,
               // Image size
	int width, int height,
               // Viewing parameters
	const Point3D& eye, const Vector3D& view,
	const Vector3D& up, double fov,
               // Lighting parameters
	const Colour& ambient,
	const std::list<Light*>& lights
	)
{
	//ParticleSystem ps("snow", Vector3D(0, -0.00001, 0), Point3D(0,0,0), 200.0, 1.0);
	
	// cout << "TEST\n";
	// ParticleSystem ptest = ParticleSystem("snow", Vector3D(0, -0.00001, 0), Point3D(0,0,0), 200.0, 1.0);
	// cout << "TESTSTST\n";
	// ParticleSystem* ps = (ParticleSystem*)malloc(sizeof(ptest));
	// *ps = ptest;

	// cout << "TEST\n";
	// //exit(1);
	// root->add_child(ps);

	int framerate = 25;
	const double FRAME_COUNT = 1165;//framerate * 71.7;
	MasterTempo masterTempo = MasterTempo("../data/1.mid", 180.0, framerate, 1);
	int SSAAFactor = 1;
	height *= SSAAFactor;
	width *= SSAAFactor;

	double filterWeight = 0;
	double filterRelease = 8.0/framerate;

	double *rbuffer = (double*)malloc(sizeof(double)*3*width*height);
	int rbufferindex = 0;

	const long double renderStartTime = time(0);
	long double previousFrameFinishTime = renderStartTime;

	for(int frame = 216; frame <= FRAME_COUNT; frame++){
		masterTempo.updateFrame(frame);
		rbufferindex=0;

		//
		// RENDER GEOMETRY
		//
		Matrix4x4 world = Matrix4x4();
		world = world.rotateX(frame);
		Point3D zeye = eye;
		double frameC = frame * (100.0/216.0);
		if(frame > 216) frameC = 100;
		if(frame > 800){
			frameC = 100 + (frame - 800)*0.35;
			// double start = 100;
			// double finish = 100 + (1066 - 800)*0.35;
			// frameC = (frameC - start) / (finish - start);
			// frameC *= frameC * (finish - start);
			// frameC += start;
		}
		if(frame > 1066) frameC = 100 + (1066-800)*0.35; // 1066
		zeye[2] += frameC/FRAME_COUNT * 200;



		StripRenderParams params = StripRenderParams(root, width, height,
           zeye, view, up, fov, ambient, lights, &masterTempo, rbuffer, world);

		pthread_t threads[NUM_THREADS];
		pthread_attr_t attr;
		void *status;
		int rc;
		int i;

		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

		for(i = 0; i < NUM_THREADS; i++){
			rc = pthread_create(&threads[i], NULL, renderNextStrip, (void*)&params);
			if(rc){
				cout << "ERROR: unable to create thread, " << rc << "\n";
				exit(1);
			}
		}

		pthread_attr_destroy(&attr);
		for(i = 0; i < NUM_THREADS; i++){
			rc = pthread_join(threads[i], &status);
			if(rc){
				cout << "ERROR: unable to join, " << rc << "\n";
				exit(1);
			}
		}
		//
		// RENDER GEOMETRY
		//

		if(masterTempo.getNoteStatus(3))
			filterWeight = 1.0;
		else if(filterWeight > 0)
			filterWeight -= filterRelease;
		if(filterWeight < 0)
			filterWeight = 0;

		if(filterWeight > 0)
			applySinCityFilter(rbuffer, height, width, filterWeight);

		if(masterTempo.getNoteStatus(0))
			applyCocaineFilter(rbuffer, height, width);

		long double delay = previousFrameFinishTime;
		previousFrameFinishTime = time(0);
		delay = previousFrameFinishTime - delay;
		long ttime = previousFrameFinishTime - renderStartTime;

		printProgBar((frame*100)/FRAME_COUNT);
		std::cout << "  frame " << frame << " of " << FRAME_COUNT << " in " << delay << "s" << " - total: " << ttime/3600 << ":" << (ttime/60)%60 << ":" << ttime%60;


		Image img(width/SSAAFactor, height/SSAAFactor, 3);
		rbufferindex = 0;

		for (int y = 0; y < height/SSAAFactor; y++) {
			for (int x = 0; x < width/SSAAFactor; x++) {
				double red = 0.0;
				double green = 0.0;
				double blue = 0.0;
				for(int i = 0; i < SSAAFactor; i++){
					for(int j = 0; j < SSAAFactor; j++){
						int sx = SSAAFactor * x;
						int sy = SSAAFactor * y;
						double sa = SSAAFactor * SSAAFactor;
						int baseIndex = 3 * (sx + sy*width);
						red   += rbuffer[baseIndex   + 3*i + 3*width*j ] / sa;
						green += rbuffer[baseIndex+1 + 3*i + 3*width*j ] / sa;
						blue  += rbuffer[baseIndex+2 + 3*i + 3*width*j ] / sa;
					}
				}
				img(x, y, 0) = red;
				img(x, y, 1) = green;
				img(x, y, 2) = blue;
			}
		}

		int frameCountScale = 100000000;

		string frameName = "";
		for(int mf = frame; mf < frameCountScale; mf *= 10){
			frameName = frameName + "0";
		}
		string frameNumberStr = "";
		stringstream ss;
		ss << frame;
		frameNumberStr = ss.str();
		frameName = frameName + frameNumberStr + ".png";

		img.savePng(frameName);

		root->tick(&masterTempo);
	}

	if(FRAME_COUNT > 1){
	
		stringstream ss;
		ss << "ffmpeg -i \%09d.png -framerate ";
		ss << framerate;
		ss << " -y -f avi -qscale 1 -s " << width/SSAAFactor << "x" << height/SSAAFactor;
		ss << " temp.avi";
		const char* ffmpegCommand = ss.str().c_str();
		cout << "\n\n";
		system(ffmpegCommand);

		stringstream sa;
		sa << "ffmpeg -i temp.avi -i audio.wav -y -f mp4 -qscale 1 -s " << width/SSAAFactor << "x" << height/SSAAFactor << " " << filename << ".mp4";
		const char* ffmpegAudio = sa.str().c_str();
		
		cout << "\n\n";
		system(ffmpegAudio);
		//system("rm *.png");
		system("clear");
		cout << "Render complete: " << filename << ".mp4\n";

	}
}

void applyCocaineFilter(double* rbuf, int bufheight, int bufwidth){
	string cocaineImage = "../data/cocaine.png";
	Image cocaine;
	cocaine.loadPng(cocaineImage);
	int width = cocaine.width();
	int height = cocaine.height();
	int numElements = cocaine.elements();
	double* cocaineData = cocaine.data();

	float u,v;
	int cx, cy;

	for(int h = 0; h < bufheight; h++){
		for(int w = 0; w < bufwidth; w++){
			u = ((float)w) / bufwidth;
			v = ((float)h) / bufheight;
			cx = (int)floor(u*width);
			cy = (int)floor(v*height);
			double cval = cocaineData[numElements*(width*cy + cx)];
			int bindex = 3*(h*bufwidth + w);
			if(cval < 0.5){
				rbuf[bindex++] = 0;
				rbuf[bindex++] = 0;
				rbuf[bindex++] = 0;
			} else {
				rbuf[bindex++] += 0.2;
				rbuf[bindex++] += 0.4;
				rbuf[bindex++] += 0.3;
			}
		}
	}
}

void applySinCityFilter(double* rbuf, int height, int width, double filterWeight){
	// string alig = "../data/ali_g.png";
	// Image ali;
	// ali.loadPng(alig);
	// int aliw = ali.width();
	// int alih = ali.height();
	// int aliElems = ali.elements(); // the number of doubles per pixel
	// double* aliData = ali.data();
	// int aliIndex = 0;
	double contrastThreshold = 0.05;
	double coontrastStrength = 3.0;
	
	int bufIndex = 0;
	for(int h = 0; h < height; h++){
		for(int w = 0; w < width; w++){
			//std::cout << "BIDX " << bufIndex << "\n";
			Vector3D orig = Vector3D(rbuf[bufIndex],rbuf[bufIndex+1],rbuf[bufIndex+2]);
			double greyscale = 0.33333 * orig.dot(Vector3D(1,1,1));
			Vector3D final = Vector3D(greyscale, greyscale, greyscale);

			double gWeight = smoothstep(0.0, 0.6, orig[0] - (orig[1]+orig[2])/2);

			final = (1.0 - gWeight)*final + gWeight*Vector3D(orig[0]*1.1, orig[1]*0.6, orig[2]*0.6);

			for(int g = 0; g < 3; g++){
				final[g] = std::pow(final[g] + contrastThreshold, coontrastStrength);
			}


			rbuf[bufIndex    ] = filterWeight*final[0] + (1 - filterWeight)*orig[0];
			rbuf[bufIndex + 1] = filterWeight*final[1] + (1 - filterWeight)*orig[1];
			rbuf[bufIndex + 2] = filterWeight*final[2] + (1 - filterWeight)*orig[2];
			bufIndex += 3;
		}
	}
}


Vector3D shade(Vector3D fc, std::list<Light*> lights, Colour ambient, Intersection* col, Point3D eye, SceneNode* root, MasterTempo* mt){
	Material *mat = col->getMaterial();
	Colour ks = mat->getKS();
	Colour kd = mat->getKD();
	if(col->hasTexture()){
		Vector3D nkd = col->getTexture();
		kd = Colour(nkd[0], nkd[1], nkd[2]);
	}
	int shininess = (int)mat->getShininess();
	
	Point3D point = col->getPoint();
	Vector3D normal = col->getNormal();
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
		//Intersection* shadow = root->intersect(point2, pointToLight, Matrix4x4(), mt);

		Intersection* shadow;
		int resolution = 1;
		double lightSize = 0;//40.0;
		double spread = 0.7;
		double shadowWeight = 0;
		double xi, yi, zi;
		

		for(int x = 0; x < resolution; x++){
			double xShift = (std::rand()  * spread/RAND_MAX - (spread/2)/RAND_MAX) * lightSize;
			double yShift = (std::rand()  * spread/RAND_MAX - (spread/2)/RAND_MAX) * lightSize;
			double zShift = (std::rand()  * spread/RAND_MAX - (spread/2)/RAND_MAX) * lightSize;
			//for(int y = 0; y < resolution; y++){
				//for(int z = 0; z < resolution; z++){
					//xi = lightSize + xShift;//((double)x)/resolution;// + xShift;
					//yi = lightSize + yShift;
					//zi = lightSize * ((double)z)/resolution;// + zShift;
					shadow = root->intersect(point2, Point3D(lpos[0]+xShift, lpos[1]+yShift, lpos[2]+zShift) - point2, Matrix4x4(), mt);
					if(shadow != NULL){
						shadowWeight += 1.0 / (resolution);//*resolution);
						free(shadow);
					}
				//}
			//}
		}

		//shadowWeight = 0;

		if(shadowWeight < 1){
			Colour lightColour = (*I)->colour;
			Vector3D lcol = Vector3D(lightColour.R(), lightColour.G(), lightColour.B());
	    	Vector3D iv = lpos - point; // incident vector
	    	Vector3D lpnorm = point - lpos;
	    	lpnorm.normalize();

	    	Vector3D tnorm = normal;
	    	tnorm.normalize();
	    	Vector3D s = lpos - point;
	    	s.normalize();
	    	Vector3D v = eye - point;
	    	v.normalize();
	    	// R = V - 2N(V . N)
	    	Vector3D r = iv - 2*(iv.dot(normal))*normal;
	    	r = -1 * r;
	    	r.normalize();
	    	double sdn = std::max(s.dot(tnorm), 0.0);
	    	Vector3D diffuse = sdn * Vector3D(lcol[0] * kd.R(),
	    		lcol[1] * kd.G(),
	    		lcol[2] * kd.B());
	    	Vector3D spec = Vector3D(0, 0, 0);
	    	if(shininess > 1){
		    	spec = std::pow(std::max(r.dot(v), 0.0), shininess) * Vector3D(ks.R() * lcol[0],
		    		ks.G() * lcol[1],
		    		ks.B() * lcol[2]);
	    	}

	    	// BLINN-PHONG SPEC
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
	    	if(blinnTerm > 1) blinnTerm = 1;
	    	//std::cout << blinnTerm << "\n";
	    	blinnTerm = std::pow(blinnTerm, shininess);
	    	//std::cout << blinnTerm << "\n";
	    	blinnSpec = blinnTerm * specColour;


	    	fc = fc + (1.0 - shadowWeight)*(diffuse + (shininess != 0 ? blinnSpec : 0*blinnSpec));
	    	//fc = Vector3D(blinnSpec[0], spec[0], 0);
	    }
	}
	return fc;
}
