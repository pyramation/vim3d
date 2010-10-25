/*********************************************************************NVMH1****
File:
nv_algebra.h

Copyright (C) 1999, 2002 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments: 
This file has been modified by Aner Ben-Artzi for the purposes of cs4160


******************************************************************************/

#ifndef _nv_algebra_h_
#define _nv_algebra_h_




struct DECLSPEC_NV_MATH vec2
{
	vec2() { }
    vec2(float x, float y) : x(x), y(y) { }
    vec2(const float* xy) : x(xy[0]), y(xy[1]) { }
	vec2(const vec2& u) : x(u.x), y(u.y) { }
	vec2(const vec3&);

    bool operator==(const vec2 & u) const
    {
        return (u.x == x && u.y == y) ? true : false;
    }

    bool operator!=(const vec2 & u) const
    {
        return !(*this == u );
    }

    vec2 & operator*=(const float & lambda)
    {
        x*= lambda;
        y*= lambda;
        return *this;
    }

    vec2 & operator-=(const vec2 & u)
    {
        x-= u.x;
        y-= u.y;
        return *this;
    }

    vec2 & operator+=(const vec2 & u)
    {
        x+= u.x;
        y+= u.y;
        return *this;
    }

    float & operator[](int i)
    {
        return vec_array[i];
    }

    const float operator[](int i) const
    {
        return vec_array[i];
    }

    union {
        struct {
            float x,y;          // standard names for components
        };
        struct {
            float s,t;          // standard names for components
        };
        float vec_array[2];     // array access
    };
};

inline const vec2 operator+(const vec2& u, const vec2& v)
{
	return vec2(u.x + v.x, u.y + v.y);
}

inline const vec2 operator-(const vec2& u, const vec2& v)
{
    return vec2(u.x - v.x, u.y - v.y);
}

inline const vec2 operator*(const float s, const vec2& u)
{
	return vec2(s * u.x, s * u.y);
}

inline const vec2 operator/(const vec2& u, const float s)
{
	return vec2(u.x / s, u.y / s);
}

inline const vec2 operator*(const vec2&u, const vec2&v)
{
	return vec2(u.x * v.x, u.y * v.y);
}

struct DECLSPEC_NV_MATH vec3
{
	vec3() { }
    vec3(float x, float y, float z) : x(x), y(y), z(z) { }
    vec3(const float* xyz) : x(xyz[0]), y(xyz[1]), z(xyz[2]) { }
	vec3(const vec2& u) : x(u.x), y(u.y), z(1.0f) { }
	vec3(const vec3& u) : x(u.x), y(u.y), z(u.z) { }
	vec3(const vec4&);

    bool operator==(const vec3 & u) const
    {
        return (u.x == x && u.y == y && u.z == z) ? true : false;
    }

    bool operator!=( const vec3& rhs ) const
    {
        return !(*this == rhs );
    }

    vec3 & operator*=(const float & lambda)
    {
        x*= lambda;
        y*= lambda;
        z*= lambda;
        return *this;
    }

    vec3 operator - () const
	{
		return vec3(-x, -y, -z);
	}

    vec3 & operator-=(const vec3 & u)
    {
        x-= u.x;
        y-= u.y;
        z-= u.z;
        return *this;
    }

    vec3 & operator+=(const vec3 & u)
    {
        x+= u.x;
        y+= u.y;
        z+= u.z;
        return *this;
    }
	float normalize();
	float sq_norm() const { return x * x + y * y + z * z; }
	float norm() const { return sqrtf(sq_norm()); }

    float & operator[](int i)
    {
        return vec_array[i];
    }

    const float operator[](int i) const
    {
        return vec_array[i];
    }

    union {
        struct {
            float x,y,z;        // standard names for components
        };
        struct {
            float s,t,r;        // standard names for components
        };
        float vec_array[3];     // array access
    };
};

inline const vec3 operator+(const vec3& u, const vec3& v)
{
	return vec3(u.x + v.x, u.y + v.y, u.z + v.z);
}

