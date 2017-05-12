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
  if (options_->shared_calibration_) {
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
  report_->using_camera_intrinsics_prior_ = false;

  // Making decision if Prior Camera Calibration parameters are provided and
  // are usable.
  QMap<QString, CameraIntrinsicsPrior> camera_intrinsics_prior;
  bool shared_calibration;
  bool calibration_available =
      storage_->GetCalibration(&camera_intrinsics_prior, &shared_calibration);
  if (!calibration_available) {
    LOG(INFO) << "Prior calibration is not available. Processing without it.";
  }

  if (calibration_available && options_->use_camera_intrinsics_prior_) {
    // The user requested to use prior data from file and that's possible.
    report_->using_camera_intrinsics_prior_ = true;

    theia::CameraIntrinsicsGroupId intrinsics_group_id =
        theia::kInvalidCameraIntrinsicsGroupId;

    report_->shared_calibration_ = false;

    if (options_->shared_calibration_) {
      if (shared_calibration) {
        LOG(INFO) << "shared_calibration mode was requested and available.";
        intrinsics_group_id = 0;
        report_->shared_calibration_ = true;
      } else {
        LOG(INFO) << "Ignoring user request of shared_calibration due to "
                     "the structure of calibration file.";
      }
    } else {
      if (shared_calibration) {
        LOG(INFO) << "File has shared_calibration structure but user didn't "
                     "request to use the feature. Cameras will be initialized "
                     "with the same values but algorithm will not use that.";
      } else {
        LOG(INFO) << "Running without shared_calibration";
      }
    }

    if (shared_calibration && options_->shared_calibration_) {
      // Shared calibration means adding the prior calibration to only one
      // image would be enough.
      CameraIntrinsicsPrior prior = camera_intrinsics_prior.begin().value();
      for (QString image_path : storage_->GetImages()) {
        CHECK(reconstruction_builder->AddImageWithCameraIntrinsicsPrior(
              image_path.toStdString(),
              prior,
              intrinsics_group_id)) << "Image not added.";
      }
    } else {
      // No shared calibration.
      for (QString image_path : storage_->GetImages()) {
        QString image_name = FileNameFromPath(image_path);
        if (camera_intrinsics_prior.contains(image_name)) {
          // Adding prior intrinsics only for images explicitly specified.
          CHECK(reconstruction_builder->AddImageWithCameraIntrinsicsPrior(
                image_path.toStdString(),
                camera_intrinsics_prior[image_name],
                intrinsics_group_id)) << "Image not added: "
                                      << image_name.toStdString();
        } else {
          CHECK(reconstruction_builder->AddImage(
                image_path.toStdString(),
                intrinsics_group_id)) << "Image not added: "
                                      << image_name.toStdString();
        }
      }
    }
  } else {
    // Either user hasn't requested read of any prior data or reading is not
    // possible.
    // Theia will attempt to use EXIF data as prior information.

    theia::CameraIntrinsicsGroupId intrinsics_group_id =
        theia::kInvalidCameraIntrinsicsGroupId;
    report_->shared_calibration_ = false;
    if (options_->shared_calibration_) {
      intrinsics_group_id = 0;
      report_->shared_calibration_ = true;
    }

    for (QString image_path : storage_->GetImages()) {
      reconstruction_builder->AddImage(image_path.toStdString(),
                                       intrinsics_group_id);
    }
  }

  LOG(INFO) << "All images were added to the builder.";

  theia::Timer extracting_matching_timer;

  CHECK(reconstruction_builder->ExtractAndMatchFeatures())
  << "Could not extract and match features";

  report_->extraction_matching_time_ =
      extracting_matching_timer.ElapsedTimeInSeconds();

  LOG(INFO) << "Extracted and matched successfully!";
  return true;
}
