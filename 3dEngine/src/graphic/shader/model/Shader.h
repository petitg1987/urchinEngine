#ifndef URCHINENGINE_SHADER_H
#define URCHINENGINE_SHADER_H

namespace urchin {

    class Shader {
        public:
            explicit Shader(unsigned int);
            ~Shader();

            unsigned int getShaderId() const;

            void bind() const; //TODO make it protected ?

        private:
            unsigned int shaderId;
    };

}

#endif
