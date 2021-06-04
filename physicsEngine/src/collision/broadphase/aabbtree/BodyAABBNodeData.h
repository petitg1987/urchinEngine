#pragma once

#include <set>
#include <UrchinCommon.h>

#include <body/model/AbstractBody.h>
#include <collision/broadphase/PairContainer.h>

namespace urchin {

    class BodyAABBNodeData : public AABBNodeData<AbstractBody*> {
        public:
            BodyAABBNodeData(AbstractBody*, PairContainer*);

            AABBNodeData<AbstractBody*>* clone() const override;

            const std::string& getObjectId() const override;
            AABBox<float> retrieveObjectAABBox() const override;
            bool isObjectMoving() const override;

            bool hasAlternativePairContainer() const;
            PairContainer* getAlternativePairContainer() const;

            void addOwnerPairContainer(PairContainer*);
            void removeOwnerPairContainer(PairContainer*);
            std::set<PairContainer*> getOwnerPairContainers() const;

        private:
            PairContainer* alternativePairContainer;
            std::set<PairContainer*> ownerPairContainers;
    };

}
