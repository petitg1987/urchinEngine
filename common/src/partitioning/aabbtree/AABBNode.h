#pragma once

#include <memory>

#include <partitioning/aabbtree/AABBNodeData.h>
#include <math/geometry/3d/object/AABBox.h>

namespace urchin {

    template<class OBJ> class AABBTree;

    template<class OBJ> class AABBNode : public std::enable_shared_from_this<AABBNode<OBJ>> {
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

            void setParent(std::shared_ptr<AABBNode<OBJ>>);
            std::shared_ptr<AABBNode<OBJ>> getParent() const;

            void setLeftChild(std::shared_ptr<AABBNode<OBJ>>);
            std::shared_ptr<AABBNode<OBJ>> getLeftChildSmartPtr() const;
            void setRightChild(std::shared_ptr<AABBNode<OBJ>>);
            std::shared_ptr<AABBNode<OBJ>> getRightChildSmartPtr() const;

            std::shared_ptr<AABBNode<OBJ>> getSibling() const;

        private:
            std::unique_ptr<AABBNodeData<OBJ>> nodeData;
            AABBox<float> aabbox;

            std::shared_ptr<AABBNode<OBJ>> parentNode;
            std::shared_ptr<AABBNode<OBJ>> children[2];
    };

    #include "AABBNode.inl"

}
