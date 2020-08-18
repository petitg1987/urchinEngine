#ifndef URCHINENGINE_AABBTREE_H
#define URCHINENGINE_AABBTREE_H

#include "partitioning/aabbtree/AABBNode.h"
#include "partitioning/aabbtree/AABBNodeData.h"
#include "math/geometry/3d/Ray.h"

#define BOUNDARIES_MARGIN_PERCENTAGE 0.3f

namespace urchin
{

    template<class OBJ> class AABBTree
    {
        public:
            explicit AABBTree(float);
            virtual ~AABBTree();

            void updateFatMargin(float);

            AABBNode<OBJ> *getRootNode() const;
            AABBNodeData<OBJ> *getNodeData(OBJ) const;
            void getAllNodeObjects(std::vector<OBJ> &) const;

            void addObject(AABBNodeData<OBJ> *);
            virtual void postAddObjectCallback(AABBNode<OBJ> *);

            void removeObject(AABBNodeData<OBJ> *);
            void removeObject(OBJ);
            virtual void preRemoveObjectCallback(AABBNode<OBJ> *);

            void updateObjects();
            virtual void preUpdateObjectCallback(AABBNode<OBJ> *);

            void aabboxQuery(const AABBox<float> &, std::vector<OBJ> &) const;
            void rayQuery(const Ray<float> &, std::vector<OBJ> &) const;
            void enlargedRayQuery(const Ray<float> &, float, const OBJ, std::vector<OBJ> &) const;

        protected:
            std::map<OBJ, AABBNode<OBJ> *> objectsNode;
            mutable std::vector<AABBNode<OBJ> *> browseNodes;

        private:
            std::vector<AABBNodeData<OBJ> *> extractAllNodeData();
            void insertNode(AABBNode<OBJ> *, AABBNode<OBJ> *);
            void replaceNode(AABBNode<OBJ> *, AABBNode<OBJ> *);
            void removeNode(AABBNode<OBJ> *);

            float fatMargin;
            AABBNode<OBJ> *rootNode;
    };

    #include "AABBTree.inl"

}

#endif
