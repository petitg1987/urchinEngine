#ifndef URCHINENGINE_EDGELINKRESULT_H
#define URCHINENGINE_EDGELINKRESULT_H

namespace urchin
{

    class EdgeLinkResult
    {
        public:
            static EdgeLinkResult noEdgeLink();
            static EdgeLinkResult collinearEdgeLink(float, float);
            static EdgeLinkResult edgeJump(float, float);

            bool hasEdgesLink() const;
            bool isJumpLink() const;

            float getLinkStartRange() const;
            float getLinkEndRange() const;

        private:
            EdgeLinkResult(bool, bool, float, float);

            bool bHasEdgesLink;
            bool bIsJumpLink;

            float linkStartRange;
            float linkEndRange;
    };

}

#endif
