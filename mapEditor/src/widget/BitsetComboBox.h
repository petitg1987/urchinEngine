#pragma once

#include <QStandardItemModel>
#include <QtWidgets/QComboBox>

namespace urchin {

    class BitsetComboBox final : public QComboBox {
        Q_OBJECT

        public:
            BitsetComboBox(QWidget*, unsigned int, unsigned long);
            ~BitsetComboBox() override = default;

            unsigned long getBitValues() const;

        protected:
            void paintEvent(QPaintEvent*) override;

        signals:
            void onBitChanged();

        private:
            void addCheckItem(const QString&, bool) const;

            unsigned int totalBits;
            QStandardItemModel* model;
    };

}
