#pragma once

#include <memory>

#include <partitioning/aabbtree/AABBNodeData.h>
#include <math/geometry/3d/object/AABBox.h>

namespace urchin {

    template<class OBJ> class AABBTree;

    template<class OBJ> class AABBNode : public std::enable_shared_from_this<AABBNode<OBJ>> {
        public:
            friend class AABBTree<OBJ>;

            explicit AABBNode(AABBNodeData<OBJ>*);
            ~AABBNode();

            AABBNodeData<OBJ>* getNodeData() const;

            bool isLeaf() const;
            bool isRoot() const;

            void setLeftChild(std::shared_ptr<AABBNode<OBJ>>);
            AABBNode<OBJ>* getLeftChild() const;
            void setRightChild(std::shared_ptr<AABBNode<OBJ>>);
            AABBNode<OBJ>* getRightChild() const;

            const AABBox<float>& getAABBox() const;
            void updateAABBox(float);

        protected:
            void clearNodeData();
            std::shared_ptr<AABBNode<OBJ>> getParent() const;
            std::shared_ptr<AABBNode<OBJ>> getLeftChildSmartPtr() const;
            std::shared_ptr<AABBNode<OBJ>> getRightChildSmartPtr() const;
            std::shared_ptr<AABBNode<OBJ>> getSibling() const;

        private:
            void setParent(std::shared_ptr<AABBNode<OBJ>>);

            AABBNodeData<OBJ>* nodeData;
            AABBox<float> aabbox;

            std::shared_ptr<AABBNode<OBJ>> parentNode;
            std::shared_ptr<AABBNode<OBJ>> children[2];
    };

    #include "AABBNode.inl"

}
