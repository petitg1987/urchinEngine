#pragma once

#include <vector>
#include <QMainWindow>
#include <QtWidgets/QLabel>

#include "widget/controller/statusbar/StatusBarState.h"
#include "widget/controller/statusbar/StatusBarStateData.h"

namespace urchin {

    class StatusBarController {
        public:
            explicit StatusBarController(QMainWindow*);

            void clearState();
            void applyState(StatusBarState);
            void applyPreviousState();

        private:
            static StatusBarStateData getStateData(StatusBarState);

            void applyCurrentState();
            void addStatusBarWidget(QWidget*);
            static QFrame* createSeparator();

            StatusBarState currentState;
            QMainWindow* window;
            std::vector<QWidget*> statusBarWidgets;
    };

}
