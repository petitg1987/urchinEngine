#pragma once

#include <string>

namespace urchin {

    class MonitorResolution {
        public:
            MonitorResolution(unsigned int, unsigned int, unsigned int);
            explicit MonitorResolution(const std::string&);
            MonitorResolution(const MonitorResolution&);
            MonitorResolution& operator=(const MonitorResolution&);

            std::string getId() const;

            unsigned int getWidth() const;
            unsigned int getHeight() const;
            unsigned int getFrequency() const;

            std::string userDisplay(bool) const;
            std::string retrieveAspectRatio() const;

        private:
            const char ID_DELIMITER;

            unsigned int width;
            unsigned int height;
            unsigned int frequency;
    };

}
