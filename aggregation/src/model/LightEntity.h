#pragma once

#include <string>
#include <Urchin3dEngine.h>

namespace urchin {

    class LightEntity {
        public:
            friend class Map;

            LightEntity();
            ~LightEntity();

            std::string getName() const;
            void setName(std::string);

            Light* getLight() const;
            void setLight(const std::shared_ptr<Light>&);

        private:
            void setup(Renderer3d*);

            Renderer3d* renderer3d;

            std::string name;
            std::shared_ptr<Light> light;
    };

}
