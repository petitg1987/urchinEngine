#pragma once

#include <list>
#include <UrchinCommon.h>
#include <UrchinAggregation.h>

#include "controller/AbstractController.h"

namespace urchin {

    class TerrainController final : public AbstractController {
        public:
            std::list<const TerrainEntity*> getTerrainEntities() const;
            void addTerrainEntity(std::unique_ptr<TerrainEntity>);
            void removeTerrainEntity(const TerrainEntity&);

            const TerrainEntity& updateTerrainGeneralProperties(const TerrainEntity&, const Point3<float>&, float, uint8_t);
            const TerrainEntity& updateTerrainMesh(const TerrainEntity&, float, float, TerrainMeshMode);
            const TerrainEntity& updateTerrainMaterial(const TerrainEntity&, float, float, std::string, const std::vector<std::string>&);

        private:
            TerrainEntity& findTerrainEntity(const TerrainEntity&) const;
    };

}
