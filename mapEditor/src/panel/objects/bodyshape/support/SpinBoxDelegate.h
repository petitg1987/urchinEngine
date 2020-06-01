#ifndef URCHINENGINE_MAPEDITOR_SPINBOXDELEGATE_H
#define URCHINENGINE_MAPEDITOR_SPINBOXDELEGATE_H

#include <string>
#include <memory>
#include <QItemDelegate>
#include <QtWidgets/QTableView>
#include <QStandardItemModel>

#include "panel/objects/bodyshape/BodyShapeWidget.h"

namespace urchin
{

	class SpinBoxDelegate : public QItemDelegate
	{
		Q_OBJECT

		public:
			SpinBoxDelegate(QObject *, BodyShapeWidget *, QTableView *, QStandardItemModel *);

			QWidget *createEditor(QWidget *, const QStyleOptionViewItem &, const QModelIndex &) const override;

			void setEditorData(QWidget *, const QModelIndex &) const override;
			void setModelData(QWidget *, QAbstractItemModel *, const QModelIndex &) const override;

			void updateEditorGeometry(QWidget *, const QStyleOptionViewItem &, const QModelIndex &) const override;

		private:
			BodyShapeWidget *bodyShapeWidget;
			QTableView *tableView;
			QStandardItemModel *model;

		private slots:
			void onChange(double);
	};

}

#endif
