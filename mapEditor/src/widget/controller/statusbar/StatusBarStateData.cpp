#include "StatusBarStateData.h"

namespace urchin {
    StatusBarStateData::StatusBarStateData(const std::vector<std::string>& labels, StatusBarState previousState) :
            labels(labels),
            previousState(previousState) {

    }

    const std::vector<std::string> &StatusBarStateData::getLabels() {
        return labels;
    }

    StatusBarState StatusBarStateData::getPreviousState() {
        return previousState;
    }
}
