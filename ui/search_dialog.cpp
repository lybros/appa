// Copyright 2017 Lybros.

#include "search_dialog.h"
#include "ui_search_dialog.h"

SearchDialog::SearchDialog(QString images_dir, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::SearchDialog),
    images_dir(images_dir) {
  ui->setupUi(this);
}

void SearchDialog::SetOptions(SearchDialogOptions* options) {
  SearchDialog::options = options;
}

SearchDialog::~SearchDialog() {
  delete ui;
}

bool SearchDialog::verify_options() {
  QString error = "";

  QString imagePath = ui->image_path->text();
  if (imagePath.isEmpty()) {
    error = "Image Path cannot be empty!";
  }
  if (!QFileInfo::exists(imagePath)) {
    error = "Image doesn't exist";
  }

  if (!error.isEmpty()) {
    show_warning(error);
    return false;
  }
  return true;
}

void SearchDialog::show_warning(const QString& error) {
  QMessageBox warningBox;
  warningBox.setText(error);
  warningBox.exec();
}

void SearchDialog::on_choose_image_button_clicked() {

  QString image_path = QFileDialog::getOpenFileName(
      this,
      tr("Choose image for search"),
      images_dir,
      tr("images (*.jpg *.jpeg *.png)"),
      0,
      QFileDialog::ReadOnly);

  if (image_path.length()) { ui->image_path->setText(image_path); }
}

void SearchDialog::on_buttonBox_accepted() {
  if (!verify_options()) {
    LOG(ERROR) << "Search options validation failed.";
    return;
  }

  options->image_path = ui->image_path->text();
  options->features_num = ui->features_num->text().toInt();

  LOG(INFO) << "Created successfully!";
  QDialog::accept();
}

void SearchDialog::on_buttonBox_rejected() {
  QDialog::reject();
}
