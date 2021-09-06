#pragma once

#include <string>

namespace urchin {

    class MonitorResolution {
        public:
            MonitorResolution(unsigned int width, unsigned int, unsigned int);
            static MonitorResolution buildFromId(const std::string&);

            bool operator ==(const MonitorResolution&) const;
            bool operator !=(const MonitorResolution&) const;

            std::string getId() const;

            unsigned int getWidth() const;
            unsigned int getHeight() const;
            unsigned int getFrequency() const;

            std::string userDisplay(bool) const;
            std::string retrieveAspectRatio() const;

        private:
            static constexpr char ID_DELIMITER = '_';

            unsigned int width;
            unsigned int height;
            unsigned int frequency;
    };

}
