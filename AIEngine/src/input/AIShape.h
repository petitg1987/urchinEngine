#ifndef URCHINENGINE_AISHAPE_H
#define URCHINENGINE_AISHAPE_H

#include "UrchinCommon.h"

namespace urchin
{

    class AIShape
    {
        public:
            explicit AIShape(std::shared_ptr<const ConvexShape3D<float>>);
            AIShape(std::shared_ptr<const ConvexShape3D<float>>, Transform<float> localTransform);

            const std::shared_ptr<const ConvexShape3D<float>> &getShape() const;
            bool hasLocalTransform() const;
            const Transform<float> &getLocalTransform() const;

        private:
            std::shared_ptr<const ConvexShape3D<float>> shape;

            bool bHasLocalTransform;
            Transform<float> localTransform;
    };

}

#endif
