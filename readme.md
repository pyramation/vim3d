# vim3d

## built and run

#### 1 To get started clone and enter repo

	git clone https://github.com/pyramation/vim3d.git
	cd vim3d

#### 2 call `make` to build the binary

    make
    
#### 3 Run `vim3d`
    
    ./vim3d

## usage

    vim3d [sceneFile]

there are some examples in the `scenes/` folder, check those out to get started.

![roomba](https://91c5ecdb0164d5b53221-86a2c79f9eb64e3688d1dc9e722e6715.ssl.cf1.rackcdn.com/6e5c0f9f3d19a4e63d00eae1bad38bfb-320x212.jpg)

    scenes/
    ├── animate
    ├── bscene
    ├── colorRefs
    ├── demoday
    ├── demoday2
    ├── demoday3
    ├── demoday4
    ├── gyroscope
    ├── lightsScene
    ├── lookat
    ├── reflections
    ├── space
    └── wolf

example:
    
    vim3d scenes/lightsScene


## quickstart / mini tutorial

open vim3d in the shell

	vim3d

![vim3d](https://91c5ecdb0164d5b53221-86a2c79f9eb64e3688d1dc9e722e6715.ssl.cf1.rackcdn.com/96a60a67b308017db901f97c19ac07ef-320x188.jpg)

add camera
	
	add camera
	position 100 100 100
	
hit `i` to go into insert mode
	
	i

![lights](https://91c5ecdb0164d5b53221-86a2c79f9eb64e3688d1dc9e722e6715.ssl.cf1.rackcdn.com/0b7d38ea91417f002eb5436cf6871c43-480x266.jpg)

you can toggle orthogonal mode with `8`

While in insert mode, hit the `l` key to cycle through different curve interpolation previews.

When finished drawing, press `enter`, and then enter command mode by pressing `:`

to interpolate specify level of detail as the argument to `bezier`, `bezier2`, or `bspline`

	bezier 5

hit `u` to undo and try these:

	bezier2 5
	bspline 5

name the object

	name mycurve	

	
now add a add grid, and extrude it.
	
	add grid 0 0 1 4
	extrude 0 0 0 10
	name mygrid
	
now copy your rectangular box to the curve that you drew!
	
	copy mygrid mycurve geo align

You can do animations, add lights, and also sorts of super cool transformations. Read the docs and check out the examples!

# Docs

More docs to come, the basics are listed below, for more syntax, and how-to: http://vim3D.com


### hotkeys

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


## basic commands

### Scenes

#### Load a scene file

	load <sceneName>

#### Write out a scene file

	save <sceneName>

### Geometry Objects

#### add a cube an assign it a name

	add cube
	name Geo0

#### Basic Objects in Vim3D

	add grid [<divisionsX> <divisionsZ> <step> <dx> <dz>]
	add sphere
	add cube
	add circle

### Geometry Files

#### Open an obj file

	file <objName>

There are shortcut commands for this:

	o <objName>

#### Write out an obj file

First select the object of interest, then type

	write <objName>

#### Write out a hierarchical group (recursive) as a single .obj

First select the object of interest, then type
	geo write <somelobj>
	
### Channels

#### Load in a channel for animation

	load channel <Channelfile>

#### Write out an channel file

First select the channel object of interest, then type

	write <chanName>

### insert mode

[http://www.vim3d.com/pages/13/insert_mode/](http://www.vim3d.com/pages/13/insert_mode/)

### curves

[http://www.vim3d.com/pages/25/curves/](http://www.vim3d.com/pages/25/curves/)

While in insert mode, hit the l key to cycle through different curve interpolation previews. When finished drawing, to interpolate specify one of the following commands: Interpolations

	bezier <lod>
	bezier2 <lod>
	bspline <lod>

### textures

[http://www.vim3d.com/pages/14/texturing/](http://www.vim3d.com/pages/14/texturing/)

Multiply texture coordinates

	poly st <s> <t>

Generate texture coordinates
	
	poly texture <s> <t>

Texture an Object

	texture <filename>
	
