#pragma once

#include <UrchinCommon.h>

#include <body/model/RigidBody.h>

namespace urchin {

    /**
    * Constraint common data container
    */
    struct CommonSolvingData {
        CommonSolvingData(const RigidBody&, const RigidBody&);

        const RigidBody& body1;
        const RigidBody& body2;

        Vector3<float> contactNormal;
        Vector3<float> contactTangent;

        Matrix3<float> invInertia1, invInertia2;

        Vector3<float> r1, r2; //vector from center of mass of body to contact point

        float depth; //penetration depth (negative when collision exist)
    };

}
