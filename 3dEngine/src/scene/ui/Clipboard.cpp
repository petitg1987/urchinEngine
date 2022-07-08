#include <scene/ui/Clipboard.h>

namespace urchin {

    void Clipboard::setText(U32StringA text) {
        this->text = std::move(text);
    }

    const U32StringA& Clipboard::getText() const {
        return text;
    }

}
