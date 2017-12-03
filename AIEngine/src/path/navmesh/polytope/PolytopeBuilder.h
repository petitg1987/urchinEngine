#ifndef URCHINENGINE_POLYTOPEBUILDER_H
#define URCHINENGINE_POLYTOPEBUILDER_H

#include <iostream>
#include <memory>
#include "UrchinCommon.h"

#include "input/AIObject.h"
#include "input/AITerrain.h"
#include "path/navmesh/polytope/Polytope.h"
#include "path/navmesh/polytope/PolytopeSurface.h"
#include "path/navmesh/polytope/PolytopePoint.h"
#include "path/navmesh/model/NavMeshConfig.h"

namespace urchin
{

    class PolytopeBuilder : public Singleton<PolytopeBuilder>
    {
        public:
            friend class Singleton<PolytopeBuilder>;

            std::vector<std::unique_ptr<Polytope>> buildPolytopes(const std::shared_ptr<AIObject> &);
            std::unique_ptr<Polytope> buildPolytope(const std::shared_ptr<AITerrain> &);

        private:
            std::vector<PolytopePoint> createPolytopePoints(OBBox<float> *) const;
            std::vector<std::unique_ptr<PolytopeSurface>> createPolytopeSurfaces(const std::vector<PolytopePoint> &) const;

            std::unique_ptr<Polytope> createPolytopeFor(const std::string &, OBBox<float> *) const;
            std::unique_ptr<Polytope> createPolytopeFor(const std::string &, Capsule<float> *) const;
            std::unique_ptr<Polytope> createPolytopeFor(const std::string &, Cone<float> *) const;
            std::unique_ptr<Polytope> createPolytopeFor(const std::string &, ConvexHull3D<float> *) const;
            std::unique_ptr<Polytope> createPolytopeFor(const std::string &, Cylinder<float> *) const;
            std::unique_ptr<Polytope> createPolytopeFor(const std::string &, Sphere<float> *) const;
    };

}

#endif
