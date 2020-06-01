#ifndef URCHINENGINE_STATUSBARCONTROLLER_H
#define URCHINENGINE_STATUSBARCONTROLLER_H

#include <memory>
#include <QMainWindow>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>

#include "scene/displayer/widget/statusbar/StatusBarState.h"
#include "scene/displayer/widget/statusbar/StatusBarStateData.h"

namespace urchin
{

    class StatusBarController
    {
        public:
            explicit StatusBarController(QMainWindow *);

            void clearState();
            void applyState(StatusBarState);
            void applyPreviousState();

        private:
            StatusBarStateData getStateData(StatusBarState);

            void applyCurrentState();
            QFrame *createSeparator();

            StatusBarState currentState;
            QMainWindow *window;
    };

}

#endif
