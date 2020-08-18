#ifndef URCHINENGINE_COMMONSOLVINGDATA_H
#define URCHINENGINE_COMMONSOLVINGDATA_H

#include "UrchinCommon.h"

#include "body/work/WorkRigidBody.h"

namespace urchin
{

    /**
    * Constraint common data container
    */
    struct CommonSolvingData
    {
        CommonSolvingData();

        const WorkRigidBody *body1, *body2;

        Vector3<float> contactNormal;
        Vector3<float> contactTangent;

        Matrix3<float> invInertia1, invInertia2;

        Vector3<float> r1, r2; //vector from center of mass of body to contact point

        float depth; //penetration depth (negative when collision exist)
    };

}

#endif
