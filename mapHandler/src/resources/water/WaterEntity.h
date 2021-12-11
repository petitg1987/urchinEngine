#pragma once

#include <string>
#include <Urchin3dEngine.h>

namespace urchin {

    class WaterEntity {
        public:
            WaterEntity();
            ~WaterEntity();

            void setup(Renderer3d*);

            std::string getName() const;
            void setName(const std::string&);

            Water* getWater() const;
            void setWater(std::shared_ptr<Water>);

        private:
            Renderer3d* renderer3d;

            std::string name;
            std::shared_ptr<Water> water;
    };

}
