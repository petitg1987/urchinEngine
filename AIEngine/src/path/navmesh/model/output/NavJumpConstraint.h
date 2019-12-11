#ifndef URCHINENGINE_NAVJUMPCONSTRAINT_H
#define URCHINENGINE_NAVJUMPCONSTRAINT_H

namespace urchin
{

    class NavJumpConstraint
    {
        public:
            NavJumpConstraint(float, float, float);

            float getSourceEdgeStartPoint() const;
            float getSourceEdgeEndPoint() const;

            float getTargetEdgeIndex() const;

        private:
            float sourceEdgeStartPoint;
            float sourceEdgeEndPoint;

            float targetEdgeIndex;
    };

}

#endif
