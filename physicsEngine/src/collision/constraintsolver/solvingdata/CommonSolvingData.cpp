#include <collision/constraintsolver/solvingdata/CommonSolvingData.h>

namespace urchin {

    CommonSolvingData::CommonSolvingData(const RigidBody& body1, const RigidBody& body2) :
            body1(body1),
            body2(body2),
            depth(0.0f) {

    }

}
