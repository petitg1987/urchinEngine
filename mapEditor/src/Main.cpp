#include <QApplication>
#include <QFont>

#include <MapEditorWindow.h>
using namespace urchin;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(53, 53, 53));
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, QColor(25, 25, 25));
    palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    palette.setColor(QPalette::ToolTipBase, Qt::white);
    palette.setColor(QPalette::ToolTipText, Qt::white);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, QColor(53, 53, 53));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Link, QColor(42, 130, 218));
    palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    palette.setColor(QPalette::HighlightedText, Qt::black);
    QApplication::setPalette(palette);

    QFont defaultFont("Arial", 8, QFont::Normal);
    defaultFont.setFamily(defaultFont.defaultFamily());
    defaultFont.setStyleStrategy(QFont::PreferQuality);
    QApplication::setFont(defaultFont);

    QApplication::setStyle("Fusion");

    auto* window = new MapEditorWindow(std::string(argv[0]));
    window->setVisible(true);
    window->showMaximized();

    return QApplication::exec();
}
