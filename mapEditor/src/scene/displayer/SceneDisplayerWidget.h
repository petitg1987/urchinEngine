#ifndef ENGINE_MAPEDITOR_SCENEDISPLAYERWIDGET_H
#define ENGINE_MAPEDITOR_SCENEDISPLAYERWIDGET_H

#include <string>
#include <QGLWidget>
#include <QKeyEvent>

#include "UrchinCommon.h"
#include "SceneDisplayer.h"

namespace urchin
{

	class SceneDisplayerWidget : public QGLWidget
	{
		Q_OBJECT

		public:
			SceneDisplayerWidget(QWidget *, const std::string &);
			virtual ~SceneDisplayerWidget();

			MapHandler *newMap(const std::string &, const std::string &);
			MapHandler *openMap(const std::string &);
			void saveMap(const std::string &) const;
			void closeMap();

			void setViewProperties(SceneDisplayer::ViewProperties, bool);
			void setHighlightSceneObject(const SceneObject *);
			void setHighlightCompoundShapeComponent(const LocalizedCollisionShape *);
			void setHighlightSceneLight(const SceneLight *);
			void setHighlightSceneSound(const SceneSound *);

			void initializeGL();
			void paintGL();
			void resizeGL(int, int);

			void keyPressEvent(QKeyEvent *);
			void keyReleaseEvent(QKeyEvent *);
			void mousePressEvent(QMouseEvent *);
			void mouseReleaseEvent(QMouseEvent *);
			void mouseMoveEvent(QMouseEvent *);

		private:
			void updateSceneDisplayerViewProperties();

			std::string mapEditorPath;

			SceneDisplayer *sceneDisplayer;
			bool viewProperties[SceneDisplayer::LAST_VIEW_PROPERTIES];
	};

}

#endif
