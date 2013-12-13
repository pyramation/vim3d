#include "Geo.h"
#include "Scene.h"
#include "vectorUtils.h"
#include "Utils.h"
#include "Term.h"

void Geo::defaults() {
    diffuse[0] = 0.8f; diffuse[1] = 0.8f; diffuse[2] = 0.8f; diffuse[3] = 1.0f; 
    specular[0] = 0.8f; specular[1] = 0.8f; specular[2] = 0.8f; specular[3] = 1.0f;
    ambient[0] = 0.2f; ambient[1] = 0.2f; ambient[2] = 0.2f; ambient[3] = 1.0f;
    emission[0] = 0.0f; emission[1] = 0.0f; emission[2] = 0.0f; emission[3] = 1.0f;
    reflection[0] = 0.0f; reflection[1] = 0.0f; reflection[2] = 0.0f;
    shininess[0] = 50.0f;
    type = GEOMETRY_TYPE;
    base=NULL;
    p=NULL;
    curve=NULL;
    tex=NULL;
    normalMap=NULL;
    group = false;
    copy = false;
    inst = false;
    disp = true;
    Target=NULL;
    Template=NULL;
    minX=minY=minZ=maxX=maxY=maxZ=cx=cy=cz=0;
    showBounds=false;
    boundingBox = NULL;
    refractiveIndex=1.0f;
}

Geo::Geo(bool instance) {
    this->defaults();
    name=String("inst");
    geoType = INSTANCE_GEO;
}

Geo::Geo() {
    this->defaults();
    String g = String("Geo");
    this->name = Scene::name(g);
    geoType = BASIC_GEO;
}

Geo::~Geo() {
    if (boundingBox) delete boundingBox;
    if (normalMap) delete normalMap;
}

void Geo::bounds() {
    Scene::toggleLights(false);
    glBegin(GL_LINES);
    glVertex3f(minX, minY, minZ); glVertex3f(maxX, minY, minZ);
    glVertex3f(minX, minY, minZ); glVertex3f(minX, maxY, minZ);
    glVertex3f(minX, minY, minZ); glVertex3f(minX, minY, maxZ);
    glVertex3f(minX, minY, maxZ); glVertex3f(minX, maxY, maxZ);
    glVertex3f(minX, minY, maxZ); glVertex3f(maxX, minY, maxZ);
    glVertex3f(maxX, minY, maxZ); glVertex3f(maxX, minY, minZ);
    glVertex3f(maxX, maxY, minZ); glVertex3f(maxX, maxY, maxZ);
    glVertex3f(maxX, maxY, minZ); glVertex3f(minX, maxY, minZ);
    glVertex3f(minX, maxY, minZ); glVertex3f(minX, maxY, maxZ);
    glVertex3f(minX, maxY, maxZ); glVertex3f(maxX, maxY, maxZ);
    glVertex3f(maxX, maxY, maxZ); glVertex3f(maxX, minY, maxZ);
    glVertex3f(maxX, maxY, minZ); glVertex3f(maxX, minY, minZ);
    glEnd();
    Scene::toggleLights(true);
}

void Geo::bound(){
    if (boundingBox) delete boundingBox;
    boundingBox = getBounds();
}
// should be different if there is a curve vs poly

void Geo::recook(){
    if (curve) {
        if (!p) {
            p = new Poly();
            addPoly(p);
        }
        p->vertices.clear();
        p->vertices = curve->points;
        p->freeList();
    } else if (p) {
        // this only happens for OBJ polys
        // the base is for when you do operations and then need to revert back to orig
        curve = new Curve(p->base);
        p->base.clear();
        p->freeList();
    }
    this->loadIdentity();

    /* select self and parse commands */
    Scene::setActive(O(this));
    for(unsigned int i=0; i < history.size(); i++) {
        Term::parse(history[i], true);
    }
    bound();
}

void Geo::addPoly(Poly * poly) { Scene::addPoly(poly); }

void Geo::addGeo(Geo * geo) { objs.push_back((Object *) geo); bound(); }

