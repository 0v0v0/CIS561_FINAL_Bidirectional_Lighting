#include "bsdf.h"
#include <warpfunctions.h>

BSDF::BSDF(const Intersection& isect, float eta /*= 1*/)
//TODO: Properly set worldToTangent and tangentToWorld
    : worldToTangent(glm::mat3(1)),
      tangentToWorld(glm::mat3(1)),
      normal(isect.normalGeometric),
      eta(eta),
      numBxDFs(0),
      bxdfs{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}
{


    glm::vec3 n = isect.normalGeometric;
    n = glm::normalize(n);
    glm::vec3 t = isect.tangent;
    glm::vec3 b =isect.bitangent;

    //glm::vec3 b = glm::vec3(n.x,n.z,n.y);

//    ns=n;
//    ts=t;
//    ss=b;

    UpdateTangentSpaceMatrices(n,t,b);
}

//Vector3f BSDF::WorldToLocal(const Vector3f &v) const
//{
//    //return Vector3f(glm::dot(v, ss), glm::dot(v, ts), glm::dot(v, ns));

//    return worldToTangent*v;
//}
//Vector3f BSDF::LocalToWorld(const Vector3f &v) const
//{
////    return Vector3f(ss.x * v.x + ts.x * v.y + ns.x * v.z,
////                    ss.y * v.x + ts.y * v.y + ns.y * v.z,
////                    ss.z * v.x + ts.z * v.y + ns.z * v.z);
//    return tangentToWorld*v;
//}



void BSDF::UpdateTangentSpaceMatrices(const Normal3f& n, const Vector3f& t, const Vector3f b)
{
    //TODO: Update worldToTangent and tangentToWorld based on the normal, tangent, and bitangent

    //glm::vec3 p= isect.point;
    //glm::vec3 t=isect.tangent;
    //glm::vec3 bit=isect.bitangent;
    //glm::vec3 nor=isect.normalGeometric;


    /*
    glm::quat q = glm::rotation(glm::vec3(0,1,0),n);
    worldToTangent = glm::toMat3(q);

    q = glm::rotation(n,glm::vec3(0,1,0));
    tangentToWorld = glm::toMat3(q);
    */

    glm::mat3 i;

    i=glm::mat3(t,b,n);

    glm::mat3 j = glm::inverse(i);

    worldToTangent= j;
    tangentToWorld= i;

    //tangentToWorld = glm::inverse(worldToTangent);

}


//
Color3f BSDF::f(const Vector3f &woW, const Vector3f &wiW, BxDFType flags /*= BSDF_ALL*/) const
{
    //TODO
    Color3f f(0.f);
    glm::vec3 wi = worldToTangent*wiW;
    glm::vec3 wo = worldToTangent*woW;

    if (wo.z == 0)
    {
        return f;
    }

    bool reflect = (glm::dot(wiW, glm::vec3(0,1,0)) * glm::dot(woW, glm::vec3(0,1,0)) > 0);



    for (int i = 0; i < numBxDFs; ++i)
    {
        if (bxdfs[i]->MatchesFlags(flags) &&
                ((reflect && (bxdfs[i]->type & BSDF_REFLECTION)) ||
                 (!reflect && (bxdfs[i]->type & BSDF_TRANSMISSION))))
        {
            f += bxdfs[i]->f(wo, wi);
        }
    }

    return f;
}

// Use the input random number _xi_ to select
// one of our BxDFs that matches the _type_ flags.

// After selecting our random BxDF, rewrite the first uniform
// random number contained within _xi_ to another number within
// [0, 1) so that we don't bias the _wi_ sample generated from
// BxDF::Sample_f.

// Convert woW and wiW into tangent space and pass them to
// the chosen BxDF's Sample_f (along with pdf).
// Store the color returned by BxDF::Sample_f and convert
// the _wi_ obtained from this function back into world space.

// Iterate over all BxDFs that we DID NOT select above (so, all
// but the one sampled BxDF) and add their PDFs to the PDF we obtained
// from BxDF::Sample_f, then average them all together.

// Finally, iterate over all BxDFs and sum together the results of their
// f() for the chosen wo and wi, then return that sum.

