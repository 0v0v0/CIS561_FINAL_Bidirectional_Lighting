#ifndef BIDIRECTIONAL_H
#define BIDIRECTIONAL_H

#pragma once
#include "integrator.h"

class BiDirectionalIntegrator : public Integrator
{
public:
    BiDirectionalIntegrator(Bounds2i bounds, Scene* s, std::shared_ptr<Sampler> sampler, int recursionLimit)
        : Integrator(bounds, s, sampler, recursionLimit)
    {}

    // Evaluate the energy transmitted along the ray back to
    // its origin, which can only be the camera in a direct lighting
    // integrator (only rays from light sources are considered)
    virtual Color3f Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth) const;
    virtual Color3f Gather_Photons(const Ray& ray, const Scene& scene, std::shared_ptr<Sampler> sampler);
};

#endif // BIDIRECTIONAL_H
