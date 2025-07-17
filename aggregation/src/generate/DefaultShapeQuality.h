#pragma once

#include <string>

namespace urchin {

    class DefaultShapeQuality {
        public:
            static const DefaultShapeQuality LOW;
            static const DefaultShapeQuality MEDIUM;
            static const DefaultShapeQuality HIGH;
            static const DefaultShapeQuality ULTRA;

            static std::array<DefaultShapeQuality, 4> getAllQualities();
            static DefaultShapeQuality getQualityById(int);

            int getId() const;
            const std::string& getLabel() const;
            float getVoxelizationSize() const;
            unsigned int getConvexHullMaxPoints() const;

        private:
            DefaultShapeQuality(int, std::string, float, unsigned int);

            int id;
            std::string label;
            float voxelizationSize;
            unsigned int convexHullMaxPoints;
    };

}
