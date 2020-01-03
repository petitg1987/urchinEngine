#ifndef URCHINENGINE_EDGEJUMPRESULT_H
#define URCHINENGINE_EDGEJUMPRESULT_H

namespace urchin
{

    class EdgeJumpResult
    {
        public:
            static EdgeJumpResult noEdgeJump();
            static EdgeJumpResult collinearNoEdgeJump();
            static EdgeJumpResult edgeJump(float, float);
            static EdgeJumpResult collinearEdgeJump(float, float);

            bool isCollinearEdgeJump() const;

            bool hasJumpRange() const;
            float getJumpStartRange() const;
            float getJumpEndRange() const;

        private:
            EdgeJumpResult(bool, bool, float, float);

            bool bIsCollinearEdgeJump;

            bool bHasJumpRange;
            float jumpStartRange;
            float jumpEndRange;
    };

}

#endif
