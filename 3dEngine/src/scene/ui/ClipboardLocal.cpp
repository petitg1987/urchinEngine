#include <scene/ui/ClipboardLocal.h>

namespace urchin {

    void ClipboardLocal::setText(const std::string& text) {
        this->text = text;
    }

    std::string ClipboardLocal::getText() const {
        return text;
    }

}
