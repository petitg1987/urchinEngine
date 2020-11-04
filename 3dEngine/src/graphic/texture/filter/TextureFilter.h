#ifndef URCHINENGINE_TEXTUREFILTER_H
#define URCHINENGINE_TEXTUREFILTER_H

#include <string>
#include <memory>
#include <limits>
#include <map>
#include <vector>

#include "graphic/displayer/generic/GenericDisplayer.h"

namespace urchin {

    template<class T> class TextureFilterBuilder;

    class TextureFilter {
        public:
            template<class BUILDER> explicit TextureFilter(const TextureFilterBuilder<BUILDER> *);
            virtual ~TextureFilter();

            void initialize();

            unsigned int getFboId() const;
            unsigned int getTextureID() const;

            void applyOn(unsigned int, unsigned int layersToUpdate = std::numeric_limits<unsigned int>::max()) const;

        protected:
            virtual std::string getShaderName() const = 0;
            virtual void initializeAdditionalUniforms(unsigned int);
            virtual void bindAdditionalTextures() const;
            virtual void completeShaderTokens(std::map<std::string, std::string> &) const = 0;

            unsigned int getTextureWidth() const;
            unsigned int getTextureHeight() const;

            unsigned int getTextureFilterShader() const;

            std::string toShaderVectorValues(std::vector<float> &) const;

        private:
            void initializeDisplay();
            void initializeTexture();

            bool isInitialized;

            unsigned int textureWidth, textureHeight;
            unsigned int textureType;
            unsigned int textureAccessFilter;
            float textureAnisotropy;
            unsigned int textureNumberLayer;
            int textureInternalFormat;
            unsigned int textureFormat;

            std::shared_ptr<GenericDisplayer> textureDisplayer;
            unsigned int textureFilterShader;
            int layersToUpdateLoc;

            unsigned int fboID;
            unsigned int textureID;
    };

    #include "TextureFilter.inl"

}

#endif
