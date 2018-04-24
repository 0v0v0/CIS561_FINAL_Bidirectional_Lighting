#pragma once
#include "integrator.h"
#include <scene/photon.h>
#include <scene/kdtree.h>
#include <integrators/directlightingintegrator.h>

class PhotonMapper : public Integrator
{
public:
    PhotonMapper(int numPhotons, std::vector<Photon>* photons, Scene* s, std::shared_ptr<Sampler> sampler, int recursionLimit);
    PhotonMapper(Bounds2i bounds, Scene* s, std::shared_ptr<Sampler> sampler, KDTree* mygl_kdtree, int recursionLimit);

    //Set KD_Tree after mygl finished building...
    void setKDTree(KDTree * kdtree);

    virtual void Render();

    virtual Color3f Li(const Ray& ray, const Scene& scene, std::shared_ptr<Sampler> sampler, int depth) const;

    DirectLightingIntegrator* direct_light_integrator;

    Color3f Gather_Photons(const Ray& ray, const Scene& scene, std::shared_ptr<Sampler> sampler);

private:
    bool preprocessing;
    //Preprocess variables
    int numPhotons;
    std::vector<Photon>* photons;

    //HW9
    KDTree* kdTree;
};

