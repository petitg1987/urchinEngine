#ifndef URCHINENGINE_MAPEDITOR_OBJECTTABLEVIEW_H
#define URCHINENGINE_MAPEDITOR_OBJECTTABLEVIEW_H

#include <QtWidgets/QTableView>
#include <QStandardItemModel>

#include "UrchinCommon.h"
#include "UrchinMapHandler.h"
#include "scene/controller/SceneController.h"

Q_DECLARE_METATYPE(const urchin::SceneObject *)

namespace urchin
{

	class ObjectTableView : public QTableView, public Observable
	{
		Q_OBJECT

		public:
			explicit ObjectTableView(QWidget *parent = nullptr);

			enum NotificationType
			{
				OBJECT_SELECTION_CHANGED
			};

			bool hasSceneObjectSelected() const;
			const SceneObject *getSelectedSceneObject() const;

			int getSceneObjectRow(const SceneObject *) const;

			int addObject(const SceneObject *);
			bool removeSelectedObject();
			void removeAllObjects();

		private:
			QStandardItemModel *objectsListModel;

			void selectionChanged(const QItemSelection &, const QItemSelection &) override;
	};

}

#endif