inline const vec3 operator-(const vec3& u, const vec3& v)
{
    return vec3(u.x - v.x, u.y - v.y, u.z - v.z);
}

inline const vec3 operator^(const vec3& u, const vec3& v)
{
    return vec3(u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x);
}

inline const vec3 operator*(const float s, const vec3& u)
{
	return vec3(s * u.x, s * u.y, s * u.z);
}

inline const vec3 operator/(const vec3& u, const float s)
{
	return vec3(u.x / s, u.y / s, u.z / s);
}

inline const vec3 operator*(const vec3& u, const vec3& v)
{
	return vec3(u.x * v.x, u.y * v.y, u.z * v.z);
}

inline vec2::vec2(const vec3& u)
{
	float k = 1 / u.z;
	x = k * u.x;
	y = k * u.y;
}

struct DECLSPEC_NV_MATH vec4
{
	vec4() { }
    vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) { }
    vec4(const float* xyzw) : x(xyzw[0]), y(xyzw[1]), z(xyzw[2]), w(xyzw[3]) { }
	vec4(const vec3& u) : x(u.x), y(u.y), z(u.z), w(1.0f) { }
	vec4(const vec4& u) : x(u.x), y(u.y), z(u.z), w(u.w) { }

    bool operator==(const vec4 & u) const
    {
        return (u.x == x && u.y == y && u.z == z && u.w == w) ? true : false;
    }

    bool operator!=( const vec4& rhs ) const
    {
        return !(*this == rhs );
    }


    vec4 & operator*=(const float & lambda)
    {
        x*= lambda;
        y*= lambda;
        z*= lambda;
        w*= lambda;
        return *this;
    }

    vec4 & operator-=(const vec4 & u)
    {
        x-= u.x;
        y-= u.y;
        z-= u.z;
        w-= u.w;
        return *this;
    }

    vec4 & operator+=(const vec4 & u)
    {
        x+= u.x;
        y+= u.y;
        z+= u.z;
        w+= u.w;
        return *this;
    }

    vec4 operator - () const
	{
		return vec4(-x, -y, -z, -w);
	}

    float & operator[](int i)
    {
        return vec_array[i];
    }

    const float operator[](int i) const
    {
        return vec_array[i];
    }

    union {
        struct {
            float x,y,z,w;          // standard names for components
        };
        struct {
            float s,t,r,q;          // standard names for components
        };
        float vec_array[4];     // array access
    };
};

inline const vec4 operator+(const vec4& u, const vec4& v)
{
	return vec4(u.x + v.x, u.y + v.y, u.z + v.z, u.w + v.w);
}

inline const vec4 operator-(const vec4& u, const vec4& v)
{
    return vec4(u.x - v.x, u.y - v.y, u.z - v.z, u.w - v.w);
}

inline const vec4 operator*(const float s, const vec4& u)
{
	return vec4(s * u.x, s * u.y, s * u.z, s * u.w);
}

inline const vec4 operator/(const vec4& u, const float s)
{
	return vec4(u.x / s, u.y / s, u.z / s, u.w / s);
}

inline const vec4 operator*(const vec4& u, const vec4& v)
{
	return vec4(u.x * v.x, u.y * v.y, u.z * v.z, u.w * v.w);
}

inline vec3::vec3(const vec4& u)
{
	x = u.x;
	y = u.y;
	z = u.z;
}

// quaternion
struct quat;  

/*
    for all the matrices...a<x><y> indicates the element at row x, col y

    For example:
    a01 <-> row 0, col 1 
*/

struct DECLSPEC_NV_MATH mat3
{
    mat3();
    mat3(const float * array);
    mat3(const mat3 & M);
    mat3( const float& f0,  const float& f1,  const float& f2,  
          const float& f3,  const float& f4,  const float& f5,  
          const float& f6,  const float& f7,  const float& f8 )
  		  : a00( f0 ), a10( f1 ), a20( f2 ), 
            a01( f3 ), a11( f4 ), a21( f5 ),
  		    a02( f6 ), a12( f7 ), a22( f8) { }

