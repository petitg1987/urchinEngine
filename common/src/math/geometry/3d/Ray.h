#pragma once

#include <array>

#include <math/algebra/point/Point3.h>
#include <math/algebra/vector/Vector3.h>

namespace urchin {

    /**
     * A ray is similar to a line segment, but it has a clearly defined origin/starting point
     */
    template<class T> class Ray {
        public:
            Ray(const Point3<T>&, const Vector3<T>&, T);
            Ray(const Point3<T>&, const Point3<T>&);

            const Point3<T>& getOrigin() const;
            Point3<T> computeTo() const;

            const Vector3<T>& getDirection() const;
            T getLength() const;

            const Vector3<T>& getInverseDirection() const;
            unsigned int getDirectionSign(unsigned int) const;

        private:
            void initializeAdditionalData();

            Point3<T> origin;
            Vector3<T> direction; //normalized
            T length;

            //additional data for performance (ray tracing, collision...)
            Vector3<T> inverseDirection;
            std::array<unsigned int, 3> directionSigns;
    };

}
