#include "StatusBarController.h"
#include <QtWidgets/QStatusBar>

namespace urchin {

    StatusBarController::StatusBarController(QMainWindow* window) :
        currentState(StatusBarState::NONE),
        window(window) {
        applyCurrentState();
    }

    void StatusBarController::clearState() {
        auto* statusBar = new QStatusBar(window);
        window->setStatusBar(statusBar);
    }

    void StatusBarController::applyState(StatusBarState state) {
        currentState = state;
        applyCurrentState();
    }

    void StatusBarController::applyPreviousState() {
        StatusBarState previousState = getStateData(currentState).getPreviousState();
        if (previousState == currentState) {
            throw new std::runtime_error("No previous state for current state: " + std::to_string(currentState));
        }

        currentState = previousState;
        applyCurrentState();
    }

    StatusBarStateData StatusBarController::getStateData(StatusBarState state) {
        switch(state) {
            case StatusBarState::NONE:
                return StatusBarStateData({}, StatusBarState::NONE);
            case StatusBarState::MAP_LOADED:
                return StatusBarStateData({"Select object: LMB"}, StatusBarState::NONE);
            case StatusBarState::OBJECT_SELECTED:
                return StatusBarStateData({"Select object: LMB",
                                           "Move selected: Ctrl + X/Y/Z"}, StatusBarState::MAP_LOADED);
            case StatusBarState::OBJECT_MOVE:
                return StatusBarStateData({"Move selected: mouse",
                                           "Switch axis: Ctrl + X/Y/Z",
                                           "Confirm move: LMB",
                                           "Cancel move: Esc"}, StatusBarState::OBJECT_SELECTED);
            default:
                throw std::runtime_error("Unknown status bar state: " + std::to_string(state));
        }
    }

    void StatusBarController::applyCurrentState() {
        auto* statusBar = new QStatusBar(window);
        window->setStatusBar(statusBar);

        auto statusLabels = getStateData(currentState).getLabels();
        for (std::size_t i=0; i<statusLabels.size(); ++i) {
            statusBar->addWidget(new QLabel(QString(statusLabels[i].c_str())));
            if (i != statusLabels.size() - 1) {
                statusBar->addWidget(createSeparator());
            }
        }
    }

    QFrame* StatusBarController::createSeparator() {
        QFrame* separator = new QFrame();
        separator->setFrameShape(QFrame::VLine);
        separator->setFrameShadow(QFrame::Shadow::Raised);
        return separator;
    }

}
