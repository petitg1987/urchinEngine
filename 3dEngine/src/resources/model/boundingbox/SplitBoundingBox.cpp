#include "resources/model/boundingbox/SplitBoundingBox.h"

namespace urchin {

    SplitBoundingBox::SplitBoundingBox() :
            limitSize(ConfigService::instance().getFloatValue("model.boxLimitSize")) {

    }

    /**
     * Split the bounding box in several bounding boxes at limitSize.
     * If original box does not exceed limit size, the original bounding box is returned.
     * @param splitBoundingBox [out] Split bounding box
     */
    void SplitBoundingBox::split(const AABBox<float>& aabbox, std::vector<AABBox<float>>& splitBoundingBox) const {
        splitBoundingBox.clear();

        std::array<std::vector<float>, 3> axisSplits;
        for (std::size_t axis = 0; axis < 3; ++axis) {
            axisSplits[axis].push_back(aabbox.getMin()[axis]);

            float size = aabbox.getHalfSize((unsigned int)axis) * 2.0f;
            auto nbSplits = MathFunction::ceilToInt(size / limitSize);

            float maxValue = aabbox.getMax()[axis];
            for (int split = 0; split < nbSplits; ++split) {
                float splitValue = std::min(aabbox.getMin()[axis] + limitSize * ((float) split + 1.0f), maxValue);
                axisSplits[axis].push_back(splitValue);
            }
        }

        for (std::size_t x = 1; x < axisSplits[0].size(); ++x) {
            for (std::size_t y = 1; y < axisSplits[1].size(); ++y) {
                for (std::size_t z = 1; z < axisSplits[2].size(); ++z) {
                    Point3 minPoint(axisSplits[0][x - 1], axisSplits[1][y - 1], axisSplits[2][z - 1]);
                    Point3 maxPoint(axisSplits[0][x], axisSplits[1][y], axisSplits[2][z]);

                    splitBoundingBox.emplace_back(minPoint, maxPoint);
                }
            }
        }
    }

}
