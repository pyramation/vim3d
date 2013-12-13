#include "Image.h"

Image::Image() { type=IMAGE_TYPE;pixels = NULL; }

Image::Image(String filepath) {
    type=IMAGE_TYPE;
    FILE *fp ; int i;
    if((fp = fopen(filepath.c_str(),"rb"))) {
        //fscanf(fp,"%*s %*d %*d %*d%*c") ;
        char s[48];
        fgets(s,48, fp); // read P6
        fgets(s,48, fp); // read width and height
        sscanf(s,"%d %d", &w, &h);
        pixels = new GLubyte[4*w*h];
        printf("h: %d, w: %d\n", h, w);
        fgets(s,48, fp);
        fgets(s,48, fp);
        for (i = 0 ; i < h*w ; i++) {
            fscanf(fp,"%c",&(pixels[i*4 + 2]));
            fscanf(fp,"%c",&(pixels[i*4 + 1]));
            fscanf(fp,"%c",&(pixels[i*4 + 0]));
            pixels[i*4 + 3] = (GLubyte) 255;
        }
        fclose(fp) ;
    } else {
        printf("error reading file\n");
    }

}

Image::Image(GLsizei w, GLsizei h, GLenum format, GLenum imgtype) {
    type = IMAGE_TYPE;
    this->w = w;
    this->h = h;
    this->format = format;
    this->imgtype = imgtype;

    if (format == GL_RGB) {
        pixels = new GLubyte[3*w*h];
    } else if (format == GL_RGBA) {
        pixels = new GLubyte[4*w*h];
    }

}


void Image::SobelEdgeDetect(int w, int h, GLubyte * pixels) {
    float GX[3][3];
    float GY[3][3];

    /* 3x3 GX Sobel mask.  Ref: www.cee.hw.ac.uk/hipr/html/sobel.html */
    GX[0][0] = -1.0; GX[0][1] = 0.0; GX[0][2] = 1.0;
    GX[1][0] = -2.0; GX[1][1] = 0.0; GX[1][2] = 2.0;
    GX[2][0] = -1.0; GX[2][1] = 0.0; GX[2][2] = 1.0;

    /* 3x3 GY Sobel mask.  Ref: www.cee.hw.ac.uk/hipr/html/sobel.html */
    GY[0][0] =  1.0; GY[0][1] =  2.0; GY[0][2] =  1.0;
    GY[1][0] =  0.0; GY[1][1] =  0.0; GY[1][2] =  0.0;
    GY[2][0] = -1.0; GY[2][1] = -2.0; GY[2][2] = -1.0;

    float SUMr = 0;
    float SUMg = 0;
    float SUMb = 0;
    for(int i=0; i<h; i++)  {
        for(int j=0; j<w; j++)  {
            float SUMXr = 0;
            float SUMXg = 0;
            float SUMXb = 0;
            float SUMYr = 0;
            float SUMYg = 0;
            float SUMYb = 0;

            if(i==0 || i==h-1) SUMr=SUMg=SUMb = 0;
            else if(j==0 ||j==w-1) SUMr=SUMg=SUMb = 0;
            else   {

                for(int h=-1; h<2; h++)  {
                    for(int k=-1; k<2; k++)  {
                        SUMXr+=pixels[3*(j+h+(i+k)*w)+0]*GX[h+1][k+1];
                        SUMXg+=pixels[3*(j+h+(i+k)*w)+1]*GX[h+1][k+1];
                        SUMXb+=pixels[3*(j+h+(i+k)*w)+2]*GX[h+1][k+1];
                    }
                }

                for(int h=-1; h<2; h++)  {
                    for(int k=-1; k<2; k++)  {
                        SUMYr+=pixels[3*(j+h+(i+k)*w)+0]*GY[h+1][k+1];
                        SUMYg+=pixels[3*(j+h+(i+k)*w)+1]*GY[h+1][k+1];
                        SUMYb+=pixels[3*(j+h+(i+k)*w)+2]*GY[h+1][k+1];
                    }
                }

                SUMr = abs(SUMXr) + abs(SUMYr);
                SUMg = abs(SUMXg) + abs(SUMYg);
                SUMb = abs(SUMXb) + abs(SUMYb);
            }

            if(SUMr>255) SUMr=255; if(SUMr<0) SUMr=0;
            if(SUMg>255) SUMg=255; if(SUMg<0) SUMg=0;
            if(SUMb>255) SUMb=255; if(SUMb<0) SUMb=0;

            pixels[3*(j+i*w)+0]=255.0-SUMr;
            pixels[3*(j+i*w)+1]=255.0-SUMg;
            pixels[3*(j+i*w)+2]=255.0-SUMb;
        }
    }



}

// right now works only for RGB
void Image::WriteImage(String filename, int w, int h, GLubyte * pixels) {
    FILE* fd = fopen(filename.c_str(), "w");
    fprintf(fd, "P6\n");
    fprintf(fd, "%d %d\n", w, h);
    fprintf(fd, "255\n");
    fprintf(fd, " \n");
    fprintf(fd, " \n");
    // for 2d -> 1D arrayPosition = (y * numberOfColumns) + x
    // for 3D -> 1D arrayPos = (numChannels * (x*numCols+y) + chan)
    for(int k=h-1;k>=0;k--) {
        for(int j=0;j<w;j++) {
            fprintf(fd, "%c", pixels[3*(k*w+j)+1]);    
            fprintf(fd, "%c", pixels[3*(k*w+j)+2]);    
            fprintf(fd, "%c", pixels[3*(k*w+j)+0]);    
        }
    }
    fclose(fd);
    printf("wrote %s\n", filename.c_str());
}

void Image::WriteImage(String filename) {
    FILE* fd = fopen(filename.c_str(), "w");
    fprintf(fd, "P6\n");
    fprintf(fd, "%d %d\n", w, h);
    fprintf(fd, "255\n");
    fprintf(fd, " \n");
    fprintf(fd, " \n");
    // for 2d -> 1D arrayPosition = (y * numberOfColumns) + x
    // for 3D -> 1D arrayPos = (numChannels * (x*numCols+y) + chan)
    for(int k=h-1;k>=0;k--) {
        for(int j=0;j<w;j++) {
            fprintf(fd, "%c", pixels[3*(k*w+j)+1]);    
            fprintf(fd, "%c", pixels[3*(k*w+j)+2]);    
            fprintf(fd, "%c", pixels[3*(k*w+j)+0]);    
        }
    }
    fclose(fd);
    printf("wrote %s\n", filename.c_str());
}

Image::~Image() { if (pixels) delete pixels;}

