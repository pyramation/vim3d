#include "Color.h"

void Color::clamp() {
  if (r>255.0f) r=255.0f;
  if (g>255.0f) g=255.0f;
  if (b>255.0f) b=255.0f;
  if (r<0)r=0;
  if (g<0)g=0;
  if (b<0)b=0;
}

Color::Color(float r, float g, float b) {
   this->r = r;
   this->g = g;
   this->b = b;
}

void Color::operator /= (const float c) {
   if (!c) return;
   r/=c;g/=c;b/=c;
}
void Color::operator *= (const Color c) {
   r *= c.r;
   g *= c.g;
   b *= c.b;
}
void Color::operator += (const Color c) {
   r+=c.r;g+=c.g;b+=c.b;
}
void Color::operator *= (const float scalar) {
   r *= scalar;
   g *= scalar;
   b *= scalar;
}

Color Color::operator + (const Color c) {
  Color clr(this->r, this->g, this->b);
  clr.r += c.r;
  clr.g += c.g;
  clr.b += c.b;
  return clr;
}
Color Color::operator + (const float s) {
  Color clr(this->r, this->g, this->b);
  clr.r += s;
  clr.g += s;
  clr.b += s;
  return clr;
}
Color Color::operator * (const float s) {
  Color clr(this->r, this->g, this->b);
  clr.r *= s;
  clr.g *= s;
  clr.b *= s;
  return clr;
}
Color Color::operator / (const float s) {
  if (s == 0.0f) return *this;
  Color clr(this->r, this->g, this->b);
  clr.r /= s;
  clr.g /= s;
  clr.b /= s;
  return clr;
}

Color::Color() {}

Color::~Color() {}

