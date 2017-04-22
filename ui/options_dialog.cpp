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

  // Reconstruction options.
  ui->shared_calibration_checkBox->setChecked(options_->shared_calibration);
  ui->use_intrinsics_prior_checkBox->setChecked(
        options_->use_camera_intrinsics_prior);

  SetCheckbox(ui->optimize_focal_length_checkBox,
              OptimizeIntrinsicsType::FOCAL_LENGTH);
  SetCheckbox(ui->optimize_principal_points_checkBox,
              OptimizeIntrinsicsType::PRINCIPAL_POINTS);
  SetCheckbox(ui->optimize_aspect_ratio_checkBox,
              OptimizeIntrinsicsType::ASPECT_RATIO);
  SetCheckbox(ui->optimize_skew_checkBox, OptimizeIntrinsicsType::SKEW);
  SetCheckbox(ui->optimize_radial_distortion_checkBox,
      OptimizeIntrinsicsType::RADIAL_DISTORTION);
  SetCheckbox(ui->optimize_tangential_distortion_checkBox,
              OptimizeIntrinsicsType::TANGENTIAL_DISTORTION);
}

void OptionsDialog::FindSetCombobox(QComboBox* combobox, QString text) {
  combobox->setCurrentIndex(combobox->findText(text));
}

void OptionsDialog::SetCheckbox(QCheckBox* checkbox,
                                OptimizeIntrinsicsType opt_type) {
  checkbox->setChecked(
        static_cast<bool>(options_->intrinsics_to_optimize_ & opt_type));
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

  if (options_enabled_ & RECONSTRUCTION_OPTIONS) {
    options_->shared_calibration = ui->shared_calibration_checkBox->isChecked();
    options_->use_camera_intrinsics_prior =
        ui->use_intrinsics_prior_checkBox->isChecked();

    OptimizeIntrinsicsType optimize_type = OptimizeIntrinsicsType::NONE;
    if (ui->optimize_focal_length_checkBox->isChecked()) {
      optimize_type |= OptimizeIntrinsicsType::FOCAL_LENGTH;
    }
    if (ui->optimize_aspect_ratio_checkBox->isChecked()) {
      optimize_type |= OptimizeIntrinsicsType::ASPECT_RATIO;
    }
    if (ui->optimize_principal_points_checkBox->isChecked()) {
      optimize_type |= OptimizeIntrinsicsType::PRINCIPAL_POINTS;
    }
    if (ui->optimize_skew_checkBox->isChecked()) {
      optimize_type |= OptimizeIntrinsicsType::SKEW;
    }
    if (ui->optimize_radial_distortion_checkBox->isChecked()) {
      optimize_type |= OptimizeIntrinsicsType::RADIAL_DISTORTION;
    }
    if (ui->optimize_tangential_distortion_checkBox->isChecked()) {
      optimize_type |= OptimizeIntrinsicsType::TANGENTIAL_DISTORTION;
    }
    options_->intrinsics_to_optimize_ = optimize_type;
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
