#include "shape.h"
#include <QDateTime>

pcg32 Shape::colorRNG = pcg32(QDateTime::currentMSecsSinceEpoch());


void Shape::InitializeIntersection(Intersection *isect, float t, Point3f pLocal) const
{
    isect->point = Point3f(transform.T() * glm::vec4(pLocal, 1));
    ComputeTBN(pLocal, &(isect->normalGeometric), &(isect->tangent), &(isect->bitangent));
    isect->uv = GetUVCoordinates(pLocal);
    isect->t = t;
}

Intersection Shape::Sample(const Intersection &ref, const Point2f &xi, float *pdf) const
{
    //TODO
    Intersection intr = Sample(xi, pdf);
    Vector3f wi = ref.point - intr.point;

    if (wi.length() == 0)
    {
        *pdf = 0;
    }
    else
    {
        wi = glm::normalize(wi);
        // Convert from area measure, as returned by the Sample() call
        // above, to solid angle measure.
        *pdf *= glm::length2(ref.point - intr.point) / AbsDot(intr.normalGeometric, wi);

        if (std::isinf(*pdf))
        {
            *pdf = 0.f;
        }
    }
    return intr;
    //return Intersection();
}
