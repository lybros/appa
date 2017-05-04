// Copyright 2017 Lybros.

#include "reconstructor.h"

#include <string>
#include <vector>

#include <QMap>
#include <QString>

#include "io/storageio.h"

Reconstructor::Reconstructor(Project* project) : project_(project) {
  storage_ = project_->GetStorage();
  options_ = project_->GetOptions();
  report_ = new Report(project_);
}

Reconstructor::~Reconstructor() {
  delete report_;
}

void Reconstructor::SmartBuild() {
  theia::Timer overall_timer;

  ReconstructionBuilderOptions options =
      options_->GetReconstructionBuilderOptions();

  ReconstructionBuilder reconstruction_builder(options);

  report_->using_prebuilt_matches_ = true;
  report_->using_calibration_file_ = false;

  if (!ReadMatches(&reconstruction_builder)) {
    LOG(INFO) << "Failed to read matches. Starting the process from scratch...";
    report_->using_prebuilt_matches_ = false;
    if (!ExtractFeaturesMatches(&reconstruction_builder)) {
      LOG(ERROR) << "Failed to extract features and matches.";
      return;
    }
  }

  LOG(INFO) << "Starting reconstruction process...";

  theia::Timer reconstructing_timer;
  std::vector<theia::Reconstruction*> reconstructions;
  CHECK(reconstruction_builder.BuildReconstruction(&reconstructions))
  << "Could not create a reconstruction";
  report_->reconstruction_time_ = reconstructing_timer.ElapsedTimeInSeconds();

  LOG(INFO) << "Reconstruction has been created.";

  // Using raw image data to colorize the reconstructions.
  // The method is running using 2 threads.
  theia::Timer colorizing_timer;
  for (int i = 0; i < reconstructions.size(); i++) {
    theia::ColorizeReconstruction(project_->GetImagesPath().toStdString(), 2,
                                  reconstructions[i]);
  }
  report_->colorizing_time_ = colorizing_timer.ElapsedTimeInSeconds();
  LOG(INFO) << "Reconstruction colorized successfully!";

  StorageIO(storage_).WriteReconstructions(reconstructions);

  report_->overall_time_ = overall_timer.ElapsedTimeInSeconds();

  // Generating a report.
  QString report_path = report_->GetDefaultReportPath();
  bool ok = report_->GenerateSmartReconstructionReport(report_path);
  if (ok) {
    LOG(INFO) << "Report was successfully created. Written to "
              << report_path.toStdString();
  } else {
    LOG(INFO) << "Failed to create reconstruction report.";
  }

  return;
}

bool Reconstructor::ReadMatches(ReconstructionBuilder* reconstruction_builder) {
  theia::Timer reading_matches_timer;

  std::vector<std::string> image_files;
  std::vector<theia::CameraIntrinsicsPrior> camera_intrinsics_prior;
  std::vector<theia::ImagePairMatch> image_matches;

  QString matches_file =
      QDir(project_->GetDefaultOutputPath()).filePath("matches.binary");
  if (!QFileInfo(matches_file).exists()) {
    return false;
  }

  // Reading match file.
  theia::ReadMatchesAndGeometry(matches_file.toStdString(),
                                &image_files,
                                &camera_intrinsics_prior,
                                &image_matches);

  // Add all the views. When the intrinsics group id is invalid, the
  // reconstruction builder will assume that the view does not share its
  // intrinsics with any other views.
  theia::CameraIntrinsicsGroupId intrinsics_group_id =
      theia::kInvalidCameraIntrinsicsGroupId;
  if (options_->shared_calibration) {
    intrinsics_group_id = 0;
  }

  for (int i = 0; i < image_files.size(); i++) {
    reconstruction_builder->AddImageWithCameraIntrinsicsPrior(
        image_files[i], camera_intrinsics_prior[i], intrinsics_group_id);
  }

  // Add the matches.
  for (const auto& match : image_matches) {
    CHECK(reconstruction_builder->AddTwoViewMatch(match.image1,
                                                 match.image2,
                                                 match));
  }

  LOG(INFO) << "Matches read from filesystem successfully";
  report_->reading_matches_time_ = reading_matches_timer.ElapsedTimeInSeconds();
  return true;
}

bool Reconstructor::ExtractFeaturesMatches(
    ReconstructionBuilder* reconstruction_builder) {
  theia::Timer extracting_matching_timer;

  // Enabling "Shared Calibration" (all images were made with the same camera).
  theia::CameraIntrinsicsGroupId intrinsics_group_id =
      theia::kInvalidCameraIntrinsicsGroupId;
  if (options_->shared_calibration) {
    intrinsics_group_id = 0;
  }

  // Making decision if Prior Camera Calibration parameters are provided.
  QMap<QString, theia::CameraIntrinsicsPrior> camera_intrinsics_prior;
  if (options_->use_camera_intrinsics_prior &&
      storage_->GetCalibration(&camera_intrinsics_prior)) {
    report_->using_calibration_file_ = true;

    for (QString image_path : storage_->GetImages()) {
      // TODO(uladbohdan): what if do not have prior intrinsics for some of the
      // images?
      LOG(INFO) << "Images will be added with prior calibration."
                << "Shared calibration is "
                << (options_->shared_calibration ? "on" : "off");
      reconstruction_builder->AddImageWithCameraIntrinsicsPrior(
            image_path.toStdString(),
            camera_intrinsics_prior[image_path],
            intrinsics_group_id);
    }
    LOG(INFO) << "Prior camera intrinsics successfully applied.";
  } else {
    LOG(INFO) << "Images will be added without prior calibration."
              << "Shared calibration is "
              << (options_->shared_calibration ? "on" : "off");
    for (QString image_path : storage_->GetImages()) {
      reconstruction_builder->AddImage(image_path.toStdString(),
                                      intrinsics_group_id);
    }
  }

  LOG(INFO) << "All images are added to the builder.";
  LOG(INFO) << "Starting extracting and matching";

  CHECK(reconstruction_builder->ExtractAndMatchFeatures())
  << "Could not extract and match features";

  LOG(INFO) << "Extracted and matched successfully!";

  report_->extraction_matching_time_ =
      extracting_matching_timer.ElapsedTimeInSeconds();

  return true;
}