String Geo::propertyInfo() {
    char s[1024];
    sprintf(s, "name: %s\ndiffuse:  %f %f %f %f\nspecular: %f %f %f %f\nambient:  %f %f %f %f\nemission:  %f %f %f %f\nshininess:%f\nrefractive index: %f\ncentroid:(%f,%f,%f)\nbounds: (%f,%f,%f),(%f,%f,%f)\n",
            name.c_str(),
            diffuse[0], diffuse[1], diffuse[2], diffuse[3],
            specular[0], specular[1], specular[2], specular[3],
            ambient[0], ambient[1], ambient[2], ambient[3],
            emission[0], emission[1], emission[2], emission[3],
            shininess[0],
            refractiveIndex,
            cx,cy,cz,
            minX,minY,minZ, maxX,maxY,maxZ);
    String n(s);
    return n;
}

void Geo::copyPoints(vector <Point> * points) {

    if (!points->size()) {
        printf("points have no points\n");
        return;
    }

    p = new Poly();
    vector <Point>::iterator cursor;
    for (cursor = points->begin(); cursor != points->end(); cursor++) {
        p->addVertex(*cursor);
    }
    addPoly(p);

}

void Geo::scale(float x, float y, float z){
    mat4 S;
    S.a00=x; S.a01=0; S.a02=0; S.a03=0;
    S.a10=0; S.a11=y; S.a12=0; S.a13=0;
    S.a20=0; S.a21=0; S.a22=z; S.a23=0;
    S.a30=0; S.a31=0; S.a32=0; S.a33=1;
    M = S*M;
    if (p) p->scaleNormals(x,y,z);

}

void Geo::move(float yDegrees, float xDegrees, Point camDir){
    Point perpDir = Point::cross(Point(0, 1, 0), camDir);
    camDir.normalize();
    perpDir.normalize();
    camDir = camDir*yDegrees;
    perpDir = perpDir*xDegrees;

    float scale = 0.2f; //relate to distance from camera
    M.a23 += scale*(camDir.z+perpDir.z);
    M.a03 += scale*(camDir.x+perpDir.x);

}
void Geo::moveUp(float deg){
    float scale = .3f;
    M.a13 += scale*deg;
}

void Geo::display(int pass) {
    // DON'T DISPLAY WHEN EDITING since the curve is what we want, or polygon
    if ((ivMODE & EDIT_MODE)&&(selOBJ==this)) return;
    if (!disp) return; // don't display template geometry
    displayPass = pass;
    display();
}

void Geo::display() {

    glPushAttrib(GL_COLOR_MATERIAL);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLineWidth(1.0) ;
    glMultMatrixf(M.mat_array);

    if (showBounds) bounds();


    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess[0]);
    glMaterialfv(GL_FRONT, GL_EMISSION, emission);

    /* display all objects */
    for(unsigned int i=0;i<OBJS(this).size();i++){
        OBJS(this)[i]->display(displayPass);
    }


    if (p && (displayPass == GEOMETRY_TYPE)) {
        if (p->vertices.size() > 0) {
            if ((curSTATE & TEXTURE_ON) && tex) {
                tex->initializeTexture();
                tex->EnableBind();
                p->displayGeometry();
                tex->Disable();
            } else {
                p->displayGeometry();
            }
        }
    }
    /* show selection */

    if (this == selOBJ && (curSTATE & SHOW_SELECT)) {
        Scene::toggleLights(false);
        if (curSTATE & SHOW_AXIS) axis();
        GLint* glP = new GLint();
        glGetIntegerv(GL_POLYGON_MODE, glP);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        {
            /* display all objects */
            obj_t::iterator csr=objs.begin();
            for(;csr!=objs.end();csr++){
                Object * o = *csr;
                o->display(displayPass);
            }
            if(p && (displayPass == GEOMETRY_TYPE)) p->displayGeometry();

        }
        Scene::toggleLights(true);
        glPolygonMode(GL_FRONT_AND_BACK, *glP);
    }

    glPopMatrix();
    glPopAttrib();
    glColor3f(1.0,1.0,1.0);

}

