// Copyright Lybros 2017.

#ifndef UI_THUMBNAIL_WIDGET_H_
#define UI_THUMBNAIL_WIDGET_H_

#include <QWidget>

#include "../src/utils.h"

class ThumbnailWidget : QWidget {
public:
    ThumbnailWidget(QWidget* parent, QString& image_path);

    ~ThumbnailWidget();

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QString name_;
};

#endif  // UI_THUMBNAIL_WIDGET_H_
