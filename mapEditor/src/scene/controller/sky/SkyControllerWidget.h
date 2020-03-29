#ifndef URCHINENGINE_SKYCONTROLLERWIDGET_H
#define URCHINENGINE_SKYCONTROLLERWIDGET_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QDoubleSpinBox>

#include "scene/controller/sky/SkyController.h"

namespace urchin
{

    class SkyControllerWidget : public QWidget
    {
        Q_OBJECT

        public:
            SkyControllerWidget();
            ~SkyControllerWidget() override = default;

            void load(SkyController *);
            void unload();

        private:
            typedef void (SkyControllerWidget::*SkyControllerWidgetMemFn)();

            void setupSkyboxBox(QVBoxLayout *);
            QHBoxLayout *createFilenameInputText(QGridLayout *, int, const QString &, QLineEdit **);
            QPushButton *createSelectFileButton(QHBoxLayout *);
            QPushButton *createClearFileButton(QHBoxLayout *);

            void setupSkyDataFrom(const std::unique_ptr<Skybox> &);

            SkyController *skyController;

            QLineEdit *xnSkyboxFilenameText, *xpSkyboxFilenameText;
            QLineEdit *ynSkyboxFilenameText, *ypSkyboxFilenameText;
            QLineEdit *znSkyboxFilenameText, *zpSkyboxFilenameText;
            static QString preferredSkyboxPath;
            QDoubleSpinBox *offsetY;

            bool disableSkyEvent;

        private slots:
            void showXnSkyboxFilenameDialog();
            void showXpSkyboxFilenameDialog();
            void showYnSkyboxFilenameDialog();
            void showYpSkyboxFilenameDialog();
            void showZnSkyboxFilenameDialog();
            void showZpSkyboxFilenameDialog();
            void showSkyboxFilenameDialog(QLineEdit *);

            void clearXnSkyboxFilename();
            void clearXpSkyboxFilename();
            void clearYnSkyboxFilename();
            void clearYpSkyboxFilename();
            void clearZnSkyboxFilename();
            void clearZpSkyboxFilename();
            void clearSkyboxFilename(QLineEdit *);

            void skyChanged();
    };

}

#endif
