#include "widget/controller/statusbar/StatusBarStateData.h"

#include <utility>

namespace urchin {
    StatusBarStateData::StatusBarStateData(std::vector<std::string> labels, StatusBarState previousState) :
            labels(std::move(labels)),
            previousState(previousState) {

    }

    const std::vector<std::string>& StatusBarStateData::getLabels() const {
        return labels;
    }

    StatusBarState StatusBarStateData::getPreviousState() const {
        return previousState;
    }
}
