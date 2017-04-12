// Copyright 2017 Lybros.

#include "thumbnail_widget.h"

#include "mainwindow.h"

// TODO(uladbohdan): to handle the situation when we have small amount of
// images (2-3) as they're expanding and does not look good.

ThumbnailWidget::ThumbnailWidget(MainWindow* main_window,
                                 QWidget* parent,
                                 const QString& image_path,
                                 const QPixmap& image_pixmap) :
    QWidget(parent),
    main_window_(main_window),
    path_(image_path),
    name_(FileNameFromPath(image_path)),
    selected_(false) {
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

  QVBoxLayout* box_layout = new QVBoxLayout(this);
  box_layout->setMargin(0);
  setLayout(box_layout);

  QLabel* image_label = new QLabel(this);
  image_label->setPixmap(image_pixmap);
  box_layout->addWidget(image_label);
  box_layout->setAlignment(image_label, Qt::AlignHCenter);

  QLabel* image_title_label = new QLabel(name_, this);
  box_layout->addWidget(image_title_label);
  box_layout->setAlignment(image_title_label, Qt::AlignHCenter);
}

ThumbnailWidget::~ThumbnailWidget() {
}

bool ThumbnailWidget::IsSelected() {
  return selected_;
}

QString& ThumbnailWidget::GetName() {
  return name_;
}

void ThumbnailWidget::mouseReleaseEvent(QMouseEvent* event) {
  selected_ = !selected_;

  if (selected_) {
    setStyleSheet("background-color: green;");
  } else {
    setStyleSheet("");
  }

  main_window_->UpdateSelectedThumbnails();
}
