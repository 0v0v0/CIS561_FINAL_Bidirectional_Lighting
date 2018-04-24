#include "fulllightingintegrator.h"
#define rrThreshold 0.1f

float MaxComponentValue(glm::vec3 in)
{
    return std::max(in.x, std::max(in.y,in.z));
}


Color3f FullLightingIntegrator::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth) const
{
    Color3f L(0.f), beta(1.f);
    //Ray ray(r);

    int bounces;

    Float etaScale = 1;

    //Make a Copy of the const ray...
    Ray ray2(ray);

    bounces = 0;

    bool specularBounce = false;

    for (bounces = 0;; ++bounces)
    {
        // Find next path vertex and accumulate contribution

        // Intersect _ray_ with scene and store intersection in _isect_
        Intersection isect;
        bool foundIntersection = false;
        foundIntersection= scene.Intersect(ray2, &isect);


        // Terminate path if ray escaped or _maxDepth_ was reached
        if ( (!foundIntersection) || (bounces >= depth) )
        {
            return L;
        }

        //Check if we hit a light, since light has no BSDF
        bool hit_light = false;
        hit_light = isect.ProduceBSDF();
        hit_light = !hit_light;

        if(hit_light)
        {
            //Decomment this to allow all integration

            //This is for "Correctly" Compute direct lighting
            //If we already got direct lighting here, ignore indirect lighting.
#define correct_lighting

#ifdef correct_lighting
            if(bounces == 0 || specularBounce)
            {
                L += beta * isect.Le(-ray2.direction);
            }
#else
            L += beta * isect.Le(-ray2.direction);
#endif
            return L;
        }

        // Sample illumination from lights to find path contribution.
        // (But skip this for perfectly specular BSDFs.)

        if (isect.bsdf->BxDFsMatchingFlags(BxDFType(BSDF_ALL & ~BSDF_SPECULAR)) >0)
        {
            Color3f Ld = glm::vec3(0);
            Ld = beta * GI_UniformSampleOneLight(isect, scene, sampler, ray2, depth);

            L += Ld;
        }

        // Sample BSDF to get new path direction
        Vector3f wo = -ray2.direction;
        Vector3f wi = glm::vec3(0);
        Float pdf = 0.f;
        BxDFType flags;
        Color3f f = glm::vec3(0);
        f = isect.bsdf->Sample_f(wo, &wi, sampler->Get2D(), &pdf, BSDF_ALL, &flags);

        if (IsBlack(f) || pdf == 0.f)
        {
            return L;
        }

        beta *= f * AbsDot(wi, isect.normalGeometric) / pdf;

        specularBounce = false;
        specularBounce = (flags & BSDF_SPECULAR) != 0;

        if ((flags & BSDF_SPECULAR) && (flags & BSDF_TRANSMISSION))
        {
            Float eta = isect.bsdf->eta;

            etaScale *= (glm::dot(wo, isect.normalGeometric) > 0) ? (eta * eta) : 1 / (eta * eta);
        }
        ray2 = isect.SpawnRay(wi);

        // Possibly terminate the path with Russian roulette.
        // Factor out radiance scaling due to refraction in rrBeta.
        Color3f rrBeta = beta * etaScale;

        if (MaxComponentValue(rrBeta) < rrThreshold && bounces > 3)
        {
            Float q = std::max( (Float).05, 1 - MaxComponentValue(rrBeta));

            if (sampler->Get1D() < q)
            {
                return L;
            }
            beta /= 1 - q;
        }
    }

    return L;

}

Color3f GI_UniformSampleOneLight(Intersection &isect, const Scene &scene ,
                                 std::shared_ptr<Sampler> sampler, const Ray &ray, const int depth)
{
    // Randomly choose a single light to sample, _light_
    int nLights = int(scene.lights.size());

    if (nLights == 0)
    {
        return Color3f(0.f);
    }

    int lightNum;

    Float lightPdf;

    //Which Light?
    lightNum = std::min((int)(sampler->Get1D() * nLights), nLights - 1);

    lightPdf = 1.f / nLights;

    const std::shared_ptr<Light> &light = scene.lights[lightNum];

    Point2f uLight = sampler->Get2D();

    return GI_EstimateDirect(isect, *light, uLight, scene, ray , sampler, depth) / lightPdf;

}

