#include "naiveintegrator.h"


Color3f NaiveIntegrator::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth) const
{
    //TODO

    /*
    Scene::Intersect
    Intersection::Le
    Intersection::ProduceBSDF
    Intersection::SpawnRay
    BSDF::Sample_f
    */


    //Seems like it is run per-pixel on camera


    //    Color3f L(0.f);

    //    // Intersect _ray_ with scene and store intersection in _isect_
    //    Intersection isect;

    //    Ray ray2(ray.origin, ray.direction);

    //    if (scene.Intersect(ray2, &isect))
    //    {

    //        glm::vec3 Li=glm::vec3(1);

    //        //Sample loop is in Render class
    //        //No need to do this again here

    //        //For each interation per sample
    //        for(int j = 0; j<recursionLimit; j++)
    //        {

    //            if(isect.ProduceBSDF())
    //            {

    //                Vector3f wi = glm::vec3(1);
    //                Vector3f wo= -ray2.direction;
    //                Float pdf;

    //                Point2f u = sampler->Get2D();

    //                //Get Color

    //                BxDFType a;
    //                BxDFType b = BSDF_ALL;

    //                Color3f F=glm::vec3(0);

    //                F = isect.bsdf->Sample_f(wo,&wi,u,&pdf,b, &a);

    //                if(pdf == 0.0)
    //                {
    //                    break;
    //                }

    //                Li *= F * AbsDot(wi, isect.normalGeometric) / pdf;

    //                //Next Ray
    //                ray2=isect.SpawnRay(wi);

    //                //If next ray hit nothing
    //                if (!scene.Intersect(ray2,&isect))
    //                {
    //                    break;
    //                }
    //            }
    //            else
    //            {
    //                //hit something does not have a BRDF
    //                //perhaps a light?
    //                //Should we break? That means give up reflection on light mesh(possibly)


    //                //see light at first glance
    //                if(j == 0)
    //                {
    //                    ray2.direction=ray2.direction*glm::vec3(-1);
    //                    L = isect.Le(ray2.direction);
    //                    break;
    //                }
    //                else
    //                {
    //                    L = Li * isect.Le(ray2.direction);
    //                    break;
    //                }

    //            }
    //        }
    //    }
    //    else
    //    {
    //        //see the void space at first glance
    //        L = glm::vec3(0,0.1,0);
    //    }
    //    return L;


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
        return L;
    }

    Vector3f wiW(0.0f);
    float pdf = 0.0f;

    BxDFType a;
    BxDFType b = BSDF_ALL;

    Color3f f_term = isect.bsdf->Sample_f(woW, &wiW, sampler->Get2D(), &pdf,b,&a);

    //    if(pdf == 0.0f)
    //    {
    //        if(!(a & BSDF_SPECULAR))
    //        {
    //            return Color3f(0.f);
    //        }
    //        else
    //        {
    //            Ray newRay = isect.SpawnRay(wiW);
    //            L += f_term * Li(newRay, scene, sampler, depth - 1);
    //            return L;
    //        }
    //    }



    Ray newRay = isect.SpawnRay(wiW);

    if(pdf!=0.0f)
    {
        L += (f_term * Li(newRay, scene, sampler, depth - 1) * AbsDot(wiW, glm::normalize(isect.normalGeometric))) / pdf;
    }
    else
    {
        L += f_term * Li(newRay, scene, sampler, depth - 1) * AbsDot(wiW, glm::normalize(isect.normalGeometric));
    }

    return L;


}

