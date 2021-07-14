#pragma once

#include <set>
#include <UrchinCommon.h>

#include <body/model/AbstractBody.h>
#include <collision/broadphase/PairContainer.h>

namespace urchin {

    class BodyAABBNodeData : public AABBNodeData<AbstractBody*> {
        public:
            explicit BodyAABBNodeData(AbstractBody*);

            std::unique_ptr<AABBNodeData<AbstractBody*>> clone() const override;

            const std::string& getObjectId() const override;
            AABBox<float> retrieveObjectAABBox() const override;
            bool isObjectMoving() const override;

            bool isGhostBody() const;
            PairContainer* getBodyPairContainer() const;

            void addOwnerPairContainer(PairContainer*);
            void removeOwnerPairContainer(PairContainer*);
            std::set<PairContainer*> getOwnerPairContainers() const;

        private:
            PairContainer* bodyPairContainer;
            std::set<PairContainer*> ownerPairContainers;
    };

}
