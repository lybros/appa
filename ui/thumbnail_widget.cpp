#include "thumbnail_widget.h"

// TODO(uladbohdan): to handle the situation when we have small amount of
// images (2-3) as they're expanding and does not look good.

ThumbnailWidget::ThumbnailWidget(QWidget* parent, QString& image_path) :
    QWidget(parent), name_(image_path), selected_(false) {

    // TODO(uladbohdan): to make sure this magic number is good for every
    // screen or to replace it with something else.
    int PREVIEW_AREA_WIDTH = parent->size().width() - 25;
    int INF = 99999999;

    QVBoxLayout* box_layout = new QVBoxLayout(this);
    box_layout->setMargin(0);
    setLayout(box_layout);

    QLabel* image_label = new QLabel(this);
    image_label->setPixmap(QPixmap::fromImage(
                    QImage(image_path).scaled(PREVIEW_AREA_WIDTH, INF,
                                              Qt::KeepAspectRatio)));
    box_layout->addWidget(image_label);
    box_layout->setAlignment(image_label, Qt::AlignHCenter);

    QLabel* image_title_label =
            new QLabel(FileNameFromPath(image_path), this);
    box_layout->addWidget(image_title_label);
    box_layout->setAlignment(image_title_label, Qt::AlignHCenter);
}

ThumbnailWidget::~ThumbnailWidget() {
}

void ThumbnailWidget::mouseReleaseEvent(QMouseEvent* event) {
    selected_ = !selected_;

    if (selected_) {
        setStyleSheet("background-color: green;");
    } else {
        setStyleSheet("");
    }
}
