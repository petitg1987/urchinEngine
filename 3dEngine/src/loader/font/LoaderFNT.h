#ifndef URCHINENGINE_LOADERFNT_H
#define URCHINENGINE_LOADERFNT_H

#include <string>

#include "resources/font/Font.h"
#include "loader/Loader.h"

namespace urchin {

    class LoaderFNT : public Loader<Font> {
        public:
            ~LoaderFNT() override = default;

            Font *loadFromFile(const std::string&) override;
    };

}

#endif