    const vec3 col(const int i) const
    {
        return vec3(&mat_array[i * 3]);
    }

    const vec3 operator[](int i) const
    {
        return vec3(mat_array[i], mat_array[i + 3], mat_array[i + 6]);
    }

    const float& operator()(const int& i, const int& j) const
    {
        return mat_array[ j * 3 + i ];
    }

    float& operator()(const int& i, const int& j)
    {
        return  mat_array[ j * 3 + i ];
    }

    void set_row(int i, const vec3 & v)
    {
        mat_array[i] = v.x;
        mat_array[i + 3] = v.y;
        mat_array[i + 6] = v.z;
    }

	void set_col(int i, const vec3 & v)
	{
        mat_array[i * 3] = v.x;
        mat_array[i * 3 + 1] = v.y;
        mat_array[i * 3 + 2] = v.z;
	}

    void set_rot(const float & theta, const vec3 & v);
    void set_rot(const vec3 & u, const vec3 & v);

    union {
        struct {
            float a00, a10, a20;        // standard names for components
            float a01, a11, a21;        // standard names for components
            float a02, a12, a22;        // standard names for components
        };
        float mat_array[9];     // array access
    };
};

const vec3 operator*(const mat3&, const vec3&);
const vec3 operator*(const vec3&, const mat3&);

struct DECLSPEC_NV_MATH mat4
{
    mat4();
    mat4(const float * array);
    mat4(const mat4 & M);

    mat4( const float& f0,  const float& f1,  const float& f2,  const float& f3,
  		  const float& f4,  const float& f5,  const float& f6,  const float& f7,
  		  const float& f8,  const float& f9,  const float& f10, const float& f11,
  		  const float& f12, const float& f13, const float& f14, const float& f15 )
  		  : a00( f0 ), a10( f1 ), a20( f2 ), a30( f3 ),
  		    a01( f4 ), a11( f5 ), a21( f6 ), a31( f7 ),
  		    a02( f8 ), a12( f9 ), a22( f10), a32( f11),
			a03( f12), a13( f13), a23( f14), a33( f15) { }
 
    const vec4 col(const int i) const
    {
        return vec4(&mat_array[i * 4]);
    }
    
    const vec4 operator[](const int& i) const
    {
        return vec4(mat_array[i], mat_array[i + 4], mat_array[i + 8], mat_array[i + 12]);
    }
   
    const float& operator()(const int& i, const int& j) const
    {
        return mat_array[ j * 4 + i ];
    }

    float& operator()(const int& i, const int& j)
    {
        return  mat_array[ j * 4 + i ];
    }

    void set_col(int i, const vec4 & v)
    {
        mat_array[i * 4] = v.x;
        mat_array[i * 4 + 1] = v.y;
        mat_array[i * 4 + 2] = v.z;
        mat_array[i * 4 + 3] = v.w;
    }

    void set_row(int i, const vec4 & v)
    {
        mat_array[i] = v.x;
        mat_array[i + 4] = v.y;
        mat_array[i + 8] = v.z;
        mat_array[i + 12] = v.w;
    }

    mat3 & get_rot(mat3 & M) const;
    quat & get_rot(quat & q) const;
    void set_rot(const quat & q);
    void set_rot(const mat3 & M);
    void set_rot(const float & theta, const vec3 & v);
    void set_rot(const vec3 & u, const vec3 & v);

    void set_translation(const vec3 & t);
    vec3 & get_translation(vec3 & t) const;

	mat4 operator*(const mat4&) const;

    union {
        struct {
            float a00, a10, a20, a30;   // standard names for components
            float a01, a11, a21, a31;   // standard names for components
            float a02, a12, a22, a32;   // standard names for components
            float a03, a13, a23, a33;   // standard names for components
        };
        struct {
            float _11, _12, _13, _14;   // standard names for components
            float _21, _22, _23, _24;   // standard names for components
            float _31, _32, _33, _34;   // standard names for components
            float _41, _42, _43, _44;   // standard names for components
        };
        union {
            struct {
                float b00, b10, b20, p; // standard names for components
                float b01, b11, b21, q; // standard names for components
                float b02, b12, b22, r; // standard names for components
                float x, y, z, w;       // standard names for components
            };
        };
        float mat_array[16];     // array access
    };
};

