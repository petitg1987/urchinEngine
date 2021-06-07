#pragma once

namespace urchin {

    class MonitorResolution {
        public:
            MonitorResolution(unsigned int, unsigned int, unsigned int);

        private:
            unsigned int width;
            unsigned int height;
            unsigned int rate;
    };

}
