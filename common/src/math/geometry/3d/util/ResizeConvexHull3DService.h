#ifndef URCHINENGINE_RESIZECONVEXHULL3DSERVICE_H
#define URCHINENGINE_RESIZECONVEXHULL3DSERVICE_H

#include <memory>
#include <map>
#include <vector>

#include <math/geometry/3d/shape/ConvexHullShape3D.h>
#include <math/geometry/3d/object/ConvexHull3D.h>
#include <math/geometry/3d/Plane.h>
#include <math/algebra/point/Point3.h>

namespace urchin {

    template<class T> class ResizeConvexHull3DService {
        public:
            ResizeConvexHull3DService() = delete;

            static std::unique_ptr<ConvexHullShape3D<T>> resizeConvexHullShape(const ConvexHullShape3D<T>&, T);
            static std::unique_ptr<ConvexHull3D<T>> resizeConvexHull(const ConvexHull3D<T>&, T);

            static std::unique_ptr<ConvexHullShape3D<T>> resizeConvexHullShape(const ConvexHullShape3D<T>&, const std::map<std::size_t, Plane<T>>&);
            static std::unique_ptr<ConvexHull3D<T>> resizeConvexHull(const ConvexHull3D<T>&, const std::map<std::size_t, Plane<T>>&);

        private:
            static std::map<std::size_t, Plane<T>> buildPlanesFromConvexHullShape(const ConvexHullShape3D<T>&);
            static void shiftPlanes(std::map<std::size_t, Plane<T>>&, T);
            static std::vector<Plane<T>> findThreeNonParallelPlanes(const std::vector<std::size_t>&, const std::map<std::size_t, Plane<T>>&);
    };

}

#endif