const vec4 operator*(const mat4&, const vec4&);
const vec4 operator*(const vec4&, const mat4&);

// quaternion
struct DECLSPEC_NV_MATH quat {
public:
	quat(float x = 0, float y = 0, float z = 0, float w = 1);
	quat(const quat& quat);
	quat(const vec3& axis, float angle);
	quat(const mat3& rot);
	quat& operator=(const quat& quat);
	quat operator-()
	{
		return quat(-x, -y, -z, -w);
	}
	quat Inverse();
	void Normalize();
	void FromMatrix(const mat3& mat);
	void ToMatrix(mat3& mat) const;
	quat& operator*=(const quat& quat);
	static const quat Identity;
	float& operator[](int i) { return comp[i]; }
	const float operator[](int i) const { return comp[i]; }
	union {
		struct {
			float x, y, z, w;
		};
		float comp[4];
	};
};
const quat operator*(const quat&, const quat&);
extern quat & conj(quat & p, const quat & q);
extern quat & add_quats(quat & p, const quat & q1, const quat & q2);
extern float dot(const quat & p, const quat & q);
extern quat & dot(float s, const quat & p, const quat & q);
extern quat & slerp_quats(quat & p, float s, const quat & q1, const quat & q2);
extern quat & axis_to_quat(quat & q, const vec3 & a, const float phi);
extern mat3 & quat_2_mat(mat3 &M, const quat &q );
extern quat & mat_2_quat(quat &q,const mat3 &M);

// constant algebraic values
const float array16_id[] =        { nv_one, nv_zero, nv_zero, nv_zero,
                                        nv_zero, nv_one, nv_zero, nv_zero,
                                        nv_zero, nv_zero, nv_one, nv_zero,
                                        nv_zero, nv_zero, nv_zero, nv_one};

const float array16_null[] =      { nv_zero, nv_zero, nv_zero, nv_zero,
                                        nv_zero, nv_zero, nv_zero, nv_zero,
                                        nv_zero, nv_zero, nv_zero, nv_zero,
                                        nv_zero, nv_zero, nv_zero, nv_zero};

const float array16_scale_bias[] = { nv_zero_5, nv_zero,   nv_zero,   nv_zero,
                                         nv_zero,   nv_zero_5, nv_zero,   nv_zero,
                                         nv_zero,   nv_zero,   nv_zero_5, nv_zero,
                                         nv_zero_5, nv_zero_5, nv_zero_5, nv_one};

const float array9_id[] =         { nv_one, nv_zero, nv_zero,
                                        nv_zero, nv_one, nv_zero,
                                        nv_zero, nv_zero, nv_one};


const vec2      vec2_null(nv_zero,nv_zero);
const vec4      vec4_one(nv_one,nv_one,nv_one,nv_one);
const vec3      vec3_one(nv_one,nv_one,nv_one);
const vec3      vec3_null(nv_zero,nv_zero,nv_zero);
const vec3      vec3_x(nv_one,nv_zero,nv_zero);
const vec3      vec3_y(nv_zero,nv_one,nv_zero);
const vec3      vec3_z(nv_zero,nv_zero,nv_one);
const vec3      vec3_neg_x(-nv_one,nv_zero,nv_zero);
const vec3      vec3_neg_y(nv_zero,-nv_one,nv_zero);
const vec3      vec3_neg_z(nv_zero,nv_zero,-nv_one);
const vec4      vec4_null(nv_zero,nv_zero,nv_zero,nv_zero);
const vec4      vec4_x(nv_one,nv_zero,nv_zero,nv_zero);
const vec4      vec4_neg_x(-nv_one,nv_zero,nv_zero,nv_zero);
const vec4      vec4_y(nv_zero,nv_one,nv_zero,nv_zero);
const vec4      vec4_neg_y(nv_zero,-nv_one,nv_zero,nv_zero);
const vec4      vec4_z(nv_zero,nv_zero,nv_one,nv_zero);
const vec4      vec4_neg_z(nv_zero,nv_zero,-nv_one,nv_zero);
const vec4      vec4_w(nv_zero,nv_zero,nv_zero,nv_one);
const vec4      vec4_neg_w(nv_zero,nv_zero,nv_zero,-nv_one);
const quat      quat_id(nv_zero,nv_zero,nv_zero,nv_one);
const mat4      mat4_id(array16_id);
const mat3      mat3_id(array9_id);
const mat4      mat4_null(array16_null);
const mat4      mat4_scale_bias(array16_scale_bias);

