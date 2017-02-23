#include "ui/mainwindow.h"
#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    QtAwesome* awesome = new QtAwesome(&a);
    MainWindow w(nullptr, awesome);
    awesome->initFontAwesome();
    w.show();

    return a.exec();
}
