#include "NavModelCopy.h"

namespace urchin {

    /**
     * @param copiedNavPolygons [out] Return copied navigation polygons
     */
    void NavModelCopy::copyNavPolygons(const std::vector<std::shared_ptr<NavPolygon>>& originalNavPolygons, std::vector<std::shared_ptr<NavPolygon>>& copiedNavPolygons) {
        std::map<const NavPolygon*, std::map<const NavTriangle*, std::pair<std::size_t, std::size_t>>> originalPositionsMap;

        copiedNavPolygons.reserve(originalNavPolygons.size());
        for (std::size_t originalPolygonIndex=0; originalPolygonIndex < originalNavPolygons.size(); ++originalPolygonIndex) {
            const auto& originalPolygon = originalNavPolygons[originalPolygonIndex];
            auto copiedNavPolygon = std::make_shared<NavPolygon>(*originalPolygon);

            //re-attach triangle to new polygons
            for (auto& copiedNavTriangle : copiedNavPolygon->getTriangles()) {
                copiedNavTriangle->attachNavPolygon(copiedNavPolygon);
            }

            //build map of original positions for polygons and triangles
            std::map<const NavTriangle*, std::pair<std::size_t, std::size_t>> originalTrianglesPositionsMap;
            for (std::size_t originalTriangleIndex=0; originalTriangleIndex < originalPolygon->getTriangles().size(); ++originalTriangleIndex) {
                originalTrianglesPositionsMap.insert(std::make_pair(originalPolygon->getTriangles()[originalTriangleIndex].get(),
                                                                    std::make_pair(originalPolygonIndex, originalTriangleIndex)));
            }
            originalPositionsMap.insert(std::make_pair(originalPolygon.get(), originalTrianglesPositionsMap));

            copiedNavPolygons.push_back(copiedNavPolygon);
        }

        //build triangles links
        for (std::size_t originalPolygonIndex=0; originalPolygonIndex < originalNavPolygons.size(); ++originalPolygonIndex) {
            const auto& originalPolygon = originalNavPolygons[originalPolygonIndex];
            for (std::size_t originalTriangleIndex=0; originalTriangleIndex < originalPolygon->getTriangles().size(); ++originalTriangleIndex) {
                const auto& originalTriangle = originalPolygon->getTriangles()[originalTriangleIndex];
                for (const auto& originalLink : originalTriangle->getLinks()) {
                    const auto& linkTargetPolygonPositions = originalPositionsMap.find(originalLink->getTargetTriangle()->getNavPolygon().get());
                    assert(linkTargetPolygonPositions != originalPositionsMap.end());

                    const auto& linkTargetTrianglePositions = linkTargetPolygonPositions->second.find(originalLink->getTargetTriangle().get());
                    assert(linkTargetTrianglePositions != linkTargetPolygonPositions->second.end());

                    const std::shared_ptr<NavTriangle>& linkTargetTriangle = copiedNavPolygons[linkTargetTrianglePositions->second.first]->getTriangles()[linkTargetTrianglePositions->second.second];
                    copiedNavPolygons[originalPolygonIndex]->getTriangles()[originalTriangleIndex]->addLink(originalLink->copyLink(linkTargetTriangle));
                }
            }
        }
    }

}
