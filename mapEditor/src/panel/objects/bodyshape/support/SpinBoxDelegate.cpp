#include <stdexcept>
#include <QtWidgets/QDoubleSpinBox>

#include "SpinBoxDelegate.h"
#include "widget/style/SpinBoxStyleHelper.h"

namespace urchin
{

    SpinBoxDelegate::SpinBoxDelegate(QObject *parent, BodyShapeWidget *bodyShapeWidget, QTableView *tableView, QStandardItemModel *model) :
            QItemDelegate(parent),
            bodyShapeWidget(bodyShapeWidget),
            tableView(tableView),
            model(model)
    {

    }

    QWidget *SpinBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const
    {
         auto *editor = new QDoubleSpinBox(parent);
         SpinBoxStyleHelper::applyDefaultStyleOn(editor);
         connect(editor, SIGNAL(valueChanged(double)), this, SLOT(onChange(double)));

         return editor;
    }

    void SpinBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
    {
        double value = index.model()->data(index, Qt::EditRole).toDouble();

        auto *spinBox = dynamic_cast<QDoubleSpinBox*>(editor);
        spinBox->setValue(value);
    }

    void SpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
    {
        auto *spinBox = dynamic_cast<QDoubleSpinBox*>(editor);
        spinBox->interpretText();
        double value = spinBox->value();
        model->setData(index, value, Qt::EditRole);
    }

    void SpinBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const
    {
        editor->setGeometry(option.rect);
    }

    void SpinBoxDelegate::onChange(double newValue)
    {
        const QModelIndex &index = tableView->currentIndex();
        model->setData(index, newValue, Qt::EditRole);

        bodyShapeWidget->updateBodyShape();
    }

}
