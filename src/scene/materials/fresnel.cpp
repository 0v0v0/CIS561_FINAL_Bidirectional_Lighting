#include "fresnel.h"

Color3f FresnelDielectric::Evaluate(float cosThetaI) const
{
    //TODO


    //cosThetaI = Clamp(cosThetaI, -1, 1);

    if(cosThetaI < -1.0f)
    {
        cosThetaI = -1.0f;
    }
    else if(cosThetaI > 1.0f)
    {
        cosThetaI = 1.0f;
    }

    float etat=etaT;
    float etai=etaI;

    // Potentially swap indices of refraction
    bool entering = cosThetaI > 0.f;

    if (!entering)
    {
        std::swap(etai, etat);
        cosThetaI = std::abs(cosThetaI);
    }

    // Compute _cosThetaT_ using Snell's law
    Float sinThetaI = std::sqrt(std::max((Float)0, 1 - cosThetaI * cosThetaI));
    Float sinThetaT = etai / etat * sinThetaI;

    // Handle total internal reflection
    if (sinThetaT >= 1)
    {
        return Color3f(1);
    }

    Float cosThetaT = std::sqrt(std::max((Float)0, 1 - sinThetaT * sinThetaT));

    Float Rparl = ((etat * cosThetaI) - (etai * cosThetaT)) /
            ((etat * cosThetaI) + (etai * cosThetaT));
    Float Rperp = ((etai * cosThetaI) - (etat * cosThetaT)) /
            ((etai * cosThetaI) + (etat * cosThetaT));
    float result = (Rparl * Rparl + Rperp * Rperp) / 2;
    return Color3f( result );

    //return Color3f(0);
}
