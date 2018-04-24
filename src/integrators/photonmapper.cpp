#include "photonmapper.h"

PhotonMapper::PhotonMapper(int numPhotons, std::vector<Photon> *photons, Scene *s, std::shared_ptr<Sampler> sampler, int recursionLimit)
    : Integrator(Bounds2i(Point2i(0,0), Point2i(0,0)), s, sampler, recursionLimit), preprocessing(true), numPhotons(numPhotons), photons(photons)
{
    //numPhotons = 100000;
}

PhotonMapper::PhotonMapper(Bounds2i bounds, Scene *s, std::shared_ptr<Sampler> sampler, KDTree *mygl_kdtree, int recursionLimit)
    : Integrator(bounds, s, sampler, recursionLimit), preprocessing(false), numPhotons(0), photons(nullptr)
{
    setKDTree(mygl_kdtree);

    direct_light_integrator = new DirectLightingIntegrator(bounds,s,sampler,recursionLimit);
}

void PhotonMapper::Render()
{
    // PhotonMapper's Render() function has
    // two modes: when preprocessing, it traces
    // a collection of photons through the scene
    // and stores them in the given k-d tree.
    // If not preprocessing, it runs like a regular Integrator
    // and invokes Li().

    if(preprocessing)
    {
        // TODO
        // Determine how many photons to assign to each light source
        // given numPhotons and the intensity of each light.
        // Shoot a number of photons equal to numPhotons from
        // the lights, bouncing them through the scene and pushing
        // back the result of each bounce to the photons vector
        // stored in the PhotonMapper.


        //TOTAL Photons shoot = numPhotons

        int nLights = scene->lights.size();

        if(nLights == 0)
        {
            return;
        }

        //FOR EACH photon shot into the scene
        for(long i =0;i< numPhotons ; i++)
        {
            //Which Light we shoot from?
            int which_light = std::min((int)(sampler->Get1D() * nLights), nLights - 1);

            std::shared_ptr<Light> light = scene->lights.at(which_light);

            //Color3f DiffuseAreaLight::Sample_Le(const Point2f &u1, const Point2f &u2,
            //                                    Ray *ray, Normal3f *nLight,
            //                                    Float *pdfPos, Float *pdfDir) const

            Ray photonRay(glm::vec3(0),glm::vec3(0));
            Normal3f light_nor;
            float pdfPos;
            float pdfDir;

            Color3f Le(0);

            //Shoot 1st generation of Photons from light
            Point2f u1 = sampler->Get2D();
            Point2f u2 = sampler->Get2D();

            Le = light->Sample_Le( u1, u2, &photonRay, &light_nor, &pdfPos, &pdfDir);

            ///DEBUG FOR PHOTON BOUNCING
//            Le = glm::vec3(1);
//            photonRay.origin = glm::vec3(u1.x-0.5f, 7.24f, u1.y-0.5f);
//            glm::vec3 dir  = WarpFunctions::squareToHemisphereCosine(sampler->Get2D());

//            dir.y = -dir.y;
//            photonRay.direction = dir;
            ///


            if (pdfPos == 0 || pdfDir == 0 || IsBlack(Le))
            {
                break;
            }

            float lightPdf = 1.f/(float)nLights;

            Color3f beta(1);
            beta *= AbsDot(light_nor, photonRay.direction) / (lightPdf * pdfPos * pdfDir);

            if (IsBlack(beta))
            {
                break;
            }

            //For each bounce
            for (int depth = 0; depth < 5; ++depth)
            {
                //Track this photon in the scene
                Intersection isect;

                //Safety tests
                if (!scene->Intersect(photonRay, &isect))
                {
                    break;
                }

                if(!isect.ProduceBSDF())
                {
                    break;
                }

                if(isect.bsdf == nullptr)
                {
                    break;
                }

                Vector3f wiW = glm::vec3(0);
                float surfacePdf = 0.f;
                Color3f f = glm::vec3(0);

                //Since it is BI-DIRECTIONAL, we can reverse wi and wo here.
                //wi is next photon ray direction

                bool specular = false;

                BxDFType bsdfFlags = specular ? BSDF_ALL : BxDFType(BSDF_ALL & ~BSDF_SPECULAR);
                BxDFType sampled_bsdf;

                f = isect.bsdf->Sample_f(photonRay.direction, &wiW, sampler->Get2D(), &surfacePdf , bsdfFlags, &sampled_bsdf);

                wiW = glm::normalize(wiW);

                if( IsBlack(f) || ( surfacePdf == 0 ))
                {
                    break;
                }

                beta *= f * AbsDot(wiW, isect.normalGeometric) / surfacePdf;

                //Ignore the 1st gen of photons since they are direct lighting
                //Only record later bounces
                if (depth > 0)
                {
                    ///DEBUG, BECAME MORE BLUE AT DEEPER BOUNCES
//                    Color3f debug(0,0,0);
//                    debug.r = 5 - depth;
//                    debug.b = depth;
//                    Photon a( isect.point, debug, photonRay.direction );
                    ///

                    /// pos(p), color(c), wi(w) ///
                    //Color3f photon_energy = beta;
                    Color3f photon_energy = beta * Le / (float)numPhotons;
                    Photon a( isect.point, photon_energy, photonRay.direction );

                    photons->push_back(a);
                }

                //UPDATE photonRay
                photonRay.direction = wiW;
                photonRay.origin = isect.point + wiW*0.0001f;

            //END OF bounce loop
            }

        //END OF photon number loop
        }
    }
    else
    {
        Integrator::Render(); // Invokes Li for each ray from a pixel
    }
}