Color3f GI_EstimateDirect(const Intersection &isect, const Light &light,
                          const Point2f &uLight,const Scene &scene, const Ray &ray, std::shared_ptr<Sampler> sampler, const int depth)
{
    bool specular = false;
    BxDFType bsdfFlags = specular ? BSDF_ALL : BxDFType(BSDF_ALL & ~BSDF_SPECULAR);;

    Color3f Ld(0.f);
    // Sample light source with multiple importance sampling
    Vector3f wi;

    Float lightPdf = 0, scatteringPdf = 0;

    //Sample Light -> Sample Shape -> Sample squareplane/disc
    glm::vec3 point;
    Color3f Li = light.Sample_Li(isect , uLight, &wi, &lightPdf, &point);

    //Visibility test
    bool visible = false;
    glm::vec3 dir = glm::normalize(point - isect.point);
    Intersection isect2;
    //from sample point to origin point on surface
    Ray ray2(point, dir);
    ray2.origin = point;
    ray2.direction = -dir;

    if(scene.Intersect(ray2 , &isect2))
    {
        if(isect2.ProduceBSDF())
        {
            float len= glm::length(isect2.point - isect.point);

            if(len<0.001f)
            {
                visible = true;
            }
        }
    }

    const Intersection tmp = (const Intersection)isect;


    //Handle BSDF on surface
    if (!IsBlack(Li) )
    {
        // Compute BSDF or phase function's value for light sample
        Color3f f = Color3f(0);

        // Evaluate BSDF for light sampling strategy
        f = tmp.bsdf->f(-ray.direction, wi, bsdfFlags)*AbsDot(wi, isect.normalGeometric);
        if(IsBlack(f))
        {
            f =tmp.bsdf->f(ray.direction, wi, bsdfFlags)*AbsDot(wi, isect.normalGeometric);
        }


        scatteringPdf = isect.bsdf->Pdf(-ray.direction, wi, bsdfFlags);

        if (!IsBlack(f))
        {
            // Compute effect of visibility for light source sample
            if (!visible)
            {
                Li = Color3f(0.f);
            }
            // Add light's contribution to reflected radiance
            else
            {
                if(lightPdf !=0)
                {
                    Float weight = GI_PowerHeuristic(1, lightPdf, 1, scatteringPdf);
                    Ld += f * Li * weight / lightPdf;
                }
            }
        }
    }

    //HW6
    // Sample BSDF with multiple importance sampling
    Point2f uScattering = sampler->Get2D();
    Color3f f;
    bool sampledSpecular = false;
    Float weight2=1;
    Float lightPdf2;

    // Sample scattered direction for surface interactions
    BxDFType sampledType;
    //const Intersection &isect = (const SurfaceInteraction &)isect;

    const Intersection tmp2 = (const Intersection)isect;

    glm::vec3 wi2;
    Float scatteringPdf2;
    f = tmp2.bsdf->Sample_f(-ray.direction, &wi2, uScattering, &scatteringPdf2, bsdfFlags, &sampledType);
    f *= AbsDot(wi2, tmp2.normalGeometric);

    sampledSpecular = (sampledType & BSDF_SPECULAR) != 0;


    if(!IsBlack(f) && scatteringPdf2>0)
    {

        // Account for light contributions along sampled direction _wi_
        if (1)
        {
            light.Sample_Li(tmp2,uScattering,&wi2,&lightPdf2,&point);
            if (lightPdf2 == 0)
            {
                return Ld;
            }
            weight2 = GI_PowerHeuristic(1, scatteringPdf2, 1, lightPdf2);
        }

        // Find intersection and compute transmittance

        Intersection lightIsect;
        Ray ray2 = tmp2.SpawnRay(wi2);

        // Add light contribution from material sampling
        Color3f Li2(0.f);

        //If ray hit the light we sampled before...
        if (scene.Intersect(ray2, &lightIsect))
        {
            if (lightIsect.objectHit->GetAreaLight() == &light)
            {
                Li2 = lightIsect.Le(-wi2);
            }
        }
        else
        {
            Li2 = light.Le(ray);
        }

        if (!IsBlack(Li2))
        {
            Ld += f * Li2 * weight2/ scatteringPdf2;
        }
    }
    Ld/= (float)scene.lights.length();

    return Ld;
}

//read which light we already sampled in direct lighting...
Color3f GI_Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth, const Light* which_light, bool isdark)
{

    Color3f L(0.f);
    Vector3f woW = -ray.direction;

    Intersection isect;

    scene.Intersect(ray, &isect);

    if(isect.objectHit == nullptr){
        return Color3f(0.f);
    }

    L += isect.Le(woW);

    if(depth == 0){
        return L;
    }

    if(!isect.ProduceBSDF()){

        if ( (isect.objectHit->GetAreaLight() != which_light) || isdark )
        {
            return L;
        }
        else
        {
            return Color3f(0);
        }
    }

    Vector3f wiW(0.0f);
    float pdf = 0.0f;

    BxDFType a;
    BxDFType b = BSDF_ALL;

    Color3f f_term = isect.bsdf->Sample_f(woW, &wiW, sampler->Get2D(), &pdf,b,&a);

    Ray newRay = isect.SpawnRay(wiW);

    if(pdf!=0.0f)
    {
        L += f_term * GI_Li(newRay, scene, sampler, depth - 1, which_light, isdark) * AbsDot(wiW, glm::normalize(isect.normalGeometric)) / pdf;
    }
    else
    {
        L += f_term * GI_Li(newRay, scene, sampler, depth - 1, which_light, isdark) * AbsDot(wiW, glm::normalize(isect.normalGeometric));
    }

    return L;
}




inline float GI_BalanceHeuristic(int nf, Float fPdf, int ng, Float gPdf)
{
    //TODO
    return (nf * fPdf) / (nf * fPdf + ng * gPdf);
}

inline float GI_PowerHeuristic(int nf, Float fPdf, int ng, Float gPdf)
{
    //TODO
    Float f = nf * fPdf, g = ng * gPdf;
    return (f * f) / (f * f + g * g);
}

