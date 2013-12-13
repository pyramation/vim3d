#include "Render.h"
#include "Image.h"
#include "Ray.h"
#include "Scene.h"

#define AU(j) (au*alphaTan*((float((j))-HalfW)/HalfW))
#define BV(i) (bv*betaTan*(HalfH - float((i)))/HalfH)

#define RAND (float(rand())/float(RAND_MAX))

int Render::samples = 1;
int Render::bounces = 5; 
int Render::w = 480;
int Render::h = 360;
unsigned int Render::start = 0;
unsigned int Render::end = 0;

bool Render::jitter=false;
bool Render::cartoon=false;
bool Render::depth_map=false;


void Render::RayTrace(String filename) {

    srand(1);

    for(unsigned int o=0; o<allOBJS.size();o++) {
        // only look at geos that are top-level (don't have parent) 
        if (isGeo(allOBJS[o]) && !O(allOBJS[o])->parent) {
            G(allOBJS[o])->bound();
            G(allOBJS[o])->showBounds = true;
        }
    }

    Image * image = new Image(w,h,GL_RGB,GL_UNSIGNED_BYTE);
    Image * depthMap = NULL;
    if (depth_map) depthMap = new Image(w,h,GL_RGB,GL_UNSIGNED_BYTE);

    float min = FLT_MAX;
    float max = FLT_MIN;

    float fovy = SceneCAM->fovy/180.0*PI_;
    float betaTan = tan(fovy/2.0f);
    float fovx = fovy*float(w)/float(h);
    float alphaTan = betaTan*float(w)/float(h);

    Ray ray = Ray();
    ray.pos = SceneCAM->eye;

    float HalfW = float(w)/2.0f;
    float HalfH = float(h)/2.0f;
    Point wu = Point::normalize(SceneCAM->focus-SceneCAM->eye);
    Point au = Point::normalize(Point::cross(SceneCAM->up, wu));//frame.u*alpha;
    Point bv = Point::normalize(Point::cross(au, wu));//frame.v*beta;	
    printf("width: %d, height %d, w*h:  %d\n", w,h,w*h);
    printf("alphaTan:%f, betaTan:%f, fovx:%f, fovy:%f\n", alphaTan, betaTan, fovx, fovy);



    // for every pixel
    for(int i=h-1;i>=0;i--) {
        if(i%(int(h/10))==0){
            int l = int(100-(float(i)/h)*100);
            l = (int((l+5)/10))*10;
            printf("rendering %d complete\n", l);
        }		
        for(int j=0;j<w;j++) {
            // inside of the pixel
            // samples = n
            float n=Render::samples;
            ray = Ray();
            ray.pos = SceneCAM->eye;
            Pixel px;
            px.color = Color(0,0,0);

            if (n > 1) {
                // anti-aliasing
                for(float p=0; p<n-1;p+=1) {
                    for(float q=0; q<n-1;q+=1) {
                        // you can turn jitter on with "render jitter 1", other wise you get regular sampling
                        if(jitter) ray.dir = (AU(j + (p+RAND)/n)+BV(i + (q+RAND)/n)+wu);
                        else ray.dir = (AU(j + (p+0.5)/n)+BV(i + (q+0.5)/n)+wu);
                        Pixel p = getColor(ray,bounces);
                        px.color+=p.color;
                        px.depth= p.depth;
                    }
                }
                // this used to be n*n
                px.color/=float((n-1)*(n-1));
                //px.depth/=float(n*n);
            } else {
                ray.dir = (AU(j)+BV(i)+wu);
                Pixel p=getColor(ray, bounces);
                px.color=p.color;
                px.depth=p.depth;
            }

            px.color*=255.0f;

            if (cartoon) { // CARTOON RENDERING
                float step = 255/5;
                px.color.r = float(int(px.color.r/step)*step);
                px.color.g = float(int(px.color.g/step)*step);
                px.color.b = float(int(px.color.b/step)*step);
            }
            px.color.clamp();


            image->pixels[3*(i*w+w-j-1)+1] = px.color.g;
            image->pixels[3*(i*w+w-j-1)+2] = px.color.b;
            image->pixels[3*(i*w+w-j-1)+0] = px.color.r;

            if (depth_map) {
                depthMap->pixels[3*(i*w+w-j-1)+1] = px.depth;
                depthMap->pixels[3*(i*w+w-j-1)+2] = px.depth;
                depthMap->pixels[3*(i*w+w-j-1)+0] = px.depth;
            }

            if (depth_map && px.depth >= 0) {
                if (px.depth < min) min = px.depth;
                if (px.depth > max) max = px.depth;
            }


        }
    }


    if (depth_map) {
        printf("min depth: %f\nmax depth: %f\n", min, max);
        for(int i=0;i<3*w*h;i++) {
            if (depthMap->pixels[i] < 0) depthMap->pixels[i]=0;
            else depthMap->pixels[i] = 255.0f - Point::fit(depthMap->pixels[i], min, max, 0, 255);        
        }
    }

    if (false) {
        for(int i=0;i<h;i++) {
            for(int j=0;j<w;j++) {
                float val = depthMap->pixels[3*(i*w+w-j-1)+0];
                if (val >= 0) {
                    Color chan = Color();
                    val = Point::fit(val, min, max, 0, 255*3);
                    if (val > 255) chan.r = 255;
                    else chan.r = val;
                    val-=255;
                    if (val > 255) chan.g = 255;
                    else if (val > 0) chan.g = val;
                    val-=255;
                    if (val > 0) chan.b = val;
                    depthMap->pixels[3*(i*w+w-j-1)+0] = chan.b;
                    depthMap->pixels[3*(i*w+w-j-1)+1] = chan.r;
                    depthMap->pixels[3*(i*w+w-j-1)+2] = chan.g;
                } else {
                    depthMap->pixels[3*(i*w+w-j-1)+0] = 0;
                    depthMap->pixels[3*(i*w+w-j-1)+1] = 0;
                    depthMap->pixels[3*(i*w+w-j-1)+2] = 0;

                }
            }
        }

    }

    printf("w: %d,h: %d\n", w,h);
    image->WriteImage(filename);
    if(depth_map){
        depthMap->WriteImage(String("depthmap.ppm"));
        //Image::SobelEdgeDetect(w,h,depthMap->pixels);
        //depthMap->WriteImage(String("sobel.ppm"));
        delete depthMap;
    }
    //WriteImageToScreen(x,y,w,h,GL_RGB,GL_UNSIGNED_BYTE, image->pixels);
    delete image;
}

