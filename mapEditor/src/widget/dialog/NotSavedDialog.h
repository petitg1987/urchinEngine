#pragma once

#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>

namespace urchin {

    class NotSavedDialog final : public QDialog {
        Q_OBJECT

        public:
            explicit NotSavedDialog(QWidget* = nullptr);

            bool needSave() const;

        private:
            bool bNeedSave;
            QDialogButtonBox* buttonBox;

        private slots:
            void buttonClick(QAbstractButton*);
    };

}
