#pragma once

#include <scene/ui/allocator/StringConverterAllocator.h>
#include <scene/ui/Clipboard.h>

namespace urchin{

    class ClipboardLocal : public Clipboard {
        public:
            void setText(std::string) override;
            const std::string& getText() const override;

        private:
            std::string text;
    };
}
