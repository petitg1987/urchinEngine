#ifndef URCHINENGINE_NAVJUMPCONSTRAINT_H
#define URCHINENGINE_NAVJUMPCONSTRAINT_H

namespace urchin
{

    class NavJumpConstraint
    {
        public:
            NavJumpConstraint(float, float, float);

            float getSourceEdgeJumpStartRange() const;
            float getSourceEdgeJumpEndRange() const;

            float getTargetEdgeIndex() const;

        private:
            float sourceEdgeJumpStartRange;
            float sourceEdgeJumpEndRange;

            float targetEdgeIndex;
    };

}

#endif
