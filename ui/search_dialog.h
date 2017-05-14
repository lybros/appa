// Copyright 2017 Lybros.

#ifndef UI_SEARCHDIALOG_H
#define UI_SEARCHDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>

#include <glog/logging.h>

namespace Ui { class SearchDialog; }

struct SearchDialogOptions;

class SearchDialog : public QDialog {
 Q_OBJECT   // NOLINT(whitespace/indent)
 public:
  explicit SearchDialog(QString images_dir, QWidget* parent = 0);

  void SetOptions(SearchDialogOptions* options);

  ~SearchDialog();

 private slots:   // NOLINT(whitespace/indent)

  void on_choose_image_button_clicked();

  void on_buttonBox_accepted();

  void on_buttonBox_rejected();

private:
  Ui::SearchDialog* ui;
  SearchDialogOptions* options;
  QString images_dir;

  bool verify_options();

  void show_warning(const QString& error);
};

struct SearchDialogOptions {
  int features_num;
  QString image_path;
};

#endif // UI_SEARCHDIALOG_H
