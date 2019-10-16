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
    QApplication::setFont(defaultFont);

	auto *window = new MapEditorWindow(std::string(argv[0]));
	window->setVisible(true);
    window->showMaximized();

	return QApplication::exec();
}
