// Copyright 2017 Lybros.

#include "report.h"

#include <QDateTime>
#include <QFile>
#include <QTextStream>

#include "theia/theia.h"

#include "../options.h"
#include "../project.h"
#include "../reconstructor.h"
#include "../storage.h"
#include "../theia_utils.h"

using theia::DescriptorExtractorType;

Report::Report(Project* project) : project_(project) {
  options_ = project_->GetOptions();
  storage_ = project_->GetStorage();
}

Report::~Report() {
}

bool Report::GenerateSmartReconstructionReport(QString filepath) {
  QFile report(filepath);
  if (!report.open(QIODevice::ReadWrite)) { return false; }

  QTextStream r(&report);
  r << "Smart reconstruction report." << endl
    << project_->GetProjectName() << endl
    << QDateTime::currentDateTime().toString() << endl;
  r << endl;

  r << "----- OVERVIEW -----" << endl
    << "project name:     " << project_->GetProjectName() << endl
    << "project location: " << project_->GetProjectPath() << endl
    << "dataset:          " << project_->GetImagesPath() << endl
    << "dataset size:     " << storage_->NumberOfImages() << endl
    << "output location:  " << project_->GetDefaultOutputPath() << endl;
  r << endl;

  r << "----- FEATURE EXTRACTION -----" << endl;
  if (using_prebuilt_matches_) {
    r << "extraction was skipped" << endl;
  } else {
    r << "extraction was performed." << endl
      << "desriptor type: "
      << DescriptorExtractorTypeToString(options_->descriptor_type_);
  }
  r << endl;

  r << "----- FEATURE MATCHING -----" << endl;
  if (using_prebuilt_matches_) {
    r << "feature matching was skipped (read from filesystem)" << endl
      << "timing (reading matches):  " << reading_matches_time_ << "s" << endl;
  } else {
    r << "feature matching was performed." << endl
      << "matching strategy: "
      << MatchingStrategyToString(options_->match_strategy_) << endl
      << "timing (extract&match):  " << extraction_matching_time_ << "s" << endl;
  }
  r << endl;

  r << "----- RECONSTRUCTION -----" << endl;
  r << "shared calibration mode: "
    << (options_->shared_calibration ? "on" : "off") << endl
    << "prior camera information: ";
  if (options_->use_camera_intrinsics_prior) {
    r << "from file" << endl
      << "  reading calibration file: "
      << (using_calibration_file_ ? "succeeded" : "failed / no need") << endl;
  } else {
    r << "from exif" << endl;
  }
  r << "timing:" << endl
    << "  overall:        " << overall_time_ << "s" << endl
    << "  reconstruction: " << reconstruction_time_ << "s" << endl
    << "  colorizing:     " << colorizing_time_ << "s" << endl;
  r << endl;

  r << "Hope it was helpful!" << endl;

  report.close();
  return true;
}
