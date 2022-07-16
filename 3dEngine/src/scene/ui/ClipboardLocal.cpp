#include <scene/ui/ClipboardLocal.h>

namespace urchin {

    void ClipboardLocal::setText(std::string text) {
        this->text = std::move(text);
    }

    const std::string& ClipboardLocal::getText() const {
        return text;
    }

}
