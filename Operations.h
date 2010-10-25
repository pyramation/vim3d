#ifndef OPERATIONS_H
#define OPERATIONS_H

#include "Packages.h"
#include "Geometry.h"

class Operations {
    public:

        static bool add(String cmdName);

        /* files, IO, etc */
        static void quit(vector<String>args); 
        static void save(vector<String>args);
        static void readOBJ(vector<String>args);
        static void write(vector<String>args);

        /* sockets */
        static void server(vector<String>args);
        static void connect(vector<String>args);
        static void send(vector<String>args);


        /* cameras */
        static void zFar(vector<String>args);
        static void zNear(vector<String>args);
        static void fovy(vector<String>args);

        static void texture(vector<String>args);
        static void normalMap(vector<String>args);

        /* drawing and curves */
        static void draw(vector<String>args);
        static void Bezier2(vector<String>args);
        static void Bezier(vector<String>args);
        static void Bspline(vector<String>args);
        static void Extrude(vector<String>args);
        static void Revolve(vector<String>args);
        static void Sweep(vector<String>args);
		static void tree(vector<String>args);
		static void cloth(vector<String>args);

        /* geometry ops */
        static void compute(vector<String>args);
		static void invertNormals(vector<String>args);
        static void bounds(vector<String>args);
		static void clearall(vector<String>args);

        /* properties */
        static void diffuse(vector<String>args);
        static void specular(vector<String>args);
        static void ambient(vector<String>args);
        static void shininess(vector<String>args);
        static void reflection(vector<String>args);
        static void refraction(vector<String>args);
        static void emission(vector<String>args);

        /* transformations */
        static void rotate(vector<String>args);
        static void translate(vector<String>args);
        static void scale(vector<String>args);
        static void add(vector<String>args);
        static void load(vector<String>args);
        static void Delete(vector<String>args);
        static void Matrix(vector<String>args);
        static void Position(vector<String>args);
        static void Up(vector<String>args);
        static void Lookat(vector<String>args);

        /* naming objects */
        static void Name(vector<String>args);


        /* terminal information and view */
        static void Vexplore(vector<String>args);
        static void Sexplore(vector<String>args);
        static void Clear(vector<String>args);
        static void Properties(vector<String>args);
        static void MatrixInfo(vector<String>args);
        static void List(vector<String>args);
        static void History(vector<String>args);

        /* Views, Channels, Lights, Geos */
        static void View(vector<String>args);
        static void ChannelOp(vector<String>args);
        static void LightOp(vector<String>args);
        static void GeoOp(vector<String>args);
        static void CameraOp(vector<String>args);
        static void ObjectOp(vector<String>args);
        static void PolyOp(vector<String>args);
        static void Children(vector<String>args);

        static void Copy(vector<String>args);
        static void Help(vector<String>args);
        static void Select(vector<String>args);

        /* Begin */
        static void BeginOp(vector<String>args);
        static void EndOp(vector<String>args);

        /* animations and references */
        static void Register(vector<String>args);
        static void Reference(vector<String>args);

        /* conversions */
        static void Path(vector<String>args);
        static void Convert(vector<String>args);
        static void Eval(vector<String>args);


        /* rendering */
        static void Render(vector<String>args);
        static void RayTrace(vector<String>args);

        static void Timeline(vector<String>args);
        static void Display(vector<String>args);
	
		static void testing(vector<String>args);
    
        Operations();
        virtual ~Operations();

};

#endif

