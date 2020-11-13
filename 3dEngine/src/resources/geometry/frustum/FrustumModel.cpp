#include <utility>

#include "resources/geometry/frustum/FrustumModel.h"

namespace urchin {

    FrustumModel::FrustumModel(Frustum<float> frustum):
            frustum(std::move(frustum)) {
        initialize();
    }

    Matrix4<float> FrustumModel::retrieveModelMatrix() const {
        return {};
    }

    std::vector<Point3<float>> FrustumModel::retrieveVertexArray() const {
        const Point3<float> &ntl = frustum.getFrustumPoint(Frustum<float>::NTL);
        const Point3<float> &ntr = frustum.getFrustumPoint(Frustum<float>::NTR);
        const Point3<float> &nbl = frustum.getFrustumPoint(Frustum<float>::NBL);
        const Point3<float> &nbr = frustum.getFrustumPoint(Frustum<float>::NBR);
        const Point3<float> &ftl = frustum.getFrustumPoint(Frustum<float>::FTL);
        const Point3<float> &ftr = frustum.getFrustumPoint(Frustum<float>::FTR);
        const Point3<float> &fbl = frustum.getFrustumPoint(Frustum<float>::FBL);
        const Point3<float> &fbr = frustum.getFrustumPoint(Frustum<float>::FBR);

        std::vector<Point3<float>> vertexArray;
        vertexArray.reserve(24);

        //near plane
        vertexArray.push_back(ntl);
        vertexArray.push_back(ntr);

        vertexArray.push_back(ntr);
        vertexArray.push_back(nbr);

        vertexArray.push_back(nbr);
        vertexArray.push_back(nbl);

        vertexArray.push_back(nbl);
        vertexArray.push_back(ntl);

        //far plane
        vertexArray.push_back(ftl);
        vertexArray.push_back(ftr);

        vertexArray.push_back(ftr);
        vertexArray.push_back(fbr);

        vertexArray.push_back(fbr);
        vertexArray.push_back(fbl);

        vertexArray.push_back(fbl);
        vertexArray.push_back(ftl);

        //sides
        vertexArray.push_back(ntl);
        vertexArray.push_back(ftl);

        vertexArray.push_back(ntr);
        vertexArray.push_back(ftr);

        vertexArray.push_back(nbl);
        vertexArray.push_back(fbl);

        vertexArray.push_back(nbr);
        vertexArray.push_back(fbr);

        return vertexArray;
    }

    ShapeType FrustumModel::getShapeType() const {
        return ShapeType::LINE;
    }

}