BoundingBox * Geo::getBounds() {

    vector <BoundingBox*> boxes = vector<BoundingBox*>();

    BoundingBox * bounds = new BoundingBox();

    if (p) bounds->Bound(p->vertices);    

    for(unsigned int i=0; i<objs.size();i++) {
        if (isGeo(objs[i])) {
            if(G(objs[i])->boundingBox) delete G(objs[i])->boundingBox;
            G(objs[i])->boundingBox = G(objs[i])->getBounds();
            if (G(objs[i])->boundingBox) boxes.push_back(G(objs[i])->boundingBox);
        } 
    }

    for(unsigned int i=0;i<boxes.size();i++) {
        bounds->Bound(boxes[i]);
    }


    minX = bounds->min.x;
    maxX = bounds->max.x;
    minY = bounds->min.y;
    maxY = bounds->max.y;
    minZ = bounds->min.z;
    maxZ = bounds->max.z;
    cx = bounds->centroid.x;
    cy = bounds->centroid.y;
    cx = bounds->centroid.z;

    // transform
    Point Mmin = bounds->min * M;
    Point Mmax = bounds->max * M;
    Point Mcentroid = bounds->centroid* M;
    bounds->min = Mmin;
    bounds->max = Mmax;
    bounds->centroid = Mcentroid;

    return bounds;
}

Poly * Geo::getPoly() {

    vector <Poly *> polys = vector<Poly *>();

    Poly * Mp = new Poly();

    int vi, ni, ti, fi;vi=ni=ti=fi=0;
    if (p) {
        for(unsigned int v=0;v<p->vertices.size();v++) {
            Mp->addVertex(p->vertices[v]);
            vi++;
        }
        for(unsigned int t=0;t<p->textures.size();t++) {
            Mp->addVertex(p->textures[t]);
            ti++;
        }
        for(unsigned int n=0;n<p->normals.size();n++) {
            Mp->addVertex(p->normals[n]);
            ni++;
        }
        // no groups, make one
        if (!p->groups.size()) {
            for(unsigned int f=0;f<p->faces.size();f++) {
                Mp->addFace(p->faces[f]);
                fi++;
            }
            PolyGroup grp;
            grp.begin = 0;
            grp.end = Mp->faces.size()-1;
            grp.name = name;
            grp.listLoaded = false;
            grp.mat = NULL;
            Mp->groups.push_back(grp);
        } else {
            for(unsigned int g=0;g<p->groups.size();g++) {
                Mp->groups.push_back(p->groups[g]);
                for(unsigned int f=p->groups[g].begin;f<=p->groups[g].end;f++) {
                    Mp->addFace(p->faces[f]);
                    fi++;
                }
            }
        }
    }

    for(unsigned int i=0; i<objs.size();i++) {
        if (isGeo(objs[i])) {
            Poly * MMp = G(objs[i])->getPoly();
            if (MMp) polys.push_back(MMp);
        } 
    }


    for(unsigned int i=0;i<polys.size();i++) {
        // add groups
        for(unsigned int g=0;g<polys[i]->groups.size();g++) {
            polys[i]->groups[g].begin += fi;
            polys[i]->groups[g].end += fi;
            Mp->groups.push_back(polys[i]->groups[g]);
        }
        /* add faces with updated indices */
        for(unsigned int f=0;f<polys[i]->faces.size();f++) {
            Face fsc = polys[i]->faces[f];
            for(unsigned int v=0;v<fsc.vi.size();v++) fsc.vi[v]+= vi;
            for(unsigned int n=0;n<fsc.ni.size();n++) fsc.ni[n]+= ni;
            for(unsigned int t=0;t<fsc.ti.size();t++) fsc.ti[t]+= ti;
            Mp->addFace(fsc);
            fi++;
        }
        /* add the data */
        for(unsigned int v=0;v<polys[i]->vertices.size();v++) {
            Mp->addVertex(polys[i]->vertices[v]);
            vi++;
        }
        for(unsigned int t=0;t<polys[i]->textures.size();t++) {
            Mp->addVertex(polys[i]->textures[t]);
            ti++;
        }
        for(unsigned int n=0;n<polys[i]->normals.size();n++) {
            Mp->addVertex(polys[i]->normals[n]);
            ni++;
        }
        Poly * deleteMe = polys[i];
        delete deleteMe;
    }


    /* transform the points */
    for(unsigned int i=0;i<Mp->vertices.size();i++) {
        Vertex Mv = Mp->vertices[i] * M;
        Mp->vertices[i] = Mv;
    }
    /*
       mat4 M_inv = M;
       M_inv.a03 = 0.0f;
       M_inv.a13 = 0.0f;
       M_inv.a23 = 0.0f;
       for(unsigned int i=0;i<Mp->vertices.size();i++) {
       Normal M_inv_n = Mp->normals[i] * M_inv;
       }
     */

    return Mp;
}

