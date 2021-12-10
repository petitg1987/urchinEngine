#pragma once

#include <string>
#include <Urchin3dEngine.h>

namespace urchin {

    class LightEntity {
        public:
            friend class Map;

            LightEntity();
            ~LightEntity();

            void setup(LightManager&);

            std::string getName() const;
            void setName(const std::string&);

            Light* getLight() const;
            void setLight(const std::shared_ptr<Light>&);

        private:
            void loadFrom(const UdaChunk*, const UdaParser&);
            void writeOn(UdaChunk&, UdaWriter&) const;

            static constexpr char NAME_ATTR[] = "name";

            LightManager* lightManager;

            std::string name;
            std::shared_ptr<Light> light;
    };

}
