#include "kdtree.h"

KDNode::KDNode()
    : leftChild(nullptr), rightChild(nullptr), axis(0), minCorner(), maxCorner(), particles()
{}

KDNode::~KDNode()
{
    delete leftChild;
    delete rightChild;
}

KDTree::KDTree()
    : root(nullptr)
{}

KDTree::~KDTree()
{
    delete root;
}

//Modified here!
// Comparator functions you can use with std::sort to sort vec3s along the cardinal axes
bool xSort(Photon a, Photon b) { return a.pos.x < b.pos.x; }
bool ySort(Photon a, Photon b) { return a.pos.y < b.pos.y; }
bool zSort(Photon a, Photon b) { return a.pos.z < b.pos.z; }

void KDTree::build(const std::vector<Photon> *points)
{
    //TODO
    //TODO
    //I think this time the points will be many so let's use uint instead of int?
    //unsigned int does not work well in for loops, change back!

    long size=points->size();

    //Init
    for( long i=0;i<size;i++)
    {
        xsorted.push_back(i);
        ysorted.push_back(i);
        zsorted.push_back(i);
    }

    //Insert Sort
    //Checked from Wikipidia, which is the most efficient way I think?
    //This sorts the points from big to small in xyz axis

    long x_tmp;
    long y_tmp;
    long z_tmp;

    qDebug() << "Sorting X";

    for(long i=0; i<size; i++)
    {
        x_tmp=xsorted.at(i);
        long j=i-1;

        while(j>=0 && xSort( points->at( xsorted.at(j) ) , points->at(x_tmp) ) )
        {
            xsorted.at(j+1)=xsorted.at(j);
            j = j-1;
        }
        xsorted.at(j+1)=x_tmp;
    }

    qDebug() << "Sorting Y";
    for(long i=0; i<size; i++)
    {
        y_tmp=ysorted.at(i);
        long j=i-1;

        while(j>=0 && ySort( points->at( ysorted.at(j) ) , points->at(y_tmp) ) )
        {
            ysorted.at(j+1)=ysorted.at(j);
            j = j-1;
        }
        ysorted.at(j+1)=y_tmp;
    }

    qDebug() << "Sorting Z";
    for(long i=0; i<size; i++)
    {
        z_tmp=zsorted.at(i);
        long j=i-1;

        while(j>=0 && zSort( points->at( zsorted.at(j) ) , points->at(z_tmp) ) )
        {
            zsorted.at(j+1)=zsorted.at(j);
            j = j-1;
        }
        zsorted.at(j+1)=z_tmp;
    }

    qDebug() << "sorting complete!";

    //build root
    root=recursive(0,0,size-1,points);


    qDebug() << "build complete!";
}

KDNode* KDTree::recursive( long dir,
                           long min,
                           long max,
                           const std::vector<Photon> *points)
{
    long size=max-min;
    KDNode* node=new KDNode();

    if(size>0)
    {
        long left,right;

        if(size%2 == 0)
        {
            //odd num of elements
            left=(size/2);
            right=size-left;

            left+=min;
            right+=min;

            if(size>1)
            {
                node->particles.push_back(points->at(left));
            }


            right++;
            if(right>max)
            {
                right=max;
            }

            //Spawn left child
            node->leftChild=recursive(dir+1,min,left,points);
            //Spawn right child
            node->rightChild=recursive(dir+1,right,max,points);
        }
        else
        {
            //even num of elements
            left=(size/2);
            right=size-left;

            left+=min;
            right+=min;

            if(size>1)
            {
                node->particles.push_back(points->at(left));
            }

            left--;
            if(left<min)
            {
                left=min;
            }

            //Spawn left child
            node->leftChild=recursive(dir+1,min,left,points);
            //Spawn right child
            node->rightChild=recursive(dir+1,right,max,points);
        }
    }
    else
    {
        node->particles.push_back(points->at(min));
        node->leftChild=NULL;
        node->rightChild=NULL;
        //qDebug() << "leaf pushed" << min;

    }

    node->axis=dir%3;

    return node;

}


void KDTree::particlesInSphere(glm::vec3 c, float r, std::vector<Photon*> * buffer)
{
    //init
    //std::vector<Photon> buffer;
    buffer->clear();

    KDNode* p;

    p=root;

    if(p->particles.size()>0)
    {
        Photon a =p->particles.at(0);
        glm::vec3 pos =a.pos;

        float length=glm::dot(pos-c,pos-c);

        if(length<r*r)
        {
            //qDebug()<< "Find ! Len= " << length;
            buffer->push_back(&a);
        }
        if(p->leftChild!=NULL)
        {
            scan(buffer,p->leftChild,c,r);
        }
        if(p->rightChild!=NULL)
        {
            scan(buffer,p->rightChild,c,r);
        }
    }

    //qDebug() << "Find Finished!";
}

void KDTree::scan(std::vector<Photon*>* buffer,KDNode* p, glm::vec3 c, float r)
{
    if(p->particles.size()>0)
    {
        Photon a =p->particles.at(0);
        glm::vec3 pos =a.pos;

        float length=glm::dot(pos-c,pos-c);

        if(length<r*r)
        {
            //qDebug()<< "Find ! Len= " << length;
            buffer->push_back(&a);
        }

        if(p->leftChild!=NULL)
        {
            scan(buffer,p->leftChild,c,r);
        }
        if(p->rightChild!=NULL)
        {
            scan(buffer,p->rightChild,c,r);
        }
    }
}

void KDTree::clear()
{
    delete root;
    root = nullptr;
}
