#pragma once

#include <scene/ui/allocator/StringConverterAllocator.h>

namespace urchin{

    class Clipboard {
        public:
            void setText(U32StringA);
            const U32StringA& getText() const;

        private:
            U32StringA text;
    };
}