Intersection::Intersection(){ geo = NULL;}
Intersection::~Intersection(){}
Intersection Render::getIntersection(Ray ray){
    Point hit = Point(0.0f,0.0f,0.0f);
    float closestD = FLT_MAX;
    Intersection intersection = Intersection();
    intersection.eye = ray.pos;
    intersection.geo = NULL;
    for(unsigned int o=0; o<allOBJS.size();o++) {
        // only look at geos that are top-level (don't have parent) and have display set to true
        if (isGeo(allOBJS[o]) && !O(allOBJS[o])->parent && G(allOBJS[o])->disp) {

            float dist = closestD;
            Intersection newIntersection = G(allOBJS[o])->getIntersection(ray, dist, Transform::identity());
            if (dist<closestD) {
                closestD=dist;
                intersection = newIntersection;
            }

        }
    }
    return intersection;
}

Color Render::shadowRays(Intersection intersection){
    if (intersection.geo) {
        Normal n = intersection.n;
        Point hit = intersection.hit;
        Color ambientColor(intersection.geo->ambient[0], intersection.geo->ambient[1], intersection.geo->ambient[2]);
        Color emissionColor(intersection.geo->emission[0], intersection.geo->emission[1], intersection.geo->emission[2]);
        Color retColor = emissionColor + ambientColor;
        for(unsigned int o=0; o<allOBJS.size();o++) {
            // only look at lights!
            if (!isLight(allOBJS[o])) continue;

            vector<Point>r;
            //vector<Point>s;
            if (L(allOBJS[o])->ltype == L_AREA) {

                int Lightsamples = samples*3;

                vector<Point>r;
                coordinateFrame_t c = Transform::getCoordinateFrame(Point(0,1,0), Point(1,0,0));
                //coordinateFrame_t c = coordinateFrame_t();
                //c.v = Point(1,0,0);
                //c.u = Point(0,0,1);
                for(int k=0;k<Lightsamples;k++) {
                    r.push_back(L(allOBJS[o])->pos() + c.v*L(allOBJS[o])->w*RAND + c.u*L(allOBJS[o])->h*RAND );
                    //s.push_back(L(allOBJS[o])->pos() + c.v*L(allOBJS[o])->w*RAND + c.u*L(allOBJS[o])->h*RAND );
                }
                //random_shuffle(s.begin(), s.end());
                for(int p=0;p<samples;p++) {
                    Ray shadowRay = Ray();
                    shadowRay.pos = intersection.hit;
                    shadowRay.dir = r[p] - intersection.hit;
                    shadowRay.dir.normalize(); 
                    // move shadowRay a bit
                    shadowRay.pos = shadowRay.pos + shadowRay.dir*0.001;
                    Intersection intersect = getIntersection(shadowRay);
                    bool noShadow = true;
                    if (intersect.geo) {
                        Point toGeo = shadowRay.pos - intersect.hit;
                        Point toLight = shadowRay.pos - L(allOBJS[o])->pos(); 
                        if (toGeo.length() < toLight.length()) {
                            noShadow = false;
                        } 
                    }

                    if (noShadow) {

                        if (intersection.mat.normalMap) {
                            Normal fromMap(0.0f,0.0f,0.0f);
                            unsigned int width = intersection.mat.normalMap->w;
                            unsigned int height = intersection.mat.normalMap->h;
                            unsigned int xCoord = int((width - 1) * intersection.t.x) % width; 
                            unsigned int yCoord = int((height - 1) * intersection.t.y) % height; 
                            fromMap.x = intersection.mat.normalMap->pixels[4*(xCoord+yCoord*width)+1]/255.0f;
                            fromMap.y = intersection.mat.normalMap->pixels[4*(xCoord+yCoord*width)+0]/255.0f;
                            fromMap.z = intersection.mat.normalMap->pixels[4*(xCoord+yCoord*width)+2]/255.0f;
                            intersection.n += fromMap;
                            intersection.n.normalize();

                        }
                        Color fromLight = lightingModel(L(allOBJS[o]), &intersection.mat, intersection.eye, intersection.hit, intersection.n);
                        retColor += fromLight;

                        /* textures */
                        if (intersection.mat.texture) {
                            if (!intersection.mat.texture->image) intersection.mat.texture->image = new Image(intersection.mat.texture->filepath);

                            Color fromTex(0.0f,0.0f,0.0f);
                            unsigned int width = intersection.mat.texture->w;
                            unsigned int height = intersection.mat.texture->h;
                            unsigned int xCoord = int((width - 1) * intersection.t.x) % width; 
                            unsigned int yCoord = int((height - 1) * intersection.t.y) % height; 
                            fromTex.r = intersection.mat.texture->image->pixels[4*(xCoord+yCoord*width)+1]/255.0f;
                            fromTex.g = intersection.mat.texture->image->pixels[4*(xCoord+yCoord*width)+0]/255.0f;
                            fromTex.b = intersection.mat.texture->image->pixels[4*(xCoord+yCoord*width)+2]/255.0f;
                            retColor *= fromTex;
                        }

                    }

                }
                retColor /= float(samples);

            } else {


                Ray shadowRay = Ray();
                shadowRay.pos = intersection.hit;
                shadowRay.dir = L(allOBJS[o])->pos() - intersection.hit;
                shadowRay.dir.normalize(); 
                // move shadowRay a bit
                shadowRay.pos = shadowRay.pos + shadowRay.dir*0.001;
                Intersection intersect = getIntersection(shadowRay);


                bool noShadow = true;
                if (intersect.geo) {
                    Point toGeo = shadowRay.pos - intersect.hit;
                    Point toLight = shadowRay.pos - L(allOBJS[o])->pos(); 
                    if (toGeo.length() < toLight.length()) {
                        noShadow = false;
                    } 
                }

                if (noShadow) {

                    if (intersection.mat.normalMap) {
                        Normal fromMap(0.0f,0.0f,0.0f);
                        unsigned int width = intersection.mat.normalMap->w;
                        unsigned int height = intersection.mat.normalMap->h;
                        unsigned int xCoord = int((width - 1) * intersection.t.x) % width; 
                        unsigned int yCoord = int((height - 1) * intersection.t.y) % height; 
                        fromMap.x = intersection.mat.normalMap->pixels[4*(xCoord+yCoord*width)+1]/255.0f;
                        fromMap.y = intersection.mat.normalMap->pixels[4*(xCoord+yCoord*width)+0]/255.0f;
                        fromMap.z = intersection.mat.normalMap->pixels[4*(xCoord+yCoord*width)+2]/255.0f;
                        intersection.n += fromMap;
                        intersection.n.normalize();

                    }

                    Color fromLight = lightingModel(L(allOBJS[o]), &intersection.mat, intersection.eye, intersection.hit, intersection.n);
                    retColor += fromLight;

                    /* textures */
                    if (intersection.mat.texture) {
                        if (!intersection.mat.texture->image) intersection.mat.texture->image = new Image(intersection.mat.texture->filepath);

                        Color fromTex(0.0f,0.0f,0.0f);
                        unsigned int width = intersection.mat.texture->w;
                        unsigned int height = intersection.mat.texture->h;
                        unsigned int xCoord = int((width - 1) * intersection.t.x) % width; 
                        unsigned int yCoord = int((height - 1) * intersection.t.y) % height; 
                        fromTex.r = intersection.mat.texture->image->pixels[4*(xCoord+yCoord*width)+1]/255.0f;
                        fromTex.g = intersection.mat.texture->image->pixels[4*(xCoord+yCoord*width)+0]/255.0f;
                        fromTex.b = intersection.mat.texture->image->pixels[4*(xCoord+yCoord*width)+2]/255.0f;
                        retColor *= fromTex;
                    }

                }

            } // end of if/else for area lights

        }
        return retColor;
    }
    return Color(0,0,0);
}

