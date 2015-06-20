#ifndef ENGINE_MAPEDITOR_LIGHTTABLEVIEW_H
#define ENGINE_MAPEDITOR_LIGHTTABLEVIEW_H

#include <QtWidgets/QTableView>
#include <QStandardItemModel>

#include "UrchinCommon.h"
#include "UrchinMapHandler.h"
#include "scene/controller/SceneController.h"

Q_DECLARE_METATYPE(const urchin::SceneLight *)

namespace urchin
{

	class LightTableView : public QTableView, public Observable
	{
		Q_OBJECT

		public:
			LightTableView(QWidget *parent = 0);

			enum NotificationType
			{
				SELECTION_CHANGED
			};

			bool hasSceneLightSelected() const;
			const SceneLight *getSelectedSceneLight() const;

			void addLight(const SceneLight *);
			bool removeSelectedLight();
			void removeAllLights();

		private:
			QStandardItemModel *lightsListModel;

			void selectionChanged(const QItemSelection &, const QItemSelection &);
	};

}

#endif
