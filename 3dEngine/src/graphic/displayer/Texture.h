#ifndef URCHINENGINE_TEXTURE_H
#define URCHINENGINE_TEXTURE_H

namespace urchin {

    class Texture { //TODO improve for texture parameter
        public:
            enum Type {
                SIMPLE,
                ARRAY,
                CUBE_MAP
            };

            static Texture build(unsigned int);
            static Texture build(unsigned int, Type);

            unsigned int getId() const;
            Type getType() const;

        private:
            Texture(unsigned int, Type);

            unsigned int id;
            Type type;

    };

}

#endif
