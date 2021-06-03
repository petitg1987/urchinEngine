#ifndef URCHINENGINE_SKYPANELWIDGET_H
#define URCHINENGINE_SKYPANELWIDGET_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QDoubleSpinBox>

#include <controller/sky/SkyController.h>

namespace urchin {

    class SkyPanelWidget : public QWidget {
        Q_OBJECT

        public:
            SkyPanelWidget();
            ~SkyPanelWidget() override = default;

            void load(SkyController*);
            void unload();

        private:
            typedef void (SkyPanelWidget::*SkyControllerWidgetMemFn)();

            void setupSkyboxBox(QVBoxLayout*);
            QHBoxLayout* createFilenameInputText(QGridLayout*, int, const QString&, QLineEdit **);
            QPushButton* createSelectFileButton(QHBoxLayout*);
            QPushButton* createClearFileButton(QHBoxLayout*);

            void setupSkyDataFrom(const std::unique_ptr<Skybox>&);

            SkyController* skyController;

            QLineEdit* xnSkyboxFilenameText;
            QLineEdit* xpSkyboxFilenameText;
            QLineEdit* ynSkyboxFilenameText;
            QLineEdit* ypSkyboxFilenameText;
            QLineEdit* znSkyboxFilenameText;
            QLineEdit* zpSkyboxFilenameText;
            static QString preferredSkyboxPath;
            QDoubleSpinBox* offsetY;

            bool disableSkyEvent;

        private slots:
            void showXnSkyboxFilenameDialog();
            void showXpSkyboxFilenameDialog();
            void showYnSkyboxFilenameDialog();
            void showYpSkyboxFilenameDialog();
            void showZnSkyboxFilenameDialog();
            void showZpSkyboxFilenameDialog();
            void showSkyboxFilenameDialog(QLineEdit*);

            void clearXnSkyboxFilename();
            void clearXpSkyboxFilename();
            void clearYnSkyboxFilename();
            void clearYpSkyboxFilename();
            void clearZnSkyboxFilename();
            void clearZpSkyboxFilename();
            void clearSkyboxFilename(QLineEdit*);

            void skyChanged();
    };

}

#endif
