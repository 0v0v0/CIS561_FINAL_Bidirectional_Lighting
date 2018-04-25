#include "squareplane.h"

float SquarePlane::Area() const
{
    //TODO

    glm::vec3 scale= transform.getScale();

    return scale.x * scale.y;
}

bool SquarePlane::Intersect(const Ray &ray, Intersection *isect) const
{
    //Transform the ray
    Ray r_loc = ray.GetTransformedCopy(transform.invT());

    //Ray-plane intersection
    float t = glm::dot(glm::vec3(0,0,1), (glm::vec3(0.5f, 0.5f, 0) - r_loc.origin)) / glm::dot(glm::vec3(0,0,1), r_loc.direction);
    Point3f P = Point3f(t * r_loc.direction + r_loc.origin);
    //Check that P is within the bounds of the square
    if(t > 0 && P.x >= -0.5f && P.x <= 0.5f && P.y >= -0.5f && P.y <= 0.5f)
    {
        InitializeIntersection(isect, t, P);
        return true;
    }
    return false;
}

void SquarePlane::ComputeTBN(const Point3f &P, Normal3f *nor, Vector3f *tan, Vector3f *bit) const
{
    *nor = glm::normalize(transform.invTransT() * Normal3f(0,0,1));
    //TODO: Compute tangent and bitangent
    *tan =  glm::normalize( transform.T3() * glm::vec3(0,1,0));
    *bit =  glm::normalize( transform.T3() * glm::vec3(1,0,0));
}


Point2f SquarePlane::GetUVCoordinates(const Point3f &point) const
{
    return Point2f(point.x + 0.5f, point.y + 0.5f);
}

Intersection SquarePlane::Sample(const Point2f &xi, Float *pdf) const
{

    float x = (xi.x - 0.5f);
    float y = (xi.y - 0.5f);

    Intersection it;

//    it.normalGeometric = glm::normalize( transform.T3() *(Normal3f(0, 0, 1)));
//    it.point = transform.T3() * glm::vec3( x, y , 0.0f );

    glm::vec3 scale = transform.getScale();

    glm::vec3 p =  glm::vec3(scale.x* x, 0, scale.y* y);
    it.point = transform.position() + p;
    //it.point = transform.invTransT()*glm::vec3(x,y,0);

    //glm::vec3 p = transform.T3() * glm::vec3(x,y,0);


   // it.normalGeometric = glm::normalize(transform.invTransT() * Normal3f(0,0,1));
    it.normalGeometric = Normal3f(0,0,-1);

    *pdf = 1 / Area();

    return it;
}