Color Render::lightingModel(Light * light, Material * mat, Point eye, Point hit, Point n) {
    Color retColor = Color();
    // I had to reverse l (it used to be hit - L->pos)
    Vector l = light->pos() - hit;
    Vector h = hit - eye;
    l.normalize();
    h.normalize();
    h = l - h;
    h.normalize();
    float cosine = n*l;
    if (n*l < 0) cosine = 0; 
    float shine = pow(n*h, mat->shininess);
    retColor.r = mat->diffuse[0] * light->diffuse[0]*cosine + mat->specular[0] * light->specular[0]*shine;
    retColor.g = mat->diffuse[1] * light->diffuse[1]*cosine + mat->specular[1] * light->specular[1]*shine;
    retColor.b = mat->diffuse[2] * light->diffuse[2]*cosine + mat->specular[2] * light->specular[2]*shine;
    if (light->ltype == L_POINT || light->type == L_SPOT) {
        float dist = (hit-light->pos()).length();
        retColor.r /= (light->constant + light->linear*dist + light->quadratic*dist*dist);
        retColor.g /= (light->constant + light->linear*dist + light->quadratic*dist*dist);
        retColor.b /= (light->constant + light->linear*dist + light->quadratic*dist*dist);
    }
    return retColor;

}

Pixel Render::getColor(Ray ray, int bounces){
    Pixel px;
    px.color = Color(0,0,0);
    px.depth = -1;

    if (!bounces) return px;

    Intersection intersect = getIntersection(ray);

    /* DOES THE HIT POINT RECEIVE LIGHT FROM A SOURCE */
    Color t = shadowRays(intersect);

    if (intersect.geo) {
        px.depth = (intersect.eye - intersect.hit).length();

        intersect.n.normalize();

        /* REFRACTION COMPONENT */
        if (intersect.mat.ambient[3]<1.0) { //change this to alpha thingy
            Color refr = getRefraction(intersect, ray, bounces);
            float invalpha = 1.0-intersect.geo->ambient[3];			
            t.r += refr.r*invalpha;
            t.g += refr.g*invalpha;
            t.b += refr.b*invalpha;			
        }

        /* REFLECTION COMPONENT */
        if ((intersect.mat.reflection[0] +
                    intersect.mat.reflection[1] +
                    intersect.mat.reflection[2]) > 0.0f){
            Color refl = getReflection(intersect, ray, bounces);
            // need to divide fractionally reflected and refracted light
            t.r += intersect.mat.reflection[0]*refl.r;
            t.g += intersect.mat.reflection[1]*refl.g;
            t.b += intersect.mat.reflection[2]*refl.b;			   
        }

        /* MONTE CARLO */
        //random = true;
        /*
           if (false) {
           int N = 10000;
           for(int i=0; i<N; i++) {

           Ray randomRay = Ray();
           randomRay.dir = Point((RAND-0.5)*2, (RAND-0.5)*2, (RAND-0.5)*2);
        // ensure that we go away from the intersection
        while( intersect.n * randomRay.dir < 0 )
        randomRay.dir = Point((RAND-0.5)*2, (RAND-0.5)*2, (RAND-0.5)*2); 

        randomRay.dir.normalize();
        randomRay.pos = intersect.hit + randomRay.dir*0.1; 

        if (true) {
        Intersection intersectMonte = getIntersection(randomRay);

        if (intersectMonte.geo) {
        t.r += intersectMonte.geo->diffuse[0]/N;
        t.g += intersectMonte.geo->diffuse[1]/N;
        t.b += intersectMonte.geo->diffuse[2]/N;
        } 
        } else {
        t += getColor(randomRay, 0);

        }
        }
        //random = false;
        }
         */

    }
    px.color = t;
    return px;
}

