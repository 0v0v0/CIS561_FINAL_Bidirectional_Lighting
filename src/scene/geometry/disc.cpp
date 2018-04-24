#include "disc.h"

float Disc::Area() const
{
    //TODO

    glm::vec3 scale=transform.getScale();

    return Pi*scale.x*scale.y*scale.z;
}

bool Disc::Intersect(const Ray &ray, Intersection *isect) const
{
    //Transform the ray
    Ray r_loc = ray.GetTransformedCopy(transform.invT());

    //Ray-plane intersection
    float t = glm::dot(glm::vec3(0,0,1), (glm::vec3(0.5f, 0.5f, 0) - r_loc.origin)) / glm::dot(glm::vec3(0,0,1), r_loc.direction);
    Point3f P = Point3f(t * r_loc.direction + r_loc.origin);
    //Check that P is within the bounds of the disc (not bothering to take the sqrt of the dist b/c we know the radius)
    float dist2 = (P.x * P.x + P.y * P.y);
    if(t > 0 && dist2 <= 1.f)
    {
        InitializeIntersection(isect, t, P);
        return true;
    }
    return false;
}

void Disc::ComputeTBN(const Point3f &P, Normal3f *nor, Vector3f *tan, Vector3f *bit) const
{
    *nor = glm::normalize(transform.invTransT() * Normal3f(0,0,1));
    //TODO: Compute tangent and bitangent
    *tan = glm::vec3(nor->y,nor->x,nor->z);
    *bit = glm::vec3(nor->x,nor->z,nor->y);
}


Point2f Disc::GetUVCoordinates(const Point3f &point) const
{
    return glm::vec2((point.x + 1)/2.f, (point.y + 1)/2.f);
}

Intersection Disc::Sample(const Point2f &xi, Float *pdf) const
{
    Point3f pd = WarpFunctions::squareToDiskConcentric(xi);

    pd.x = (pd.x - 0.5f)*2.f;
    pd.y = (pd.y - 0.5f)*2.f;

    Intersection it;

    glm::mat3 obj2wrd=transform.T3();

    it.normalGeometric = glm::normalize(obj2wrd*(Normal3f(0, 0, 1)));

    it.point = obj2wrd*pd;

    *pdf = 1 / Area();

    return it;
}

