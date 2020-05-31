#include "StatusBarController.h"
#include <QtWidgets/QStatusBar>

namespace urchin
{

    StatusBarController::StatusBarController(QMainWindow *window) :
        window(window)
    {

    }

    void StatusBarController::applyInitialState()
    {
        replaceStatus({"Select object: LMB"});
    }

    void StatusBarController::replaceStatus(const std::vector<std::string> &statusLabels)
    {
        auto *statusBar = new QStatusBar(window);
        window->setStatusBar(statusBar);

        for(std::size_t i=0; i<statusLabels.size(); ++i)
        {
            statusBar->addWidget(new QLabel(QString(statusLabels[i].c_str())));
            if(i != statusLabels.size() - 1)
            {
                statusBar->addWidget(createSeparator());
            }
        }
    }

    QFrame *StatusBarController::createSeparator()
    {
        QFrame *separator = new QFrame();
        separator->setFrameShape(QFrame::VLine);
        separator->setFrameShadow(QFrame::Shadow::Raised);
        return separator;
    }

}
