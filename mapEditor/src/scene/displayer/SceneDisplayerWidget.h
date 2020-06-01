#ifndef URCHINENGINE_MAPEDITOR_SCENEDISPLAYERWIDGET_H
#define URCHINENGINE_MAPEDITOR_SCENEDISPLAYERWIDGET_H

#include <string>
#include <QKeyEvent>
#include <GL/glew.h>
#include <QGLWidget>

#include "UrchinCommon.h"
#include "SceneDisplayer.h"
#include "scene/displayer/widget/statusbar/StatusBarController.h"

namespace urchin
{

	class SceneDisplayerWidget : public QGLWidget, public Observable
	{
		Q_OBJECT

		public:
			SceneDisplayerWidget(QWidget *, const StatusBarController &, std::string);
			~SceneDisplayerWidget() override;

			enum NotificationType
			{
				BODY_PICKED
			};

			MapHandler *newMap(const std::string &, const std::string &);
			MapHandler *openMap(const std::string &);
			void saveMap(const std::string &) const;
			void closeMap();

			void setViewProperties(SceneDisplayer::ViewProperties, bool);
			void setHighlightSceneObject(const SceneObject *);
			void setHighlightCompoundShapeComponent(std::shared_ptr<const LocalizedCollisionShape>);
			void setHighlightSceneLight(const SceneLight *);
			void setHighlightSceneSound(const SceneSound *);

			void initializeGL() override;
			void paintGL() override;
			void resizeGL(int, int) override;

			void keyPressEvent(QKeyEvent *) override;
			void keyReleaseEvent(QKeyEvent *) override;
			void mousePressEvent(QMouseEvent *) override;
			void mouseReleaseEvent(QMouseEvent *) override;
			void mouseMoveEvent(QMouseEvent *) override;

            bool onMouseClickBodyPickup();
			const std::string &getLastPickedBodyId() const;

		private:
            MapHandler *loadMap(const std::string &, const std::string &);
			void updateSceneDisplayerViewProperties();

            StatusBarController statusBarController;
			std::string mapEditorPath;

			SceneDisplayer *sceneDisplayer;
			bool viewProperties[SceneDisplayer::LAST_VIEW_PROPERTIES];

			int mouseX, mouseY;
			std::string lastPickedBodyId;

        private slots:
	        void onCtrlXPressed();
            void onCtrlYPressed();
            void onCtrlZPressed();
	};

}

#endif
