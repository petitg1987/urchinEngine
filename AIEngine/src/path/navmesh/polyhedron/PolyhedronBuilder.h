#ifndef URCHINENGINE_POLYHEDRONBUILDER_H
#define URCHINENGINE_POLYHEDRONBUILDER_H

#include <iostream>
#include "UrchinCommon.h"

#include "input/AIObject.h"
#include "path/navmesh/polyhedron/Polyhedron.h"
#include "path/navmesh/polyhedron/PolyhedronFace.h"
#include "path/navmesh/polyhedron/PolyhedronPoint.h"
#include "path/navmesh/model/NavMeshConfig.h"

namespace urchin
{

    class PolyhedronBuilder : public Singleton<PolyhedronBuilder>
    {
        public:
            friend class Singleton<PolyhedronBuilder>;

            std::vector<std::unique_ptr<Polyhedron>> buildPolyhedrons(const std::shared_ptr<AIObject> &);

        private:
            std::vector<PolyhedronPoint> createPolyhedronPoints(OBBox<float> *) const;
            std::vector<PolyhedronFace> createPolyhedronFaces() const;

            std::unique_ptr<Polyhedron> createPolyhedronFor(const std::string &, OBBox<float> *) const;
            std::unique_ptr<Polyhedron> createPolyhedronFor(const std::string &, Capsule<float> *) const;
            std::unique_ptr<Polyhedron> createPolyhedronFor(const std::string &, Cone<float> *) const;
            std::unique_ptr<Polyhedron> createPolyhedronFor(const std::string &, ConvexHull3D<float> *) const;
            std::unique_ptr<Polyhedron> createPolyhedronFor(const std::string &, Cylinder<float> *) const;
            std::unique_ptr<Polyhedron> createPolyhedronFor(const std::string &, Sphere<float> *) const;
    };

}

#endif
