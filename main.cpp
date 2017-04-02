// Copyright 2017 Lybros.

#include <QApplication>
#include "ui/mainwindow.h"

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    QtAwesome* awesome = new QtAwesome(&a);
    awesome->initFontAwesome();

    google::InitGoogleLogging(argv[0]);
    LOG(INFO) << "Logging is enabled.";

    MainWindow w;
    w.set_icons(awesome);
    w.show();

    return a.exec();
}
