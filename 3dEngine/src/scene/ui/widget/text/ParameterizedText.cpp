#include <scene/ui/widget/text/ParameterizedText.h>
#include <scene/ui/widget/text/Text.h>

namespace urchin {

    const std::string& ParameterizedText::getText() const {
        return text;
    }

    const std::vector<std::string>& ParameterizedText::getParameters() const {
        return parameters;
    }

}
