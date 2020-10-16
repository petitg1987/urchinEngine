#ifndef URCHINENGINE_DOWNSAMPLEFILTER_H
#define URCHINENGINE_DOWNSAMPLEFILTER_H

#include <string>
#include <map>

#include "utils/filter/TextureFilter.h"

namespace urchin {

    class DownSampleFilterBuilder;

    class DownSampleFilter : public TextureFilter {
        public:
            explicit DownSampleFilter(const DownSampleFilterBuilder *);

        private:
            std::string getShaderName() const override;
            void completeShaderTokens(std::map<std::string, std::string> &) const override;
    };

}

#endif
