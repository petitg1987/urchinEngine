#ifndef URCHINENGINE_EDGEJUMPRESULT_H
#define URCHINENGINE_EDGEJUMPRESULT_H

namespace urchin
{

    class EdgeJumpResult
    {
        public:
            static EdgeJumpResult noEdgeJump();
            static EdgeJumpResult edgeJump(float, float);

            bool hasJumpRange() const;
            float getJumpStartRange() const;
            float getJumpEndRange() const;

        private:
            EdgeJumpResult(bool, float, float);

            bool bHasJumpRange;

            float jumpStartRange;
            float jumpEndRange;
    };

}

#endif
