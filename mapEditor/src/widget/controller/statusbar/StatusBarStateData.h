#pragma once

#include <vector>
#include <string>
#include <widget/controller/statusbar/StatusBarState.h>

namespace urchin {

    class StatusBarStateData {
        public:
            StatusBarStateData(const std::vector<std::string>&, StatusBarState);

            const std::vector<std::string>& getLabels();
            StatusBarState getPreviousState();

        private:
            std::vector<std::string> labels;
            StatusBarState previousState;
    };

}
