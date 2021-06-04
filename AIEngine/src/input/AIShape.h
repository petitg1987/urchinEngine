#pragma once

#include <UrchinCommon.h>

namespace urchin {

    class AIShape {
        public:
            explicit AIShape(const ConvexShape3D<float>*);
            AIShape(const ConvexShape3D<float>*, Transform<float> localTransform);
            ~AIShape();

            const ConvexShape3D<float>* getShape() const;
            bool hasLocalTransform() const;
            const Transform<float>& getLocalTransform() const;

        private:
            ConvexShape3D<float>* shape;

            bool bHasLocalTransform;
            Transform<float> localTransform;
    };

}
