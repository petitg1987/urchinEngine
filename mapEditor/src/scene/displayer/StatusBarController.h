#ifndef URCHINENGINE_STATUSBARCONTROLLER_H
#define URCHINENGINE_STATUSBARCONTROLLER_H

#include <QMainWindow>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>

namespace urchin
{

    class MapEditorWindow;

    class StatusBarController
    {
        public:
            explicit StatusBarController(QMainWindow *);

            void applyInitialState();
            void replaceStatus(const std::vector<std::string> &labels);

        private:
            QFrame *createSeparator();

            QMainWindow *window;
    };

}

#endif
