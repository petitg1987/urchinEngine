#include <QtWidgets/QStatusBar>

#include "widget/controller/statusbar/StatusBarController.h"

namespace urchin {

    StatusBarController::StatusBarController(QMainWindow* window) :
            currentState(NONE),
            window(window) {
        window->setStatusBar(new QStatusBar(window));
        applyCurrentState();
    }

    void StatusBarController::clearState() {
        for (auto& statusBarWidget : statusBarWidgets) {
            window->statusBar()->removeWidget(statusBarWidget);
        }
        statusBarWidgets.clear();
    }

    void StatusBarController::applyState(StatusBarState state) {
        currentState = state;
        applyCurrentState();
    }

    void StatusBarController::applyPreviousState() {
        StatusBarState previousState = getStateData(currentState).getPreviousState();
        if (previousState == currentState) {
            throw std::runtime_error("No previous state for current state: " + std::to_string(currentState));
        }

        currentState = previousState;
        applyCurrentState();
    }

    StatusBarStateData StatusBarController::getStateData(StatusBarState state) {
        switch(state) {
            case NONE:
                return StatusBarStateData({}, NONE);
            case MAP_LOADED:
                return StatusBarStateData({"Select model: LMB"}, NONE);
            case MODEL_SELECTED:
                return StatusBarStateData({"Select model: LMB",
                                           "Move selected: Ctrl + X/Y/Z"}, MAP_LOADED);
            case MODEL_MOVE:
                return StatusBarStateData({"Move selected: mouse",
                                           "Switch axis: Ctrl + X/Y/Z",
                                           "Confirm move: LMB",
                                           "Cancel move: Esc"}, MODEL_SELECTED);
            default:
                throw std::runtime_error("Unknown status bar state: " + std::to_string(state));
        }
    }

    void StatusBarController::applyCurrentState() {
        clearState();

        auto statusLabels = getStateData(currentState).getLabels();
        for (std::size_t i = 0; i < statusLabels.size(); ++i) {
            addStatusBarWidget(new QLabel(QString(statusLabels[i].c_str())));
            if (i != statusLabels.size() - 1) {
                addStatusBarWidget(createSeparator());
            }
        }
    }

    void StatusBarController::addStatusBarWidget(QWidget* widget) {
        statusBarWidgets.push_back(widget);
        window->statusBar()->addWidget(widget);
    }

    QFrame* StatusBarController::createSeparator() {
        auto* separator = new QFrame();
        separator->setFrameShape(QFrame::VLine);
        separator->setFrameShadow(QFrame::Shadow::Raised);
        return separator;
    }

}
