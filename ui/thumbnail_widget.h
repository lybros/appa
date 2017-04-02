// Copyright Lybros 2017.

#ifndef UI_THUMBNAIL_WIDGET_H_
#define UI_THUMBNAIL_WIDGET_H_

#include <iostream>

#include <QLabel>
#include <QMainWindow>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

#include "../src/utils.h"

class MainWindow;

class ThumbnailWidget : public QWidget {

public:
    ThumbnailWidget(MainWindow* main_window, QWidget* parent,
                    QString& image_path);

    QString& GetName();

    bool IsSelected();

    ~ThumbnailWidget();

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QString name_;
    QString path_;
    bool selected_;
    MainWindow* main_window_;
};

#endif  // UI_THUMBNAIL_WIDGET_H_
