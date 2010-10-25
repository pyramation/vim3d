#ifndef CHANNEL_H
#define CHANNEL_H

#include "OpenGLinc.h"
#include "Packages.h"
#include "Elements.h"
#include "Generic.h"

class Point;
class Channel;
typedef vector <Channel *> chan_t;

class Channel : public Generic {
    public:

        void setBounds();
        float maxX, minX, maxY, minY, maxZ, minZ;
        static float globalMaxY, globalMinY;
        /* each Channel has a start and end frame */
        bool enabled;
        int start;
        int end;
        int scopeStart;
        int scopeEnd;
        float sampleRate;
    
        String propertyInfo();

        vector<Point> points;
        void WriteCHAN(String filename);

        void Sin();    
        void Cos();    
        void display(int x,int y, int w, int h, int begin, float amountx, float amounty);
        void display(int x, int y, int w, int h);

        static Channel convolve(const Channel * X, const Channel * Y);
        Channel project(Channel * channel);
        Channel * differentiate();
        Channel operator * (const Channel &C) const;
        Channel operator / (const Channel &C) const;
        Channel operator + (const Channel &C) const;
        Channel operator - (const Channel &C) const;
        Channel operator * (const float &f) const;
        Channel operator / (const float &f) const;
        Channel operator + (const float &f) const;
        Channel operator - (const float &f) const;

        static Channel combine(const Channel * X, const Channel * Y, const Channel * Z);
        static Channel combine(const Channel * X, const Channel * Y);
        Channel(const Channel &ch);
        Channel(String filename);
        Channel();
        virtual ~Channel();

};

#endif

