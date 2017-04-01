#include "thumbnail_widget.h"

ThumbnailWidget::ThumbnailWidget(QWidget* parent, QString& image_path) :
    QWidget(parent), name_(image_path) {
}

ThumbnailWidget::~ThumbnailWidget() {
}

void ThumbnailWidget::mouseReleaseEvent(QMouseEvent *event) {
}
