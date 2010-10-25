#ifndef ELEMENTS_H
#define ELEMENTS_H


#include "Channel.h"
#include "Point.h"
#include "Color.h"
#include "Texture.h"
#include "Poly.h"
#include "Image.h"

/* for contolling parameters */
enum Param {

// tranforms //
    TX=1,TY,TZ,
    RX,RY,RZ,
    SX,SY,SZ,

// 4x4 Matrix //
    M1,M2,M3,M4,
    M5,M6,M7,M8,
    M9,M10,M11,M12,

// diffuse, specular, ambient, shininess //
    KD_R,KD_G,KD_B,KD_A,
    KS_R,KS_G,KS_B,KS_A,
    KA_R,KA_G,KA_B,KA_A,
    KSHINE,


// special , number copies, history level
//    this allows you to animate:
// point numbers, channel numbers, copies, history levels //
    POINT_NUM, CHANNEL_NUM, NUM_CPS, HIST_LVL,

// mapping for textures, channels, etc
    ARRAY_INDEX, CHANNEL_INDEX, 
  
    LOOKAT, POS,

    UPDATE


};

#endif
