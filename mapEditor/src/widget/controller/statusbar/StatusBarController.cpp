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

    std::vector<std::string> StatusBarController::getStateLabels(StatusBarState state) {
        switch(state) {
            case NONE:
                return {};
            case MAP_LOADED:
                return {"Select model: LMB"};
            case MODEL_SELECTED:
                return {"Select model: LMB", "Move selected: Ctrl + X/Y/Z"};
            case MODEL_MOVE:
                return {"Move selected: mouse", "Switch axis: Ctrl + X/Y/Z", "Confirm move: LMB", "Cancel move: Esc"};
            default:
                throw std::runtime_error("Unknown status bar state: " + std::to_string(state));
        }
    }

    void StatusBarController::applyCurrentState() {
        clearState();

        auto stateLabels = getStateLabels(currentState);
        for (std::size_t i = 0; i < stateLabels.size(); ++i) {
            addStatusBarWidget(new QLabel(QString::fromStdString(stateLabels[i])));
            if (i != stateLabels.size() - 1) {
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
