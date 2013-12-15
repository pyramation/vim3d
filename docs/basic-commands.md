
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
    
