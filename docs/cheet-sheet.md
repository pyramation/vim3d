
# vim3D cheat-sheet

This is simply a list, more organized docs to come. 

## camera movement

tumble is default behavior when mouse movement occurs. 

press `x` to toggle zoom/tumble when mouse movement occurs

press `z` to toggle pan/tumble when mouse movement occurs


## commands
use `:` to go into command mode


adds a new object to the scene

	add <light|cam|grid|cube|sphere> 

sets the ambient property for the current object

	ambient <R> <G> <B> <A>

begin command

	begin

bezier interpolation of the current geometry

	bezier <lod>

bezier interpolation of the current geometry

	bezier2 <lod>

displays the bounding box for the selected geometry

	bounds

bspline interpolation of the current geometry

	bspline <lod>

control the camera

	cam <command>

mathematically modify a channel

	channel <expr(tx|ty|tz|sx|sy)>

perform operations of children of an object

	children <lookat>

clears the buffer

	clear

clears the polygon of the selected geo

	clearall

creates a cloth object

	cloth

computes normals for basic geometry and [melds vertex indices]

	compute [meld]

connect to a server

	connect <ip> <port>

convert geometry

	convert <obj> <type>

modify geometry parameters

	copy <target> <template>

deletes the current object

	delete

sets the diffuse property for the current object

	diffuse <R> <G> <B> <A>

display options

	display <none|default>

sets draw mode

	draw

modify emission attributes

	emission <r> <g> <b>

end command

	end

evaluate expression

	eval <some expression>

extrudes the current geometry along an axis

	extrude <x> <y> <z>

sets the current cameras fovy parameter

	fovy <amount>

modify geometry parameters

	geo <param> <value>

help documentation

	help

displays history commands of an object

	history

inverts normals

	invertNormals

modify light parameters

	light <param> <value>

lists all objects

	list

loads the specified scene into the current scene

	load <scene name>

set up a lookat relation

	lookat <obj> <refObj>

set the the 4x4 matrix for the current object

	matrix <0> <1> ... <15>

lists the matrix of the current object

	minfo

name the current object

	name <name>

add normal map

	normalmap <filename>

commands for objects

	obj

reads in an OBJ file

	open <filename>

place object along path

	path <target> <path> [<dx> <dy> <dz> <freqscalar> <timeShift> <start> <end>]

polygon

	poly <centoid|zmin|zmax|...|texture> [<s> <t>]

set the position for the current camera

	position <x> <y> <z>

lists the properties of the current object

	properties

quits the program

	quit

raytrace

	raytrace

allows objects parameters to be controlled by others

	reference <refObject> <refParam> <object> <param> [<freqScalar> <scalar> <offset> <timeShift> <start> <end>]

modify reflection attributes

	reflection <r> <g> <b>

modify refractive index

	refraction <index>

register an animation

	register <object> <param> <channel> [<freqScalar> <scalar> <offset> <timeShift> <start> <end>]

render

	render

revolves the current geometry around an arbitrary axis

	revolve <degrees> <x> <y> <z>

rotates the current object around an arbitrary axis

	rotate <degress> <x> <y> <z>

saves a scene file

	save <scene name>

scales the current object

	scale <x> <y> <z>

select an object by name

	select <objname>

sends a message over a socket connection

	send <msg>

start or stop a server

	server <start|stop>

split the window horizontally

	Sexplore

sets the shininess property for the current geometry

	shininess <amount>

sets the specular property for the current object

	specular <R> <G> <B> <A>

sweep transformation

	sweep <crossSection> [<hallway> <onpath>]

run a test

	testing

applies a texture to the current selected geometry

	texture <filename>

start the timeline

	timeline <play|stop|set> [<arg>]

translates the current object

	translate <x> <y> <z>

set the up vector for the current camera

	up <x> <y> <z>

split the window vertically

	Vexplore

changes the current window view

	view <model|channel>

writes an OBJ file

	write <filename>

sets the current cameras zFar parameter

	zfar <amount>

sets the current cameras zNear parameter

	znear <amount>



## hotkeys

Here are a list of some of the keyboard commands (not vim3d commands) to toggle various features
		
#### workflow
		
    :		enter command mode
	esc		exit command code
	u		undo
	v		redo
	m		(hold) toggles moving tool

#### view hotkeys

	w		wireframe
	o		orthogrid
	2		top view
	
#### drawing
	
	i		insert new curve
	8		toggle orthogonal snapping for drawing

#### selection

	j		select previous object
	l		select next object

#### display options (and writing to disk)

	a		toggle lights display
	t		toggle textures
	s		smooth shading	

	n		display lights as geometry
	p		points display
	5		toggle display lists on/off
	6		toggle terminal display on/off
	9		capture current frame buffer to disk
	0		toggle display mode