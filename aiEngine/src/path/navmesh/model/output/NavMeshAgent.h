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
            static constexpr float DEFAULT_AGENT_HEIGHT = 1.8f;
            static constexpr float DEFAULT_AGENT_RADIUS = 0.25f;
            static constexpr float DEFAULT_MAX_SLOPE = (MathValue::PI_FLOAT / 4.0f);
            static constexpr float DEFAULT_JUMP_DISTANCE = 1.5f;

            float agentHeight;
            float agentRadius;
            float maxSlopeInRadian;
            float jumpDistance;
    };

}