// normalizes a vector and return a reference of itself
extern vec3 & normalize(vec3 & u);
extern vec4 & normalize(vec4 & u);

// Computes the squared magnitude
inline float nv_sq_norm(const vec3 & n)
{ return n.x * n.x + n.y * n.y + n.z * n.z; }

inline float nv_sq_norm(const vec4 & n)
{ return n.x * n.x + n.y * n.y + n.z * n.z + n.w * n.w; }

// Computes the magnitude
inline float nv_norm(const vec3 & n)
{ return sqrtf(nv_sq_norm(n)); }

inline float nv_norm(const vec4 & n)
{ return sqrtf(nv_sq_norm(n)); }

// computes the cross product ( v cross w) and stores the result in u
// i.e.     u = v cross w
extern vec3 & cross(vec3 & u, const vec3 & v, const vec3 & w);

// computes the dot product ( v dot w) and stores the result in u
// i.e.     u = v dot w
extern float & dot(float & u, const vec3 & v, const vec3 & w);
extern float dot(const vec3 & v, const vec3 & w);
extern float & dot(float & u, const vec4 & v, const vec4 & w);
extern float dot(const vec4 & v, const vec4 & w);
extern float & dot(float & u, const vec3 & v, const vec4 & w);
extern float dot(const vec3 & v, const vec4 & w);
extern float & dot(float & u, const vec4 & v, const vec3 & w);
extern float dot(const vec4 & v, const vec3 & w);

// compute the reflected vector R of L w.r.t N - vectors need to be 
// normalized
//
//                R     N     L
//                  _       _
//                 |\   ^   /|
//                   \  |  /
//                    \ | /
//                     \|/
//                      +
extern vec3 & reflect(vec3 & r, const vec3 & n, const vec3 & l);

// Computes u = v * lambda + u
extern vec3 & madd(vec3 & u, const vec3 & v, const float & lambda);
// Computes u = v * lambda
extern vec3 & mult(vec3 & u, const vec3 & v, const float & lambda);
// Computes u = v * w
extern vec3 & mult(vec3 & u, const vec3 & v, const vec3 & w);
// Computes u = v + w
extern vec3 & add(vec3 & u, const vec3 & v, const vec3 & w);
// Computes u = v - w
extern vec3 & sub(vec3 & u, const vec3 & v, const vec3 & w);

// Computes u = u * s
extern vec3 & scale(vec3 & u, const float s);
extern vec4 & scale(vec4 & u, const float s);

// Computes u = M * v
extern vec3 & mult(vec3 & u, const mat3 & M, const vec3 & v);
extern vec4 & mult(vec4 & u, const mat4 & M, const vec4 & v);

// Computes u = v * M
extern vec3 & mult(vec3 & u, const vec3 & v, const mat3 & M);
extern vec4 & mult(vec4 & u, const vec4 & v, const mat4 & M);

// Computes u = M(4x4) * v and divides by w
extern vec3 & mult_pos(vec3 & u, const mat4 & M, const vec3 & v);
// Computes u = M(4x4) * v
extern vec3 & mult_dir(vec3 & u, const mat4 & M, const vec3 & v);
// Computes u = M(4x4) * v and does not divide by w (assumed to be 1)
extern vec3 & mult(vec3& u, const mat4& M, const vec3& v);

