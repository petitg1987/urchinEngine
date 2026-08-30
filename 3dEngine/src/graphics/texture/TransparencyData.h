#pragma once

namespace urchin {

    class TransparencyData {
        public:
            TransparencyData(float, float);

            static TransparencyData buildOpaque();
            static TransparencyData buildFromAlpha(float, float);
            static TransparencyData buildFromAlpha8Bits(unsigned char, unsigned char);

            float getMinTransparency() const;
            float getMaxTransparency() const;
            bool hasTransparency() const;

            TransparencyData merge(const TransparencyData&) const;

        private:
            float minTransparency;
            float maxTransparency;
    };

}
