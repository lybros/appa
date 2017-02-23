#include "ui/mainwindow.h"
#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    QtAwesome* awesome = new QtAwesome(&a);
    awesome->initFontAwesome();
    MainWindow w;
    w.set_icons(awesome);
    w.show();

    return a.exec();
}
