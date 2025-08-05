#pragma once

#include <QStandardItemModel>
#include <QtWidgets/QComboBox>

namespace urchin {

    class BitsetComboBox final : public QComboBox {
        Q_OBJECT

        public:
            BitsetComboBox(QWidget*, unsigned int, unsigned long);
            ~BitsetComboBox() override = default;

            void updateLabels(std::span<const std::string>) const;

            void setBitValues(unsigned long) const;
            unsigned long getBitValues() const;

        protected:
            void paintEvent(QPaintEvent*) override;

        signals:
            void onBitChanged();

        private:
            unsigned int totalBits;
            QStandardItemModel* model;
    };

}
