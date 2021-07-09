#pragma once

#include <memory>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>

namespace urchin {

    class CollisionShapeReaderWriter {
        public:
            static constexpr char TYPE_ATTR[] = "type";
            static constexpr char SPHERE_VALUE[] = "SPHERE";
            static constexpr char CAPSULE_VALUE[] = "CAPSULE";
            static constexpr char CYLINDER_VALUE[] = "CYLINDER";
            static constexpr char CONE_VALUE[] = "CONE";
            static constexpr char BOX_VALUE[] = "BOX";
            static constexpr char CONVEX_HULL_VALUE[] = "CONVEX_HULL";
            static constexpr char COMPOUND_SHAPE_VALUE[] = "COMPOUND_SHAPE";

            virtual ~CollisionShapeReaderWriter() = default;

            virtual CollisionShape3D* loadFrom(const DataChunk*, const DataParser&) const = 0;
            virtual void writeOn(DataChunk*, const CollisionShape3D&, DataWriter&) const = 0;
    };

}
