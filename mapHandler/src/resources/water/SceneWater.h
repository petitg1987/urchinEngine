#ifndef URCHINENGINE_SCENEWATER_H
#define URCHINENGINE_SCENEWATER_H

#include <string>
#include "Urchin3dEngine.h"

#include "resources/common/SceneEntity.h"

namespace urchin {

    class SceneWater {
        //XML attributes
        #define NAME_ATTR "name"

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
            void loadFrom(const std::shared_ptr<XmlChunk>&, const XmlParser&);
            void writeOn(const std::shared_ptr<XmlChunk>&, XmlWriter&) const;

            Renderer3d* renderer3d;

            std::string name;
            Water* water;
    };

}

#endif
