#ifndef URCHINENGINE_EPARESULTINVALID_H
#define URCHINENGINE_EPARESULTINVALID_H

#include <stdexcept>
#include "UrchinCommon.h"

#include "EPAResult.h"

namespace urchin {

    /**
    * Invalid result of EPA algorithm. Invalid result is returned by EPA algorithm when no solution is found due to numerical imprecision.
    */
    template<class T> class EPAResultInvalid : public EPAResult<T> {
        public:
            bool isValidResult() const override;

            bool isCollide() const override;
            const Point3<T> &getContactPointA() const override;
            const Point3<T> &getContactPointB() const override;
            const Vector3<T> &getNormal() const override;
            T getPenetrationDepth() const override;
    };

}

#endif
