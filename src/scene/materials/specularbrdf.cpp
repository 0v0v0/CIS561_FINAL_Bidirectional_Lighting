#include "specularbrdf.h"

Color3f SpecularBRDF::f(const Vector3f &wo, const Vector3f &wi) const
{
    return Color3f(0.f);
}


float SpecularBRDF::Pdf(const Vector3f &wo, const Vector3f &wi) const
{
    return 0.f;
}

Color3f SpecularBRDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &sample, Float *pdf, BxDFType *sampledType) const
{
    //TODO!
    //surface normal 0 0 1 !

    //Normal
    glm::vec3 normal=glm::vec3(0,0,1);

    *wi = Vector3f(-wo.x, -wo.y, wo.z);
    *pdf = 1;
    return fresnel->Evaluate(wi->z) * R / std::abs(wi->z);
    //return fresnel->Evaluate(CosTheta(*wi)) * R / AbsCosTheta(*wi);
    //inline Float CosTheta(const Vector3f &w) { return w.z; }
    //inline Float AbsCosTheta(const Vector3f &w) { return std::abs(w.z); }
}
