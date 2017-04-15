// Copyright 2017 Lybros.

#include "options_dialog.h"
#include "ui_options_dialog.h"

OptionsDialog::OptionsDialog(QWidget* parent,
                             Options* options, int options_enabled) :
  QDialog(parent), ui(new Ui::OptionsDialog),
  options_(options), options_enabled_(options_enabled) {
  ui->setupUi(this);
  InitializeForms();
  EnableOptionSections();
}

OptionsDialog::~OptionsDialog() {
  delete ui;
}

// Initializing Options Dialog with options from the project.
void OptionsDialog::InitializeForms() {
  FindSetCombobox(ui->num_threads_combobox,
                  QString::number(options_->num_threads_));
  FindSetCombobox(ui->descriptor_type_combobox,
                  QMap<theia::DescriptorExtractorType, QString>({
                    {theia::DescriptorExtractorType::SIFT, "SIFT"},
                    {theia::DescriptorExtractorType::AKAZE, "AKAZE"},
                  })[options_->descriptor_type_]);
}

void OptionsDialog::FindSetCombobox(QComboBox* combobox, QString text) {
  combobox->setCurrentIndex(combobox->findText(text));
}

void OptionsDialog::accept() {
  if (options_enabled_ & GENERAL_OPTIONS) {
    options_->num_threads_ = ui->num_threads_combobox->currentText().toInt();
  }

  if (options_enabled_ & EXTRACTING_FEATURES_OPTIONS) {
    options_->descriptor_type_ = QMap<QString, theia::DescriptorExtractorType>({
      {"SIFT", theia::DescriptorExtractorType::SIFT},
      {"AKAZE", theia::DescriptorExtractorType::AKAZE},
    })[ui->descriptor_type_combobox->currentText()];
  }

  LOG(INFO) << "All options applied successfully.";
  QDialog::accept();
}

void OptionsDialog::EnableOptionSections() {
  if (options_enabled_ & GENERAL_OPTIONS) {
    ui->general_groupbox->setEnabled(true);
  }
  if (options_enabled_ & EXTRACTING_FEATURES_OPTIONS) {
    ui->extracting_features_groupbox->setEnabled(true);
  }
  if (options_enabled_ & MATCHING_FEATURES_OPTIONS) {
    ui->matching_features_groupbox->setEnabled(true);
  }
  if (options_enabled_ & RECONSTRUCTION_OPTIONS) {
    ui->reconstruction_groupbox->setEnabled(true);
  }
}
