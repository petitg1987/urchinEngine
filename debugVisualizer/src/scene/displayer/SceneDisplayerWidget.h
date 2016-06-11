#ifndef ENGINE_DEBUGVISUALIZER_SCENEDISPLAYERWIDGET_H
#define ENGINE_DEBUGVISUALIZER_SCENEDISPLAYERWIDGET_H

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

			void openDebugFile(std::shared_ptr<GeometryEntityHandler>);

			void initializeGL();
			void paintGL();
			void resizeGL(int, int);

			void keyPressEvent(QKeyEvent *);
			void keyReleaseEvent(QKeyEvent *);
			void mousePressEvent(QMouseEvent *);
			void mouseReleaseEvent(QMouseEvent *);
			void mouseMoveEvent(QMouseEvent *);

		private:
			std::string debugVisualizerPath;

			SceneDisplayer *sceneDisplayer;
	};

}

#endif
