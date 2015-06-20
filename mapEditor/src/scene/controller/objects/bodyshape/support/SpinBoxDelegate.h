#ifndef ENGINE_MAPEDITOR_SPINBOXDELEGATE_H
#define ENGINE_MAPEDITOR_SPINBOXDELEGATE_H

#include <string>
#include <memory>
#include <QItemDelegate>
#include <QtWidgets/QTableView>
#include <QStandardItemModel>

#include "scene/controller/objects/bodyshape/BodyShapeWidget.h"

namespace urchin
{

	class SpinBoxDelegate : public QItemDelegate
	{
		Q_OBJECT

		public:
			SpinBoxDelegate(QObject *, BodyShapeWidget *, QTableView *, QStandardItemModel *);

			QWidget *createEditor(QWidget *, const QStyleOptionViewItem &, const QModelIndex &) const;

			void setEditorData(QWidget *, const QModelIndex &) const;
			void setModelData(QWidget *, QAbstractItemModel *, const QModelIndex &) const;

			void updateEditorGeometry(QWidget *, const QStyleOptionViewItem &, const QModelIndex &) const;

		private:
			BodyShapeWidget *bodyShapeWidget;
			QTableView *tableView;
			QStandardItemModel *model;

		private slots:
			void onChange(double);
	};

}

#endif
