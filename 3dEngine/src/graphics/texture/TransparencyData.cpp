#include <algorithm>
#include <stdexcept>
#include <string>

#include "graphics/texture/TransparencyData.h"

namespace urchin {

    /**
     * @param minTransparency Transparency of the most opaque texel (0.0: fully opaque, 1.0: fully transparent)
     * @param maxTransparency Transparency of the most transparent texel (0.0: fully opaque, 1.0: fully transparent)
     */
    TransparencyData::TransparencyData(float minTransparency, float maxTransparency) :
            minTransparency(minTransparency),
            maxTransparency(maxTransparency) {
        if (minTransparency < 0.0f || maxTransparency > 1.0f || minTransparency > maxTransparency) {
            throw std::invalid_argument("Invalid transparency range: " + std::to_string(minTransparency) + " - " + std::to_string(maxTransparency));
        }
    }

    TransparencyData TransparencyData::buildOpaque() {
        return TransparencyData(0.0f, 0.0f);
    }

    /**
     * @param minAlpha Alpha of the most transparent texel (0.0: fully transparent, 1.0: fully opaque)
     * @param maxAlpha Alpha of the most opaque texel (0.0: fully transparent, 1.0: fully opaque)
     */
    TransparencyData TransparencyData::buildFromAlpha(float minAlpha, float maxAlpha) {
        return TransparencyData(1.0f - maxAlpha, 1.0f - minAlpha);
    }

    /**
     * @param minAlpha Alpha of the most transparent texel (0: fully transparent, 255: fully opaque)
     * @param maxAlpha Alpha of the most opaque texel (0: fully transparent, 255: fully opaque)
     */
    TransparencyData TransparencyData::buildFromAlpha8Bits(unsigned char minAlpha, unsigned char maxAlpha) {
        return buildFromAlpha((float)minAlpha / 255.0f, (float)maxAlpha / 255.0f);
    }

    float TransparencyData::getMinTransparency() const {
        return minTransparency;
    }

    float TransparencyData::getMaxTransparency() const {
        return maxTransparency;
    }

    bool TransparencyData::hasTransparency() const {
        return maxTransparency > std::numeric_limits<float>::epsilon();
    }

    TransparencyData TransparencyData::merge(const TransparencyData& transparencyData) const {
        return TransparencyData(std::min(minTransparency, transparencyData.getMinTransparency()), std::max(maxTransparency, transparencyData.getMaxTransparency()));
    }

}
