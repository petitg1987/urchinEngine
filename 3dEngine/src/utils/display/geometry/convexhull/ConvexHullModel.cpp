#include <GL/glew.h>
#include <utility>

#include "utils/display/geometry/convexhull/ConvexHullModel.h"

namespace urchin {

    ConvexHullModel::ConvexHullModel(ConvexHull3D<float> convexHull):
            convexHull(std::move(convexHull)) {
        initialize();
    }

    Matrix4<float> ConvexHullModel::retrieveModelMatrix() const {
        return {};
    }

    std::vector<Point3<float>> ConvexHullModel::retrieveVertexArray() const {
        std::vector<Point3<float>> vertexArray;
        vertexArray.reserve(convexHull.getIndexedTriangles().size()*3);

        auto &convexHullPoints = convexHull.getConvexHullPoints();
        for (auto &it : convexHull.getIndexedTriangles()) {
            IndexedTriangle3D<float> triangle = it.second;

            Point3<float> point1 = convexHullPoints.at(triangle.getIndex(0)).point;
            Point3<float> point2 = convexHullPoints.at(triangle.getIndex(1)).point;
            Point3<float> point3 = convexHullPoints.at(triangle.getIndex(2)).point;

            vertexArray.push_back(point1);
            vertexArray.push_back(point2);
            vertexArray.push_back(point3);
        }

        return vertexArray;
    }

    void ConvexHullModel::drawGeometry() const {
        glDrawArrays(GL_TRIANGLES, 0, convexHull.getIndexedTriangles().size()*3);
    }

}