// Computes u = v * M(4x4) and divides by w
extern vec3 & mult_pos(vec3 & u, const vec3 & v, const mat4 & M);
// Computes u = v * M(4x4)
extern vec3 & mult_dir(vec3 & u, const vec3 & v, const mat4 & M);
// Computes u = v * M(4x4) and does not divide by w (assumed to be 1)
extern vec3 & mult(vec3& u, const vec3& v, const mat4& M);

// Computes A += B
extern mat4 & add(mat4 & A, const mat4 & B);
extern mat3 & add(mat3 & A, const mat3 & B);

// Computes C = A * B
extern mat4 & mult(mat4 & C, const mat4 & A, const mat4 & B);
extern mat3 & mult(mat3 & C, const mat3 & A, const mat3 & B);

// Computes B = Transpose(A)
//       T
//  B = A
extern mat3 & transpose(mat3 & B, const mat3 & A);
extern mat4 & transpose(mat4 & B, const mat4 & A);
extern mat3 & transpose(mat3 & B);
extern mat4 & transpose(mat4 & B);

// Computes B = inverse(A)
//       -1
//  B = A
extern mat4 & invert(mat4 & B, const mat4 & A);
extern mat3 & invert(mat3 & B, const mat3 & A);

// Computes B = inverse(A)
//                                       T  T
//                   (R t)             (R -R t)
// assuming that A = (0 1) so that B = (0    1)
//  B = A
extern mat4 & invert_rot_trans(mat4 & B, const mat4 & A);

extern mat4 & frustum(mat4 & M, const float l, const float r, const float b, 
               const float t, const float n, const float f);

extern mat4 & perspective(mat4 & M, const float fovy, const float aspect, const float n, const float f);

// quaternion
extern quat & normalize(quat & p);
extern quat & conj(quat & p);
extern quat & conj(quat & p, const quat & q);
extern quat & add_quats(quat & p, const quat & q1, const quat & q2);
extern quat & axis_to_quat(quat & q, const vec3 & a, const float phi);
extern mat3 & quat_2_mat(mat3 &M, const quat &q );
extern quat & mat_2_quat(quat &q,const mat3 &M);
extern quat & mat_2_quat(quat &q,const mat4 &M);

// surface properties
extern mat3 & tangent_basis(mat3 & basis,const vec3 & v0,const vec3 & v1,const vec3 & v2,const vec2 & t0,const vec2 & t1,const vec2 & t2, const vec3 & n);

// linear interpolation
inline float lerp(float t, float a, float b)
{ return a * (nv_one - t) + t * b; }

inline vec3 & lerp(vec3 & w, const float & t, const vec3 & u, const vec3 & v)
{ w.x = lerp(t, u.x, v.x); w.y = lerp(t, u.y, v.y); w.z = lerp(t, u.z, v.z); return w; }

// utilities
inline float nv_min(const float & lambda, const float & n)
{ return (lambda < n ) ? lambda : n; }

inline float nv_max(const float & lambda, const float & n)
{ return (lambda > n ) ? lambda : n; }

inline float nv_clamp(float u, const float min, const float max)
{ u = (u < min) ? min : u; u = (u > max) ? max : u; return u; }

extern float nv_random();

extern quat & trackball(quat & q, vec2 & pt1, vec2 & pt2, float trackballsize);

extern vec3 & cube_map_normal(int i, int x, int y, int cubesize, vec3 & v);

// geometry
// computes the area of a triangle
extern float nv_area(const vec3 & v1, const vec3 & v2, const vec3 &v3);
// computes the perimeter of a triangle
extern float nv_perimeter(const vec3 & v1, const vec3 & v2, const vec3 &v3);
// find the inscribed circle
extern float nv_find_in_circle( vec3 & center, const vec3 & v1, const vec3 & v2, const vec3 &v3);
// find the circumscribed circle
extern float nv_find_circ_circle( vec3 & center, const vec3 & v1, const vec3 & v2, const vec3 &v3);

// fast cosine functions
extern float fast_cos(const float x);
extern float ffast_cos(const float x);

// determinant
float det(const mat3 & A);


#endif //_nv_algebra_h_
