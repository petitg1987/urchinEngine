#pragma once

#include <string>

namespace urchin {

    class MonitorResolution {
        public:
            MonitorResolution(unsigned int, unsigned int, unsigned int);

            unsigned int getWidth() const;
            unsigned int getHeight() const;
            unsigned int getFrequency() const;

            std::string userDisplay(bool) const;
            std::string retrieveAspectRatio() const;

        private:
            unsigned int width;
            unsigned int height;
            unsigned int frequency;
    };

}
