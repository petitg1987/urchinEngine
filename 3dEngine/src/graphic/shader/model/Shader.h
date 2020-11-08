#ifndef URCHINENGINE_SHADER_H
#define URCHINENGINE_SHADER_H

namespace urchin {

    class Shader {
        public:
            Shader(unsigned int);
            ~Shader();

            unsigned int getShaderId() const;

            void bind() const;

        private:
            unsigned int shaderId;
    };

}

#endif
