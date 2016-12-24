#include <QApplication>
#include <QGuiApplication>
#include <QFont>

#include "Main.h"
#include "MapEditorWindow.h"
using namespace urchin;

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	QFont defaultFont("Arial", 8, QFont::Normal);
	defaultFont.setFamily(defaultFont.defaultFamily());
	defaultFont.setStyleStrategy(QFont::PreferQuality);
	app.setFont(defaultFont);

	MapEditorWindow *window = new MapEditorWindow(std::string(argv[0]));
	window->setVisible(true);
    window->showMaximized();

	return app.exec();
}
