#pragma once

#include <UrchinCommon.h>

namespace urchin {

    class AIShape {
        public:
            explicit AIShape(std::unique_ptr<ConvexShape3D<float>>);
            AIShape(std::unique_ptr<ConvexShape3D<float>>, const Transform<float>& localTransform);

            const ConvexShape3D<float>& getShape() const;
            bool hasLocalTransform() const;
            const Transform<float>& getLocalTransform() const;

        private:
            std::unique_ptr<ConvexShape3D<float>> shape;

            bool bHasLocalTransform;
            Transform<float> localTransform;
    };

}
