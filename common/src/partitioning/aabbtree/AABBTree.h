#pragma once

#include "partitioning/aabbtree/AABBNode.h"
#include "partitioning/aabbtree/AABBNodeData.h"
#include "math/geometry/3d/Ray.h"

namespace urchin {

    template<class T> using objects_node_it = typename std::map<void*, std::shared_ptr<AABBNode<T>>>::iterator;

    template<class OBJ> class AABBTree {
        public:
            explicit AABBTree(float);
            virtual ~AABBTree() = default;

            void updateFatMargin(float);

            AABBNode<OBJ>* getRootNode() const;
            AABBNodeData<OBJ>& getNodeData(void*) const;
            void getAllNodeObjects(std::vector<OBJ>&) const;

            void addObject(std::unique_ptr<AABBNodeData<OBJ>>);
            virtual void postAddObjectCallback(AABBNode<OBJ>&);

            objects_node_it<OBJ> removeObject(AABBNodeData<OBJ>&);
            objects_node_it<OBJ> removeObject(OBJ);
            virtual void preRemoveObjectCallback(AABBNode<OBJ>&);

            void updateObjects();
            virtual void preUpdateObjectCallback(AABBNode<OBJ>&);

            void aabboxQuery(const AABBox<float>&, std::vector<OBJ>&) const;
            void rayQuery(const Ray<float>&, std::vector<OBJ>&) const;
            void enlargedRayQuery(const Ray<float>&, float, const void*, std::vector<OBJ>&) const;

        protected:
            std::map<void*, std::shared_ptr<AABBNode<OBJ>>> objectsNode;
            mutable std::vector<AABBNode<OBJ>*> browseNodes;

        private:
            std::vector<std::unique_ptr<AABBNodeData<OBJ>>> extractAllNodeData();
            void insertNode(std::shared_ptr<AABBNode<OBJ>>, std::shared_ptr<AABBNode<OBJ>>);
            void replaceNode(const AABBNode<OBJ>&, std::shared_ptr<AABBNode<OBJ>>);
            void removeLeafNode(AABBNode<OBJ>&);

            float fatMargin;
            std::shared_ptr<AABBNode<OBJ>> rootNode;
    };

    #include "AABBTree.inl"

}
