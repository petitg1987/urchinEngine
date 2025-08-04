#include <QStylePainter>
#include <ranges>

#include "widget/BitsetComboBox.h"

namespace urchin {

    BitsetComboBox::BitsetComboBox(QWidget* parent, unsigned int totalBits, unsigned long defaultBitValues) :
            QComboBox(parent),
            totalBits(totalBits) {
        model = new QStandardItemModel(this);
        this->setModel(model);
        connect(model, &QStandardItemModel::itemChanged, this, [this]() {
            update(); //trigger paint event
            emit onBitChanged();
        });

        std::bitset<32> defaultBits(defaultBitValues);
        for (unsigned int i = 0; i < totalBits; ++i) {
            std::string text = std::to_string(i + 1);
            addCheckItem(QString::fromStdString(text), defaultBits.test(i));
        }
    }

    void BitsetComboBox::addCheckItem(const QString& text, bool checked) const {
        auto item = new QStandardItem(text);
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
        item->setData(checked ? Qt::Checked : Qt::Unchecked, Qt::CheckStateRole);
        model->appendRow(item);
    }

    unsigned long BitsetComboBox::getBitValues() const {
        std::bitset<32> bits;
        for (unsigned int i = 0; i < totalBits; ++i) {
            if (model->item(i)->checkState() == Qt::Checked) {
                bits.set(i);
            }
        }
        return bits.to_ulong();
    }

    void BitsetComboBox::paintEvent(QPaintEvent*) {
        QStylePainter painter(this);
        QStyleOptionComboBox opt;
        initStyleOption(&opt);

        std::string allBitsStrings = std::bitset<32>(getBitValues()).to_string();
        std::string bitsString = allBitsStrings.substr(allBitsStrings.size() - totalBits, allBitsStrings.size() - 1);
        std::ranges::reverse(bitsString);

        std::string spaceBitsString;
        for (std::size_t i = 0; i < bitsString.length(); ++i) {
            spaceBitsString += bitsString[i];
            if (i != bitsString.length() - 1) {
                spaceBitsString += ' ';
            }
        }
        opt.currentText = QString::fromStdString(spaceBitsString);

        painter.drawComplexControl(QStyle::CC_ComboBox, opt);
        painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
    }


}