#include "TerrainGrassQuadtree.h"

#include <utility>

namespace urchin {

    TerrainGrassQuadtree::TerrainGrassQuadtree(std::vector<TerrainGrassQuadtree*> children) :
            children(std::move(children)) {

    }

    TerrainGrassQuadtree::~TerrainGrassQuadtree() {
        for (const auto* child : children) {
            delete child;
        }
    }

    void TerrainGrassQuadtree::setRenderer(std::unique_ptr<GenericRenderer>&& renderer) {
        this->renderer = std::move(renderer);
    }

    const std::unique_ptr<GenericRenderer>& TerrainGrassQuadtree::getRenderer() const {
        return renderer;
    }

    bool TerrainGrassQuadtree::isLeaf() const {
        return children.empty();
    }

    const std::unique_ptr<AABBox<float>>& TerrainGrassQuadtree::getBox() const {
        if (!bbox) {
            if (isLeaf()) {
                if(grassVertices.empty()) {
                    bbox = std::unique_ptr<AABBox<float>>(nullptr);
                } else {
                    bbox = std::make_unique<AABBox<float>>(grassVertices);
                }
            } else {
                const std::unique_ptr<AABBox<float>>& localBboxPtr = children[0]->getBox();
                if(localBboxPtr) {
                    AABBox<float> localBbox = *localBboxPtr;
                    for (std::size_t i = 1; i < children.size(); ++i) {
                        localBbox = localBbox.merge(*children[i]->getBox());
                    }
                    bbox = std::make_unique<AABBox<float>>(localBbox);
                } else {
                    bbox = std::unique_ptr<AABBox<float>>(nullptr);
                }
            }
        }

        return bbox;
    }

    void TerrainGrassQuadtree::addChild(TerrainGrassQuadtree* grassPatchOctree) {
        assert(bbox == nullptr);
        children.push_back(grassPatchOctree);
    }

    const std::vector<TerrainGrassQuadtree*>& TerrainGrassQuadtree::getChildren() const {
        return children;
    }

    /**
     * Add vertex with its associate normal. Method is thread-safe.
     */
    void TerrainGrassQuadtree::addVertex(const Point3<float>& vertex, const Vector3<float>& normal) {
        std::lock_guard<std::mutex> lock(mutexAddVertex);

        assert(children.empty());
        assert(bbox == nullptr);

        grassVertices.push_back(vertex);
        normals.push_back(normal);
    }

    const std::vector<Point3<float>>& TerrainGrassQuadtree::getGrassVertices() const {
        return grassVertices;
    }

    const std::vector<Vector3<float>>& TerrainGrassQuadtree::getGrassNormals() const {
        return normals;
    }
}
