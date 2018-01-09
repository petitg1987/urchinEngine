#include "TerrainGrassQuadtree.h"

namespace urchin
{

    TerrainGrassQuadtree::TerrainGrassQuadtree(const std::vector<TerrainGrassQuadtree *> &children) :
        children(children)
    {

    }

    TerrainGrassQuadtree::~TerrainGrassQuadtree()
    {
        for(const auto *child : children)
        {
            delete child;
        }
    }

    bool TerrainGrassQuadtree::isLeaf() const
    {
        return children.empty();
    }

    const std::unique_ptr<AABBox<float>> &TerrainGrassQuadtree::getBox() const
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

    void TerrainGrassQuadtree::addChild(TerrainGrassQuadtree *grassPatchOctree)
    {
        #ifdef _DEBUG
            assert(bbox==nullptr);
        #endif

        children.push_back(grassPatchOctree);
    }

    const std::vector<TerrainGrassQuadtree *> &TerrainGrassQuadtree::getChildren() const
    {
        return children;
    }

    void TerrainGrassQuadtree::addVertex(const Point3<float> &vertex)
    {
        #ifdef _DEBUG
            assert(children.empty());
            assert(bbox==nullptr);
        #endif

        grassVertices.push_back(vertex);
    }

    const std::vector<Point3<float>> &TerrainGrassQuadtree::getGrassVertices() const
    {
        return grassVertices;
    }

}