Color Render::getReflection(Intersection intersect, Ray ray, int bounces){
    Ray reflectionRay = Ray();
    Point d = Point::normalize(ray.dir);
    Point iN = intersect.n;
    float cosTheta1 = Point::dot(iN, d*-1);
    //float theta = arccos(Point::dot(d, iN));
    reflectionRay.dir = Point::normalize(d+(iN*cosTheta1*2));
    reflectionRay.pos = intersect.hit + (reflectionRay.dir*.01);
    return (getColor(reflectionRay, bounces-1)).color;
}

Color Render::getRefraction(Intersection intersect, Ray ray, int bounces){
    float n1 = 1.0f;
    float n2 = intersect.geo->refractiveIndex; //this should be a property of objects
    float n1dn2 = n1/n2;
    float n2dn1 = n2/n1;
    Point center = intersect.geo->pos();	
    Point d = Point::normalize(ray.dir);
    Point iN = intersect.n;
    float cosTheta1 = Point::dot(iN, d*-1);	
    float cosTheta2 = 1-((n1dn2*n1dn2)*(1-cosTheta1*cosTheta1));
    if (cosTheta2>0.0) {
        cosTheta2 = sqrt(cosTheta2);
        Ray refractionRay = Ray();
        Point ref;
        if (cosTheta1>0.0) ref = Point::normalize(d*(n1dn2)+(iN*(n1dn2*cosTheta1-cosTheta2))); 
        else ref = Point::normalize(d*(n1dn2)+(iN*(n1dn2*cosTheta1+cosTheta2))); 
        refractionRay.dir = ref;
        refractionRay.pos = intersect.hit + (refractionRay.dir*.01);
        //Intersection inter = Intersection();
        Point hit = Point(0.0f,0.0f,0.0f);			
        if (isSphere(intersect.geo)) { //get out ray from inside sphere 
            if (Ray::intersectSphere(refractionRay, ((Sphere*)intersect.geo)->radius, center, &hit)) {
                intersect.hit = hit;
                intersect.n = Point::normalize(center-hit);
                d = refractionRay.dir;
                iN = intersect.n;
                cosTheta1 = Point::dot(iN, refractionRay.dir*-1);
                cosTheta2 = 1-((n2dn1*n2dn1)*(1-cosTheta1*cosTheta1));
                if (cosTheta2>0) { 
                    cosTheta2 =  sqrt(cosTheta2);
                    if (cosTheta1>0.0) ref = Point::normalize(d*(n2dn1)+(iN*(n2dn1*cosTheta1-cosTheta2))); 
                    else ref = Point::normalize(d*(n2dn1)+(iN*(n2dn1*cosTheta1+cosTheta2))); 					
                    refractionRay.dir = ref;//ractionRay.dir*(n2dn1)+intersect.n*(n2dn1*cosTheta1+cosTheta2);
                    refractionRay.pos = hit + (refractionRay.dir*.01);
                    return (getColor(refractionRay, bounces-1)).color;
                } else{ // critical angled
                    // for now no internal reflection even on full critical
                    return Color(0,0,0);
                }
            }
        } else { 
            // deal with other objects internal travel
            float dist = FLT_MAX;
            Intersection newI = intersect.geo->getIntersection(refractionRay, dist, Transform::identity());//O(intersect.geo)->M);
            if(newI.geo) {
                d = refractionRay.dir;
                iN = newI.n*-1;
                hit = newI.hit;
            } else{
                d = refractionRay.dir;
                iN = intersect.n;
                if (Point::dot(d, iN)>0.0) {
                    hit = hit + iN * 2.0;
                } else{
                    hit = hit + iN * -2.0;
                }
            }
            cosTheta1 = Point::dot(iN, d*-1);
            cosTheta2 = 1-((n2dn1*n2dn1)*(1-cosTheta1*cosTheta1));	
            if (cosTheta2>0) { 
                cosTheta2 =  sqrt(cosTheta2);
                if (cosTheta1>0.0) ref = Point::normalize(d*(n2dn1)+(iN*(n2dn1*cosTheta1-cosTheta2))); 
                else ref = Point::normalize(d*(n2dn1)+(iN*(n2dn1*cosTheta1+cosTheta2))); 					
                refractionRay.dir = ref;//ractionRay.dir*(n2dn1)+intersect.n*(n2dn1*cosTheta1+cosTheta2);
                refractionRay.pos = hit + (refractionRay.dir*.01);
                return (getColor(refractionRay, bounces-1)).color;

            } else{ // beyond critical angle
                return Color(0,0,0);
            }
        }

    } else{
        //all external reflection should not occur with n1<n2
        printf("this shouldn't happen\n");
        return Color(0,0,0);
    }
    //reflectionRay.dir = ray.dir - (intersect.n * ( ray.dir * intersect.n)); 
    //reflectionRay.dir = intersect.n - ray.dir * ( ray.dir * intersect.n); 
    //reflectionRay.dir.normalize();
    //reflectionRay.pos = intersect.hit + reflectionRay.dir*1;
    return Color(0,0,0);
}

