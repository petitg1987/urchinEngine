#ifndef ENGINE_NAVMESHAGENT_H
#define ENGINE_NAVMESHAGENT_H

#include "UrchinCommon.h"

namespace urchin {

    class NavMeshAgent
    {
        public:
            NavMeshAgent(float, float);

            float getAgentHeight() const;
            float getAgentRadius() const;

            CylinderShape<float> getAgentCylinder() const;
            BoxShape<float> getAgentBox() const;

            float computeExpandDistance(const Vector3<float> &) const;

        private:
            float agentHeight;
            float agentRadius;
    };

}

#endif
