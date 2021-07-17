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

            void setup(Renderer3d*);

            std::string getName() const;
            void setName(const std::string&);

            Water* getWater() const;
            void setWater(std::shared_ptr<Water>);

        private:
            void loadFrom(const UdaChunk*, const UdaParser&);
            void writeOn(UdaChunk&, UdaWriter&) const;

            static constexpr char NAME_ATTR[] = "name";

            Renderer3d* renderer3d;

            std::string name;
            std::shared_ptr<Water> water;
    };

}
