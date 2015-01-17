#include "a4.hpp"
#include "image.hpp"
#include "mastertempo.hpp"
#include <ctime>
#include <stdlib.h>
#include <sstream>
#include <pthread.h>

#define NUM_THREADS 8
#define MAX_REFRACTION_COUNT 6

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

			Ray ray = {pixel, v};

			Intersection col = Intersection();
			root->intersect(ray, col, world, mt);
			Intersection initHit = Intersection();

			if(!col.isValid()){
				rbuffer[rbufferindex++] = 0;
				rbuffer[rbufferindex++] = 0;
				rbuffer[rbufferindex++] = 0;
			} else {
				initHit = col;

				int curRefCount = 0;
				while(col.isValid() && col.isRefraction()){
					Point3D point = col.getPoint();
					Vector3D normal = col.getNormal();
					normal.normalize();
					Vector3D refAngle = col.getRefAngle();
					Ray r = {point, refAngle};
					root->intersect(r, col, world, mt);
					rayWasRefracted = true;
					curRefCount++;
					if(curRefCount > MAX_REFRACTION_COUNT) break;
				}

				Vector3D fc = Vector3D(1,1,1);

				if(col.isValid()) {
					Material *mat = col.getMaterial();
					Colour kd = mat->getKD();

					fc = Vector3D(0,0,0);

					fc = shade(fc, lights, ambient, col, eye, root, mt);
					fc.cap(1.0);
				}


				if(rayWasRefracted){
					double opacityFactor = initHit.getNormal().dot(v);
					if(opacityFactor < 0) opacityFactor *= -1;

					opacityFactor = 1 - opacityFactor;
					opacityFactor = pow(opacityFactor, 3) + pow(opacityFactor + 0.1, 5);
					opacityFactor /= 2.0;
					if(opacityFactor > 1) opacityFactor = 1;
					
					double transparancy = 0.9 * (1.0 - opacityFactor);
					Colour glassKD = initHit.getMaterial()->getKD();
					Vector3D glassDiff = Vector3D(glassKD.R(), glassKD.G(), glassKD.B());
					Vector3D glassSpec = Vector3D(0.0, 0.0, 0.0);
					glassDiff = shade(glassDiff, lights, ambient, initHit, eye, root, mt);

					//
					// Hacky way of calculating ONLY the specular part (use black diffuse and white spec)
					//
					Intersection glassSpecI = initHit;
					const Colour black = Colour(0.0, 0.0, 0.0);
					const Colour white = Colour(1.0, 1.0, 1.0);
					double shine = initHit.getMaterial()->getShininess();
					Material* onlySpec = (Material*) new PhongMaterial(black, white, shine);
					glassSpecI.setMaterial(onlySpec);
					glassSpec = shade(glassSpec, lights, ambient, glassSpecI, eye, root, mt);
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

	int framerate = 25;
	const double FRAME_COUNT = 10000;//1165;//framerate * 71.7;
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

	for(int frame = 1; frame <= FRAME_COUNT; frame++){
		masterTempo.updateFrame(frame);
		rbufferindex=0;

		//
		// RENDER GEOMETRY
		//
		Matrix4x4 world = Matrix4x4();
		world = world.rotateX(frame);

		StripRenderParams params = StripRenderParams(root, width, height,
           eye, view, up, fov, ambient, lights, &masterTempo, rbuffer, world);

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

	} else {
		cout << "\n";
	}
}



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
