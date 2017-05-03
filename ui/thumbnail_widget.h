// Copyright 2017 Lybros.

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
 Q_OBJECT   // NOLINT(whitespace/indent)

 public:
  ThumbnailWidget(MainWindow* main_window,
                  QWidget* parent,
                  const QString& image_path,
                  const QPixmap& image_pixmap);

  QString& GetName();

  bool IsSelected();

  ~ThumbnailWidget();

 protected:
  void mouseReleaseEvent(QMouseEvent* event) override;

 private:
  QString name_;
  QString path_;
  bool selected_;
  MainWindow* main_window_;
};

#endif  // UI_THUMBNAIL_WIDGET_H_
