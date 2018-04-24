#include "bidirectional.h"
#include "scene/photon.h"

//USE this to store path nodes.
struct path_node
{
    Point3f point;
    Color3f energy;
    Vector3f direction;
};



Color3f BiDirectionalIntegrator ::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth) const
{
    Color3f L(0);

    //For each ray here, generate a light ray to pair with it.

    int nLights = scene.lights.size();

    if(nLights <=0)
    {
        return L;
    }

    //Select a Light
    int lightNum;
    Float lightPdf;
    //Which Light?
    lightNum = std::min((int)(sampler->Get1D() * nLights), nLights - 1);
    lightPdf = 1.f / nLights;

    std::shared_ptr<Light> light = ( scene.lights.at(lightNum) );

    Ray lightray(glm::vec3(0),glm::vec3(0));
    Normal3f nor(0);
    float pdfPos = 0;
    float pdfDir = 0;
    Color3f Le(0);

    Point2f u1=sampler->Get2D();
    Point2f u2=sampler->Get2D();
    //Get Light Color
    Le = light->Sample_Le( u1, u2, &lightray, &nor, &pdfPos, &pdfDir);

    Le*=AbsDot(lightray.direction,nor);

    if(pdfPos!=0 && pdfDir!=0)
    {
        Le*= 1.f/(lightPdf*pdfPos*pdfDir);
    }

    std::vector<path_node> campath;
    std::vector<path_node> lightpath;


    Ray camray(ray);

    camray.direction = ray.direction;
    camray.origin = ray.origin;

    Color3f beta(1);

    QString sampled_light;

    /// bounce CAMERA ray
    for(int i=0;i<depth;i++)
    {
        Intersection isect;

        if( !scene.Intersect(camray,&isect) )
        {
            break;
        }

        if( !isect.ProduceBSDF() )
        {
            if(i == 0) // see light at first glance
            {
                return isect.Le(-camray.direction);
            }
            else
            {
                //L+= isect.Le(-camray.direction) * beta;
                sampled_light = isect.objectHit->GetAreaLight()->name;
                break;
            }
        }

        if( isect.bsdf == nullptr)
        {
            break;
        }

        BxDFType a;
        BxDFType b = BSDF_ALL;

        Vector3f wiW;
        float pdf = 0;

        Color3f f_term = isect.bsdf->Sample_f(-camray.direction, &wiW, sampler->Get2D(), &pdf,b,&a);

//        if(IsBlack(f_term))
//        {
//            f_term = isect.bsdf->Sample_f(camray.direction, &wiW, sampler->Get2D(), &pdf,b,&a);
//        }

        f_term*=AbsDot(isect.normalGeometric,wiW);

        if(pdf!=0)
        {
            beta*=f_term/pdf;
        }
        else
        {
            break;
        }

        //Store node
        path_node cam_path_node;
        cam_path_node.energy = beta;
        cam_path_node.point = isect.point;
        cam_path_node.direction = camray.direction;
        campath.push_back(cam_path_node);

        Ray next(isect.SpawnRay(wiW));

        camray.direction = next.direction;
        camray.origin = next.origin;
    }

    //Re init beta
    beta = glm::vec3(1);

    /// bounce LIGHT ray
    for(int i=0;i<depth;i++)
    {
        Intersection isect;

        if( !scene.Intersect(lightray,&isect) )
        {
            break;
        }

        if( !isect.ProduceBSDF() )
        {
            break;  //This means the light ray hit a light, unluckily!
        }

        if( isect.bsdf == nullptr)
        {
            break;
        }

        BxDFType a;
        BxDFType b = BSDF_ALL;

        Vector3f wiW;
        float pdf = 0;

        Color3f f_term = isect.bsdf->Sample_f(-lightray.direction, &wiW, sampler->Get2D(), &pdf,b,&a);

//        if(IsBlack(f_term))
//        {
//            f_term = isect.bsdf->Sample_f(lightray.direction, &wiW, sampler->Get2D(), &pdf,b,&a);
//        }

        f_term*=AbsDot(isect.normalGeometric,-lightray.direction);

        if(pdf!=0)
        {
            beta*=f_term/pdf;
        }
        else
        {
            break;
        }

        //Store node
        path_node light_path_node;
        light_path_node.energy = Le*beta;
        light_path_node.point = isect.point;
        light_path_node.direction = lightray.direction;
        lightpath.push_back(light_path_node);


        Ray next(isect.SpawnRay(wiW));

        lightray.direction = next.direction;
        lightray.origin = next.origin;
    }


    float count = 0;
    //Connect light path
    for(int i=0;i<campath.size();i++)
    {
        for(int j=0;j<lightpath.size();j++)
        {
            path_node* camnode = &(campath.at(i));
            path_node* lightnode = &(lightpath.at(j));

            //Visibility test
            bool visible = false;
            glm::vec3 dir = glm::normalize(lightnode->point - camnode->point); //From Light to camera

            Intersection isect;

            Ray ray2(lightnode->point, dir);
            ray2.origin = lightnode->point;
            ray2.direction = -dir;

            if(scene.Intersect(ray2 , &isect))
            {
                if(isect.ProduceBSDF())
                {
                    float len = 1;

                    len =glm::length( isect.point - camnode->point );

                    if(len<0.000001f)
                    {
                        visible = true;
                    }
                }
            }


            //END of visibility test

            if(visible)
            {
                if(isect.bsdf!=nullptr)
                {

                    Color3f f(0);
                    f = isect.bsdf->f(dir,-camnode->direction);

                    if(IsBlack(f))
                    {
                        f = isect.bsdf->f(dir,camnode->direction);
                    }

                    float pdf = 0;
                    pdf = isect.bsdf->Pdf(dir,-camnode->direction);

                    f*= AbsDot(isect.normalGeometric,dir);

                    if(pdf!=0)
                    {
                        L +=  camnode->energy * lightnode->energy *f/pdf;
                        count++;

                        if(sampled_light == light->name)
                        {
                            L*=0.5f;
                        }
                    }
                }
            }


        }
    }

    if(count>0)
    {
        L = L*0.0015f/count;
    }


    //CLEAR BUFFERS
    campath.clear();
    lightpath.clear();

    return L;
}

Color3f BiDirectionalIntegrator::Gather_Photons(const Ray& ray, const Scene& scene, std::shared_ptr<Sampler> sampler)
{
    return Color3f(0);
}