void Render::CurrentScreen(String filename, GLint x, GLint y, GLsizei w, GLsizei h) {
    GLubyte * pixels = new GLubyte[4*(w-x)*(h-y)];
    glPixelZoom(-1.0f, -1.0f);
    glReadPixels(x,y,w,h,GL_RGB, GL_UNSIGNED_BYTE, pixels);
    Image::WriteImage(filename,w,h,pixels);
    delete pixels;
}

void Render::WriteImageToScreen(GLint x, GLint y, GLsizei w, GLsizei h, GLenum format, GLenum type, GLubyte * pixels) {
    glClear(GL_COLOR_BUFFER_BIT);

    //glLoadIdentity();

    GLint pos[4];
    GLboolean isValid;


    glGetIntegerv(GL_CURRENT_RASTER_POSITION, pos);
    glGetBooleanv(GL_CURRENT_RASTER_POSITION_VALID, &isValid);
    printf("isvalid: %d\n x: %d, y: %d, z: %d, w: %d\n", isValid, pos[0], pos[1], pos[2], pos[3]);
    glRasterPos4i(0,0,0,1);
    glGetIntegerv(GL_CURRENT_RASTER_POSITION, pos);
    glGetBooleanv(GL_CURRENT_RASTER_POSITION_VALID, &isValid);
    printf("isvalid: %d\n x: %d, y: %d, z: %d, w: %d\n", isValid, pos[0], pos[1], pos[2], pos[3]);
    glDrawPixels(w, h, format, type, pixels);
    glutSwapBuffers();
    glFlush();
    while(true) { /* haha */ }
}

Render::Render() {}

Render::~Render() {}

