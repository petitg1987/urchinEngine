#pragma once

#include <string>
#include <Urchin3dEngine.h>

#include <resources/common/SceneEntity.h>

namespace urchin {

    class SceneWater {
        public:
            friend class Map;

            SceneWater();
            ~SceneWater();

            void setWaterManagers(Renderer3d*);

            std::string getName() const;
            void setName(const std::string&);

            Water* getWater() const;
            void setWater(Water*);

        private:
            void loadFrom(const XmlChunk*, const DataParser&);
            void writeOn(XmlChunk*, XmlWriter&) const;

            static constexpr char NAME_ATTR[] = "name";

            Renderer3d* renderer3d;

            std::string name;
            Water* water;
    };

}
