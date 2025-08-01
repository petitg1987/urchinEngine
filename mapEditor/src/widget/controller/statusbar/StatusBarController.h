#pragma once

#include <vector>
#include <QMainWindow>
#include <QtWidgets/QLabel>

#include "widget/controller/statusbar/StatusBarState.h"

namespace urchin {

    class StatusBarController {
        public:
            explicit StatusBarController(QMainWindow*);

            void clearState();
            void applyState(StatusBarState);

        private:
            static std::vector<std::string> getStateLabels(StatusBarState);

            void applyCurrentState();
            void addStatusBarWidget(QWidget*);
            static QFrame* createSeparator();

            StatusBarState currentState;
            QMainWindow* window;
            std::vector<QWidget*> statusBarWidgets;
    };

}
