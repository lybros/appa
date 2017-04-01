// Copyright Lybros 2017.

#ifndef UI_THUMBNAIL_WIDGET_H_
#define UI_THUMBNAIL_WIDGET_H_

#include <iostream>

#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

#include "mainwindow.h"
#include "../src/utils.h"

class ThumbnailWidget : public QWidget {

public:
    ThumbnailWidget(QWidget* parent, QString& image_path);

    ~ThumbnailWidget();

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QString name_;
    bool selected_;
};

#endif  // UI_THUMBNAIL_WIDGET_H_
