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
  // General options.
  FindSetCombobox(ui->num_threads_combobox,
                  QString::number(options_->num_threads_));

  // Feature extracting options.
  FindSetCombobox(ui->descriptor_type_combobox,
                  QMap<theia::DescriptorExtractorType, QString>({
                    {theia::DescriptorExtractorType::SIFT, "SIFT"},
                    {theia::DescriptorExtractorType::AKAZE, "AKAZE"},
                  })[options_->descriptor_type_]);

  // Feature matching options.
  FindSetCombobox(ui->matching_strategy_combobox,
                QMap<theia::MatchingStrategy, QString>({
                  {theia::MatchingStrategy::BRUTE_FORCE, "Brute Force"},
                  {theia::MatchingStrategy::CASCADE_HASHING, "Cascade Hashing"},
                })[options_->match_strategy_]);
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

  if (options_enabled_ & MATCHING_FEATURES_OPTIONS) {
    options_->match_strategy_ = QMap<QString, theia::MatchingStrategy>({
      {"Brute Force", theia::MatchingStrategy::BRUTE_FORCE},
      {"Cascade Hashing", theia::MatchingStrategy::CASCADE_HASHING},
    })[ui->matching_strategy_combobox->currentText()];
  }

  // if (options_enabled_ & RECONSTRUCTION_OPTIONS) {
  // }

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