Point Geo::pos() { return Point(M.a03, M.a13, M.a23); }


void Geo::writeMTL(String matLibName) {
    Material mat;
    mat.name = name + String("_mtl");
    copyMaterial(mat);
    FILE * fd = fopen(matLibName.c_str(), "w");
    if (!fd) return;
    Utils::writeMTL(fd, mat);
    fclose(fd);
}

void Geo::writeGeometry(String filename) {

    Poly * Mp = getPoly();
    Mp->writeOBJ(filename);
    delete Mp;

}

bool Geo::isCube(Geo * geo) { return (geo->geoType == CUBE_GEO); }
bool Geo::isSphere(Geo * geo) { return (geo->geoType == SPHERE_GEO); }
bool Geo::isGrid(Geo * geo) { return (geo->geoType == GRID_GEO); }

/* getIntersection returns where on this geometry object a ray intersects */

Intersection Geo::getIntersection(Ray ray, float &closestD, mat4 Mat){
    Point hit = Point(0.0f,0.0f,0.0f);
    Point coords = Point(0.0f,0.0f,0.0f);

    Intersection intersection = Intersection();
    intersection.eye = ray.pos;
    intersection.geo = NULL;

    // inverse transform the ray!

    //mat4 PM = Mat * M;
    mat4 PM = Mat;


    mat4 invM = invert(invM, PM);
    mat4 T = PM;
    T.a03 = 0;
    T.a13 = 0;
    T.a23 = 0;
    mat4 invD = invert(invD, T);
    ray.pos = ray.pos * invM;
    ray.dir = ray.dir * invD;

    /* more inverted rays */
    Ray r = ray;
    mat4 iM = invert(iM, M);
    r.pos = r.pos * iM;

    mat4 mT = M;
    mT.a03 = 0; mT.a13 = 0; mT.a23 = 0;
    mat4 iD = invert(iD, mT);
    r.dir = r.dir * iD;
    mT = Transform::invTransposeForNormals(M);

    /* bounding box test 
     */
    Point nil;
    Point min = Point(minX,minY,minZ);
    Point max = Point(maxX,maxY,maxZ);
    if (!Ray::intersectBox(r,min,max,&nil)) return intersection;
    //if (!Ray::intersectBox(boundingBox->min,boundingBox->max, ray.pos, ray.dir, &nil)) return intersection;

    if (isSphere(this)) {
        Point center = Point(0,0,0);


        if (Ray::intersectSphere(r, ((Sphere*)this)->radius, center, &hit)) {
            float dist = (hit - r.pos).length();
            if(dist<closestD){
                closestD=dist;
                copyMaterial(intersection.mat);
                intersection.hit = hit * M;
                intersection.geo = this;
                intersection.n = (hit - center) * mT;
                intersection.n.normalize();
            }
        }
    } else if(p){
        for (unsigned int i = 0; i<p->faces.size(); i++) {
            if (p->faces[i].vi.size()==4||p->faces[i].vi.size()==3) {
                vector <Vertex> facePoints = vector<Vertex>();
                for(unsigned int k=0; k<p->faces[i].vi.size();k++) {
                    facePoints.push_back(p->vertices[p->faces[i].vi[k]-1]);
                    //facePoints.push_back(p->vertices[p->faces[i].vi[k]-1]*M);
                }
                int check;
                if (p->faces[i].vi.size()==4) {
                    check = Ray::intersectRectangle(r,facePoints, &hit, &coords);
                } else {
                    check = Ray::intersectTriangle(r,facePoints, &hit, &coords);
                }

                if (check>0) {
                    float dist = (hit - r.pos).length();
                    if(dist < closestD){
                        closestD=dist;
                        copyMaterial(intersection.mat);
                        if (p->groups.size() > 0) {
                            for(unsigned int g=0;g<p->groups.size();g++) {
                                if (p->groups[g].begin <= i && i <= p->groups[g].end) {
                                    if (p->groups[g].mat) 
                                        intersection.mat = *p->groups[g].mat;
                                    break;
                                }
                            }
                        }
                        intersection.hit = hit * M;
                        intersection.f = p->faces[i];
                        intersection.geo = this;

                        // gourad normal here!
                        if (p->faces[i].ni.size()==p->faces[i].vi.size()) {
                            Normal nrm(0.0f,0.0f,0.0f);
                            if (check==2||check==1) {
                                nrm += p->normals[p->faces[i].ni[0]-1]*coords.z;
                                nrm += p->normals[p->faces[i].ni[1]-1]*coords.y;
                                nrm += p->normals[p->faces[i].ni[2]-1]*coords.x;
                            } else if (check==3) {
                                nrm += p->normals[p->faces[i].ni[0]-1]*coords.x;
                                nrm += p->normals[p->faces[i].ni[2]-1]*coords.z;
                                nrm += p->normals[p->faces[i].ni[3]-1]*coords.y;
                            }

                            /* normals are normalized  */
                            nrm.normalize();
                            intersection.n = nrm* mT;
                        } else { 
                            if (p->faces[i].ni.size()>0)intersection.n = p->normals[p->faces[i].ni[0]-1] * mT;
                        }

                        /* if textured */
                        if (p->faces[i].ti.size()==p->faces[i].vi.size()) {
                            TexCoord tc(0.0f,0.0f,0.0f);
                            if (check==2||check==1) {
                                tc += p->textures[p->faces[i].ti[0]-1]*coords.z;
                                tc += p->textures[p->faces[i].ti[1]-1]*coords.y;
                                tc += p->textures[p->faces[i].ti[2]-1]*coords.x;
                            } else if (check==3) {
                                tc += p->textures[p->faces[i].ti[0]-1]*coords.x;
                                tc += p->textures[p->faces[i].ti[2]-1]*coords.z;
                                tc += p->textures[p->faces[i].ti[3]-1]*coords.y;
                            }
                            intersection.t = tc;
                        }

                        /*
                           if (normalMap && p->faces[i].ti.size()==p->faces[i].vi.size()) {
                           TexCoord tc(0.0f,0.0f,0.0f);
                           if (check==2||check==1) {
                           tc += p->textures[p->faces[i].ti[0]-1]*coords.z;
                           tc += p->textures[p->faces[i].ti[1]-1]*coords.y;
                           tc += p->textures[p->faces[i].ti[2]-1]*coords.x;
                           } else if (check==3) {
                           tc += p->textures[p->faces[i].ti[0]-1]*coords.x;
                           tc += p->textures[p->faces[i].ti[2]-1]*coords.z;
                           tc += p->textures[p->faces[i].ti[3]-1]*coords.y;
                           }
                           Normal fromMap(0.0f,0.0f,0.0f);
                           unsigned int width = normalMap->w;
                           unsigned int height = normalMap->h;
                           unsigned int xCoord = int((width - 1) * tc.x) % width; 
                           unsigned int yCoord = int((height - 1) * tc.y) % height; 
                           fromMap.x = normalMap->pixels[4*(xCoord+yCoord*width)+1]/255.0f;
                           fromMap.y = normalMap->pixels[4*(xCoord+yCoord*width)+0]/255.0f;
                           fromMap.z = normalMap->pixels[4*(xCoord+yCoord*width)+2]/255.0f;
                           intersection.n += fromMap;
                           intersection.n.normalize();

                           }
                         */
                    }		
                }
            }
        }
    }

    /* RECURSIVE CALL */
    if (objs.size() > 0) {
        for(unsigned int i=0; i<objs.size();i++) {
            // only look at objects that have disp set to true
            if (isGeo(objs[i]) && G(objs[i])->disp) {
                float dist = closestD;
                Intersection newI = G(objs[i])->getIntersection(ray, dist, M);
                if (dist<closestD) {
                    closestD=dist;
                    intersection=newI;
                }
            }
        }
    }
    intersection.hit = intersection.hit * Mat;
    intersection.n = intersection.n * Transform::invTransposeForNormals(Mat);
    intersection.n.normalize();
    return intersection;
}

void Geo::copyMaterial(Material & mat) {
    for(int i=0;i<4;i++) {
        mat.specular[i] = specular[i];
        mat.ambient[i] = ambient[i];
        mat.diffuse[i] = diffuse[i];
        mat.emission[i] = emission[i];
    }

    mat.shininess = shininess[0];

    for(int i=0;i<3;i++) {
        mat.reflection[i] = reflection[i];
    } 

    mat.refractiveIndex = refractiveIndex;
    mat.texture = tex;
    mat.normalMap = normalMap;

}
