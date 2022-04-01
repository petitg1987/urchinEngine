#pragma once

#include <string>
#include <Urchin3dEngine.h>

namespace urchin {

    class WaterEntity {
        public:
            friend class Map;

            WaterEntity();
            ~WaterEntity();

            std::string getName() const;
            void setName(std::string);

            Water* getWater() const;
            void setWater(std::shared_ptr<Water>);

        private:
            void setup(Renderer3d*);

            Renderer3d* renderer3d;

            std::string name;
            std::shared_ptr<Water> water;
    };

}
