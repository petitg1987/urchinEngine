#include "TerrainGrassQuadtree.h"

namespace urchin
{

    TerrainGrassQuadtree::TerrainGrassQuadtree() :
            vertexArrayObjectId(0)
    {

    }

    TerrainGrassQuadtree::TerrainGrassQuadtree(const std::vector<TerrainGrassQuadtree *> &children) :
            vertexArrayObjectId(0),
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

    void TerrainGrassQuadtree::setVertexArrayObjectId(unsigned int vertexArrayObjectId)
    {
        this->vertexArrayObjectId = vertexArrayObjectId;
    }

    unsigned int TerrainGrassQuadtree::getVertexArrayObjectId() const
    {
        return vertexArrayObjectId;
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

    /**
     * Add vertex with its associate normal. Method is thread-safe.
     */
    void TerrainGrassQuadtree::addVertex(const Point3<float> &vertex, const Vector3<float> &normal)
    {
        std::lock_guard<std::mutex> lock(mutexAddVertex);

        #ifdef _DEBUG
            assert(children.empty());
            assert(bbox==nullptr);
        #endif

        grassVertices.push_back(vertex);
        normals.push_back(normal);
    }

    const std::vector<Point3<float>> &TerrainGrassQuadtree::getGrassVertices() const
    {
        return grassVertices;
    }

    const std::vector<Vector3<float>> &TerrainGrassQuadtree::getGrassNormals() const
    {
        return normals;
    }
}