Color3f BSDF::Sample_f(const Vector3f &woW, Vector3f *wiW, const Point2f &xi,
                       float *pdf, BxDFType type, BxDFType *sampledType) const
{
    //TODO




    // Choose which _BxDF_ to sample
    int matchingComps = numBxDFs;

    if (matchingComps == 0)
    {
        *pdf = 0;
        if (sampledType)
        {
            *sampledType = BxDFType(0);
            return glm::vec3(0);
        }
    }



    int comp = std::min((int)std::floor(xi[0] * matchingComps), matchingComps - 1);


    // Get _BxDF_ pointer for chosen component
    BxDF *bxdf = nullptr;

    int count = comp;
    for (int i = 0; i < numBxDFs; ++i)
    {
        if (bxdfs[i]->MatchesFlags(type) && count-- == 0)
        {
            bxdf = bxdfs[i];
            break;
        }
    }


    // Remap _BxDF_ sample _u_ to [0,1)^2
    Point2f uRemapped(std::min(xi[0] * matchingComps - comp, OneMinusEpsilon), xi[1]);

    // Sample chosen _BxDF_
    Vector3f wi, wo = worldToTangent*woW;

    if (wo.z == 0)
    {
        return glm::vec3(0);
    }

    *pdf = 0;

    if (sampledType)
    {
        *sampledType = bxdf->type;
    }


    Color3f f;
    f = bxdf->Sample_f(wo, &wi, uRemapped, pdf, sampledType);


    if (*pdf == 0)
    {
        if (sampledType)
        {
            *sampledType = BxDFType(0);
        }
        return glm::vec3(0);
    }
    *wiW = tangentToWorld*wi;



    // Compute overall PDF with all matching _BxDF_s
    if (!(bxdf->type & BSDF_SPECULAR) && matchingComps > 1)
    {
        for (int i = 0; i < numBxDFs; ++i)
        {
            if (bxdfs[i] != bxdf && bxdfs[i]->MatchesFlags(type))
            {
                *pdf += bxdfs[i]->Pdf(wo, wi);
            }
        }
    }
    if (matchingComps > 1)
    {
        *pdf /= matchingComps;
    }

    // Compute value of BSDF for sampled direction
    if (!(bxdf->type & BSDF_SPECULAR) && matchingComps > 1)
    {
        bool reflect = glm::dot(*wiW, normal) * glm::dot(woW, normal) > 0;
        f = glm::vec3(0);
        for (int i = 0; i < numBxDFs; ++i)
        {
            if (bxdfs[i]->MatchesFlags(type) &&
                    ((reflect && (bxdfs[i]->type & BSDF_REFLECTION)) ||
                     (!reflect && (bxdfs[i]->type & BSDF_TRANSMISSION))))
            {
                f += bxdfs[i]->f(wo, wi);
            }
        }
    }
    return f;
}


float BSDF::Pdf(const Vector3f &woW, const Vector3f &wiW, BxDFType flags) const
{
    //TODO

    if (numBxDFs == 0.f)
    {
        return 0.f;
    }
    Vector3f wo = worldToTangent*woW, wi = worldToTangent*wiW;
    if (wo.z == 0)
    {
        return 0.;
    }
    Float pdf = 0.f;
    int matchingComps = 0;

    for (int i = 0; i < numBxDFs; ++i)
    {
        if (bxdfs[i]->MatchesFlags(flags))
        {
            ++matchingComps;
            pdf += bxdfs[i]->Pdf(wo, wi);
        }
    }
    Float v = matchingComps > 0 ? pdf / matchingComps : 0.f;
    return v;

}

Color3f BxDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &xi,
                       Float *pdf, BxDFType *sampledType) const
{
    //TODO
//    WarpFunctions warp;
//    *wi = warp.squareToHemisphereCosine(xi);
    *wi = WarpFunctions::squareToHemisphereCosine(xi);

    if (wo.z < 0)
    {
        wi->z *= -1;
    }
    *pdf = Pdf(wo, *wi);
    return f(wo, *wi);

}

// The PDF for uniform hemisphere sampling
float BxDF::Pdf(const Vector3f &wo, const Vector3f &wi) const
{
    return SameHemisphere(wo, wi) ? Inv2Pi : 0;
}

BSDF::~BSDF()
{
    for(int i = 0; i < numBxDFs; i++)
    {
        delete bxdfs[i];
    }
}
