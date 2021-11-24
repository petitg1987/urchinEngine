#pragma once

#include <memory>
#include <cassert>

#include <partitioning/aabbtree/AABBNodeData.h>
#include <math/geometry/3d/object/AABBox.h>

namespace urchin {

    template<class OBJ> class AABBTree;

    template<class OBJ> class AABBNode {
        public:
            friend class AABBTree<OBJ>;

            explicit AABBNode(std::unique_ptr<AABBNodeData<OBJ>>);

            AABBNodeData<OBJ>& getNodeData() const;

            bool isLeaf() const;
            bool isRoot() const;

            AABBNode<OBJ>* getLeftChild() const;
            AABBNode<OBJ>* getRightChild() const;

            const AABBox<float>& getAABBox() const;

        protected:
            void updateAABBox(float);

            std::unique_ptr<AABBNodeData<OBJ>> moveNodeData();

            void setParent(AABBNode<OBJ>*);
            AABBNode<OBJ>* getParent() const;

            void setLeftChild(std::shared_ptr<AABBNode<OBJ>>);
            std::shared_ptr<AABBNode<OBJ>> getLeftChildSmartPtr() const;
            void setRightChild(std::shared_ptr<AABBNode<OBJ>>);
            std::shared_ptr<AABBNode<OBJ>> getRightChildSmartPtr() const;

            std::shared_ptr<AABBNode<OBJ>> getSibling() const;

        private:
            std::unique_ptr<AABBNodeData<OBJ>> nodeData;
            AABBox<float> aabbox;

            AABBNode<OBJ>* parentNode;
            std::array<std::shared_ptr<AABBNode<OBJ>>, 2> children;
    };

    #include "AABBNode.inl"

}