Color3f PhotonMapper::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth) const
{

    Color3f L= Color3f(0);

    Intersection isect;

    //Direct Lighting Part...
    L+=direct_light_integrator->Li(ray,scene,sampler,depth);

//    if(!scene.Intersect(ray,&isect))
//    {
//        return L;
//    }

//    if(!isect.ProduceBSDF())
//    {
//        L += isect.Le(-ray.direction);
//        return L;
//    }

//    //Get Photons
//    float r = 0.1f;

//    std::vector<Photon*> photon_inrange;
//    kdTree->particlesInSphere(isect.point, r, &photon_inrange);

//    Color3f photon_gather(0);

//    for(int i=0; i <photon_inrange.size() ; i++)
//    {
//        Photon* a = photon_inrange.at(i);

//        //L += a->color;

//        // Evaluate BSDF for light sampling strategy

//        bool specular = false;

//        BxDFType bsdfFlags = specular ? BSDF_ALL : BxDFType(BSDF_ALL & ~BSDF_SPECULAR);

//        float scatteringPdf = 0;

//        Color3f f(0);

//        f = isect.bsdf->f(-ray.direction, a->wi, bsdfFlags);
//        scatteringPdf = isect.bsdf->Pdf(-ray.direction, a->wi, bsdfFlags);

//        if(IsBlack(f))
//        {
//            f =isect.bsdf->f(ray.direction, a->wi, bsdfFlags);
//            scatteringPdf = isect.bsdf->Pdf(ray.direction, a->wi, bsdfFlags);
//        }

//        float dot=AbsDot(a->wi, isect.normalGeometric);

//        if(!std::isinf(dot))
//        {
//            f*=dot;
//        }

//        if (!IsBlack(f))
//        {
//            // Add light's contribution to reflected radiance
//            {
//                if( (scatteringPdf !=0) && (!std::isinf(scatteringPdf)))
//                {
//                    L += f * a->color/ scatteringPdf;
//                    //L+= f * (a->color) * (1.f/ photon_inrange.size() );
//                }
//            }
//        }

//    }

    return L;
}

//HW9

Color3f PhotonMapper::Gather_Photons(const Ray& ray, const Scene& scene, std::shared_ptr<Sampler> sampler)
{
    Color3f L(0);

    Intersection isect;

    if(!scene.Intersect(ray,&isect))
    {
        return L;
    }

    if(!isect.ProduceBSDF())
    {
        L += isect.Le(-ray.direction);
        return L;
    }

    //Get Photons
    float r = 0.1f;

    std::vector<Photon*> photon_inrange;
    kdTree->particlesInSphere(isect.point, r, &photon_inrange);

    Color3f photon_gather(0);

    for(long i=0; i <photon_inrange.size() ; i++)
    {
        Photon* a = photon_inrange.at(i);

        //L += a->color;

        // Evaluate BSDF for light sampling strategy

        bool specular = false;

        BxDFType bsdfFlags = specular ? BSDF_ALL : BxDFType(BSDF_ALL & ~BSDF_SPECULAR);

        float scatteringPdf = 0;

        Color3f f(0);

        f = isect.bsdf->f(-ray.direction, a->wi, bsdfFlags);
        scatteringPdf = isect.bsdf->Pdf(-ray.direction, a->wi, bsdfFlags);

        if(IsBlack(f))
        {
            f =isect.bsdf->f(ray.direction, a->wi, bsdfFlags);
            scatteringPdf = isect.bsdf->Pdf(ray.direction, a->wi, bsdfFlags);
        }

        float dot=AbsDot(a->wi, isect.normalGeometric);

        if(!std::isinf(dot))
        {
            f*=dot;
        }

        if (!IsBlack(f))
        {
            // Add light's contribution to reflected radiance
            {
                if( (scatteringPdf !=0) && (!std::isinf(scatteringPdf)))
                {
                    L += f * a->color/ scatteringPdf;
                    //L+= f * (a->color) * (1.f/ photon_inrange.size() );
                }
            }
        }

    }

    return L;
}

void PhotonMapper::setKDTree(KDTree * kdtree)
{
    kdTree = kdtree;
}
