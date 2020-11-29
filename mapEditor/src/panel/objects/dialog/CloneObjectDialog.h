#ifndef URCHINENGINE_CLONEOBJECTDIALOG_H
#define URCHINENGINE_CLONEOBJECTDIALOG_H

#include <list>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>

#include "UrchinMapHandler.h"
#include "controller/objects/ObjectController.h"

namespace urchin {

    class CloneObjectDialog : public QDialog {
            Q_OBJECT

        public:
            CloneObjectDialog(QWidget *parent, const ObjectController*);

            SceneObject *getSceneObject() const;

        private:
            void setupNameFields(QGridLayout *);

            void updateObjectName();
            int buildSceneObject(int);

            void done(int) override;
            bool isSceneObjectExist(const std::string&);

            const ObjectController *objectController;

            QLabel *objectNameLabel;
            QLineEdit *objectNameText;

            std::string objectName;
            SceneObject *sceneObject;
    };

}

#endif
