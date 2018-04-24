#define _USE_MATH_DEFINES
#include "warpfunctions.h"
#include <math.h>

Point3f WarpFunctions::squareToDiskUniform(const Point2f &sample)
{
    //TODO
    //TODO
    float rad=sample.x*2*M_PI;
    float r=sample.y+0.1f; //add 0.1 to prevent the first circle to compressed in center!

    return glm::vec3(r*sin(rad),r*cos(rad),0);
    //throw std::runtime_error("You haven't yet implemented uniform disk warping!");
}

Point3f WarpFunctions::squareToDiskConcentric(const Point2f &sample)
{
    //TODO
    //TODO
    float phi, r, u, v;
    float a = 2*sample.x-1; // (a,b) is now on [-1,1]ˆ2
    float b = 2*sample.y-1;
    if (a > -b) // region 1 or 2
    {
        if (a > b) // region 1, also |a| > |b|
        {
            r = a;
            phi = (M_PI/4 ) * (b/a);
        }
        else // region 2, also |b| > |a|
        {
            r = b;
            phi = (M_PI/4) * (2 - (a/b));
        }
    }
    else // region 3 or 4
    {
        if (a < b) // region 3, also |a| >= |b|, a != 0
        {
            r = -a;
            phi = (M_PI/4) * (4 + (b/a));
        }
        else // region 4, |b| >= |a|, but a==0 and b==0 could occur.
        {
            r = -b;
            if (b != 0)
            {
                phi = (M_PI/4) * (6 - (a/b));
            }
            else
            {
                phi = 0;
            }
        }
    }
    u = r * cos( phi );
    v = r * sin( phi );
    return glm::vec3(u,v,0);
    //throw std::runtime_error("You haven't yet implemented concentric disk warping!");
}

float WarpFunctions::squareToDiskPDF(const Point3f &sample)
{
    //TODO
    return 1/M_PI;
}

Point3f WarpFunctions::squareToSphereUniform(const Point2f &sample)
{
    //TODO
    //from [0,1) to [-1,1]
    float sigma1=2*sample.x-1;
    float sigma2=2*sample.y-1;

    float z=1-2*sigma1;
    float x=cos(sigma2*M_PI*2)*sqrt(1-z*z);
    float y=sin(sigma2*M_PI*2)*sqrt(1-z*z);
    //throw std::runtime_error("You haven't yet implemented uniform sphere warping!");

    return glm::vec3(x,y,z);
}

float WarpFunctions::squareToSphereUniformPDF(const Point3f &sample)
{
    //TODO
    return 1/(4*M_PI);
}

Point3f WarpFunctions::squareToSphereCapUniform(const Point2f &sample, float thetaMin)
{
    //TODO
    //TODO

    //Degree to [0,1]
    float distribute=thetaMin/180.f;
    //from [0,1) to [-1,distribute]
    float sigma1=(1+distribute)*sample.x-1;
    float sigma2=2*sample.y-1;

    float z=1-2*sigma1;
    float x=cos(sigma2*M_PI*2)*sqrt(1-z*z);
    float y=sin(sigma2*M_PI*2)*sqrt(1-z*z);

    return glm::vec3(x,y,z);
}

float WarpFunctions::squareToSphereCapUniformPDF(const Point3f &sample, float thetaMin)
{
    //TODO
    float area;
    float r=1;
    if(thetaMin<90)
    {
        float r2=r*sin(M_PI*thetaMin/180.f);
        area=M_PI*((r-r2)*(r-r2)+r2*r2);
    }
    else
    {
        float angle=180.f-thetaMin;
        float r2=r*sin(M_PI*angle/180.f);
        area=4*M_PI - M_PI*(((r-r2)*(r-r2)+r2*r2));
    }
    return 1/area; //WHY is this WRONG???
}

Point3f WarpFunctions::squareToHemisphereUniform(const Point2f &sample)
{
    //TODO
    //from [0,1) to [-1,1]
    float theta=sample.x*M_PI_2;
    float psi=sample.y*M_PI*2.0f;

    float x=sin(theta)*cos(psi);
    float y=sin(theta)*sin(psi);
    float z=sqrt(1-x*x-y*y);
    //throw std::runtime_error("You haven't yet implemented uniform hemisphere warping!");
    return glm::vec3(x,y,z);
}

float WarpFunctions::squareToHemisphereUniformPDF(const Point3f &sample)
{
    //TODO
    return 1/(2*M_PI);
}

Point3f WarpFunctions::squareToHemisphereCosine(const Point2f &sample)
{
    //TODO
    glm::vec3 disk=squareToDiskConcentric(sample);
    float x=disk.x;
    float y=disk.y;
    float z=sqrt(1-x*x-y*y);
    //throw std::runtime_error("You haven't yet implemented uniform hemisphere warping!");
    return glm::vec3(x,y,z);

    //throw std::runtime_error("You haven't yet implemented cosine-weighted hemisphere warping!");
}

float WarpFunctions::squareToHemisphereCosinePDF(const Point3f &sample)
{
    //TODO
    //float length=sqrt(sample.x*sample.x+sample.y*sample.y+sample.z*sample.z);

    float length = sqrt(sample.x*sample.x+sample.y*sample.y);

    float theta = asin(length);

    return cos(theta)*InvPi;

}
