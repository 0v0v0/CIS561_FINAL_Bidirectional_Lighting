#pragma once
#include "integrator.h"

class FullLightingIntegrator : public Integrator
{
public:
    FullLightingIntegrator(Bounds2i bounds, Scene* s, std::shared_ptr<Sampler> sampler, int recursionLimit)
        : Integrator(bounds, s, sampler, recursionLimit)
    {}

    // Evaluate the energy transmitted along the ray back to
    // its origin using multiple importance sampling
    virtual Color3f Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth) const;
};

//float BalanceHeuristic(int nf, Float fPdf, int ng, Float gPdf);
//float PowerHeuristic(int nf, Float fPdf, int ng, Float gPdf);


//Function Announcer

float GI_BalanceHeuristic(int nf, Float fPdf, int ng, Float gPdf);
float GI_PowerHeuristic(int nf, Float fPdf, int ng, Float gPdf);


Color3f GI_Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth, const Light *which_light, bool isdark);

/// Sample One Light at each time only
/// Pass Sampler and Camera Ray into this...for Estimate Direct Light
Color3f GI_UniformSampleOneLight(Intersection &isect, const Scene &scene ,
                                 std::shared_ptr<Sampler> sampler, const Ray &ray, const int depth);

/// Estimate Direct Light
/// Need Original Ray and the Sampler
/// They are const, so they're safe here.
Color3f GI_EstimateDirect(const Intersection &it, const Light &light,
                          const Point2f &uLight, const Scene &scene,
                          const Ray &ray, std::shared_ptr<Sampler> sampler, const int depth);
