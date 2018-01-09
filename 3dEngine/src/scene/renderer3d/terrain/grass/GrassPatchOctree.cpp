#include "GrassPatchOctree.h"

namespace urchin
{

    GrassPatchOctree::GrassPatchOctree(const std::vector<GrassPatchOctree *> &children) :
        children(children)
    {

    }

    GrassPatchOctree::~GrassPatchOctree()
    {
        for(const auto *child : children)
        {
            delete child;
        }
    }

    bool GrassPatchOctree::isLeaf() const
    {
        return children.empty();
    }

    const std::unique_ptr<AABBox<float>> &GrassPatchOctree::getBox() const
    {
        if(bbox==nullptr)
        {
            if(isLeaf())
            {
                bbox = std::make_unique<AABBox<float>>(grassVertices);
            }else
            {
                AABBox<float> localBbox = *children[0]->getBox();
                for(unsigned int i=1; i<children.size(); ++i)
                {
                    localBbox = localBbox.merge(*children[i]->getBox());
                }
                bbox = std::make_unique<AABBox<float>>(localBbox);
            }
        }

        return bbox;
    }

    void GrassPatchOctree::addChild(GrassPatchOctree *grassPatchOctree)
    {
        #ifdef _DEBUG
            assert(bbox==nullptr);
        #endif

        children.push_back(grassPatchOctree);
    }

    const std::vector<GrassPatchOctree *> &GrassPatchOctree::getChildren() const
    {
        return children;
    }

    void GrassPatchOctree::addVertex(const Point3<float> &vertex)
    {
        #ifdef _DEBUG
            assert(children.empty());
            assert(bbox==nullptr);
        #endif

        grassVertices.push_back(vertex);
    }

    const std::vector<Point3<float>> &GrassPatchOctree::getGrassVertices() const
    {
        return grassVertices;
    }

}
