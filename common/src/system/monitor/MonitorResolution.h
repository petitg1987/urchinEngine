#pragma once

#include <string>

namespace urchin {

    class MonitorResolution {
        public:
            MonitorResolution(unsigned int width, unsigned int, unsigned int);
            static MonitorResolution buildFromId(const std::string&);

            std::string getId() const;

            unsigned int getWidth() const;
            unsigned int getHeight() const;
            unsigned int getFrequency() const;

            std::string userDisplay(bool) const;
            std::string retrieveAspectRatio() const;

        private:
            static const char ID_DELIMITER;

            unsigned int width;
            unsigned int height;
            unsigned int frequency;
    };

}
