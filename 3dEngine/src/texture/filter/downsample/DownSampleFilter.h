#ifndef URCHINENGINE_DOWNSAMPLEFILTER_H
#define URCHINENGINE_DOWNSAMPLEFILTER_H

#include <string>
#include <map>

#include "texture/filter/TextureFilter.h"

namespace urchin {

    class DownSampleFilterBuilder;

    class DownSampleFilter : public TextureFilter {
        public:
            explicit DownSampleFilter(const DownSampleFilterBuilder*);

        private:
            std::string getShaderName() const override;
            std::unique_ptr<ShaderConstants> buildShaderConstants() const override;
    };

}

#endif
