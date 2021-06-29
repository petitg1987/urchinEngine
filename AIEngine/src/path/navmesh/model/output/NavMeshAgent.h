#pragma once

#include <UrchinCommon.h>

namespace urchin {

    class NavMeshAgent {
        public:
            NavMeshAgent();
            NavMeshAgent(float, float);

            float getAgentHeight() const;

            float getAgentRadius() const;

            void setMaxSlope(float);
            float getMaxSlope() const;

            void setJumpDistance(float);
            float getJumpDistance() const;

            CylinderShape<float> getAgentCylinder() const;
            BoxShape<float> getAgentBox() const;
            float computeExpandDistance(const Vector3<float>&) const;

        private:
            static const float DEFAULT_AGENT_HEIGHT;
            static const float DEFAULT_AGENT_RADIUS;
            static const float DEFAULT_MAX_SLOPE;
            static const float DEFAULT_JUMP_DISTANCE;

            float agentHeight;
            float agentRadius;
            float maxSlopeInRadian;
            float jumpDistance;
    };

}
