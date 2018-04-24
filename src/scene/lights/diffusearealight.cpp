#include "diffusearealight.h"

Color3f DiffuseAreaLight::L(const Intersection &isect, const Vector3f &w) const
{
    //TODO

    if(twoSided)
    {
        return emittedLight;
    }
    else if( glm::dot(isect.normalGeometric, w) > 0)
    {
        return emittedLight;
    }

    //return glm::vec3(0);
    return emittedLight;

}

Color3f DiffuseAreaLight::Sample_Li(const Intersection &ref, const Point2f &xi,
                                    Vector3f *wi, Float *pdf, Vector3f *point) const
{
    //Generate intersection
    Intersection pShape = shape->Sample(ref, xi, pdf);

    *point=pShape.point;

    //pShape.mediumInterface = mediumInterface;

    if (*pdf == 0 || glm::length(pShape.point - ref.point) == 0)
    {
        *pdf = 0;
        return Color3f(0);
    }
    *wi = glm::normalize(pShape.point - ref.point);


    return L(pShape, -*wi);
}

float DiffuseAreaLight::Pdf_Li(const Intersection &ref, const Vector3f &wi) const
{
    Ray ray = ref.SpawnRay(wi);
    Float tHit;
    Intersection isectLight;
    // Ignore any alpha textures used for trimming the shape when performing
    // this intersection. Hack for the "San Miguel" scene, where this is used
    // to make an invisible area light.

    if(!shape->Intersect(ray,&isectLight))
    {
        return 0;
    }

    // Convert light sample weight to solid angle measure
    Float pdf = glm::length2(ref.point - isectLight.point) / (AbsDot(isectLight.normalGeometric, -wi) * shape->Area());
    if (std::isinf(pdf))
    {
        pdf = 0.f;
    }
    return pdf;
}

Color3f DiffuseAreaLight::Sample_Le(const Point2f &u1, const Point2f &u2,
                                    Ray *ray, Normal3f *nLight,
                                    Float *pdfPos, Float *pdfDir) const
{

    // Sample a point on the area light's _Shape_, _pShape_
    Intersection pShape = shape->Sample(u1, pdfPos);

    pShape.normalGeometric = glm::normalize(pShape.normalGeometric);

    *nLight = pShape.normalGeometric;

    // Sample a cosine-weighted outgoing direction _w_ for area light
    Vector3f w(0);

    w = WarpFunctions::squareToHemisphereCosine(u2);
    *pdfDir = WarpFunctions::squareToHemisphereCosinePDF(w);


///DEBUG
//    w = shape->transform.T3() * w;
//    w = glm::normalize(w);
//    ray->origin= pShape.point - w*0.1f;
//    ray->direction = w ;
//    *nLight = glm::vec3(0,-1,0);
//    pShape.normalGeometric = glm::vec3(0,-1,0);

    Vector3f v1, v2, n(pShape.normalGeometric);
    CoordinateSystem(n, &v1, &v2);

    w = w.x * v1 + w.y * v2 + w.z * n;
    w = glm::normalize(w);
    Ray a(pShape.SpawnRay(w));

    ray->direction = a.direction;
    ray->origin = a.origin;

    return L(pShape, w);

    //return L(pShape, -w);
}

