/*********************************************************************NVMH1****
File:
nv_math.h

Copyright (C) 1999, 2002 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/
#ifndef _nv_math_h_
#define _nv_math_h_

#ifndef _nv_mathdecl_h_
#include "nv_mathdecl.h"
#endif // _nv_mathdecl_h_

#include <assert.h>
#include <math.h>

#ifdef _WIN32
#include <limits>
#else
#include <limits.h>
#endif

#ifdef MACOS
#define sqrtf sqrt
#define sinf sin
#define cosf cos
#define tanf tan
#endif

#include <memory.h>
#include <stdlib.h>
#include <float.h>

#define nv_zero			      float(0)
#define nv_zero_5             float(0.5)
#define nv_one			      float(1.0)
#define nv_two			      float(2)
#define nv_half_pi            float(3.14159265358979323846264338327950288419716939937510582 * 0.5)
#define nv_quarter_pi         float(3.14159265358979323846264338327950288419716939937510582 * 0.25)
#define nv_pi			      float(3.14159265358979323846264338327950288419716939937510582)
#define nv_two_pi			  float(3.14159265358979323846264338327950288419716939937510582 * 2.0)
#define nv_oo_pi			  nv_one / nv_pi
#define nv_oo_two_pi	      nv_one / nv_two_pi
#define nv_oo_255   	      nv_one / float(255)
#define nv_oo_128   	      nv_one / float(128)
#define nv_to_rad             nv_pi / float(180)
#define nv_to_deg             float(180) / nv_pi
#define nv_eps		          float(10e-6)
#define nv_double_eps	      float(10e-6) * nv_two
#define nv_big_eps            float(10e-6)
#define nv_small_eps          float(10e-2)

struct vec2;
struct vec2t;
struct vec3;
struct vec3t;
struct vec4;
struct vec4t;

#ifndef _nv_algebra_h_
#include "nv_algebra.h"
#endif // _nv_algebra_h_

#endif //_nv_math_h_
