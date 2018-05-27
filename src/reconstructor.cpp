// Copyright 2017 Lybros.

#include "reconstructor.h"

#include <string>
#include <vector>

#include <QMap>
#include <QString>

#include "io/storageio.h"

using theia::ReconstructionEstimator;
using theia::ReconstructionEstimatorOptions;
using theia::ReconstructionEstimatorSummary;

using theia::TwoViewInfoFromTwoCameras;

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

//  if (!storage_->GetSlamImages().empty()) {
//    LOG(INFO) << "loading SLAM subset into the Reconstructor...";
//    if (!ExtractFeaturesMatches(&reconstruction_builder)) {
//      LOG(ERROR) << "Failed to extract features and matches.";
//      return;
//    }
//  } else {
  if (!ReadMatches(&reconstruction_builder)) {
    LOG(INFO) << "Failed to read matches. Starting the process from scratch...";
    report_->using_prebuilt_matches_ = false;
    if (!ExtractFeaturesMatches(&reconstruction_builder)) {
      LOG(ERROR) << "Failed to extract features and matches.";
      return;
    }
  }
//  }

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
      for (QString image_path : storage_->GetProperImages()) {
        CHECK(reconstruction_builder->AddImageWithCameraIntrinsicsPrior(
              image_path.toStdString(),
              prior,
              intrinsics_group_id)) << "Image not added.";
      }
    } else {
      // No shared calibration.
      for (QString image_path : storage_->GetProperImages()) {
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

    for (QString image_path : storage_->GetProperImages()) {
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

void Reconstructor::FastBuild() {
  LOG(INFO) << "About to perform Fast Reconstruction...";
//  theia::Timer overall_timer;

//  ReconstructionBuilderOptions options =
//      options_->GetReconstructionBuilderOptions();

//  ReconstructionBuilder reconstruction_builder(options);

  LOG(INFO) << "Number of images for reconstruction: "
            << storage_->GetProperImages().size();

//  theia::CameraIntrinsicsGroupId intrinsics_group_id =
//      theia::kInvalidCameraIntrinsicsGroupId;

  CameraIntrinsicsPrior prior;
  prior.image_height = 480;
  prior.image_width = 640;
  prior.focal_length.is_set = true;
  prior.focal_length.value[0] = 517.;
  prior.principal_point.is_set = true;
  prior.principal_point.value[0] = 318.6;
  prior.principal_point.value[1] = 255.3;

//  LOG(INFO) << "About to push images?..";

//  for (QString image_path : storage_->GetProperImages()) {
//    auto im = FileNameFromPath(image_path).toStdString();
//    LOG(INFO) << "Adding " << im;
//    CHECK(reconstruction_builder.AddImageWithCameraIntrinsicsPrior(
//            im, prior, 0))
//        << "Failed to add image! :(";
//  }

  auto slam_data = project_->GetSlamData();

  ReconstructionEstimatorOptions estimator_options;
//  estimator_options.min_num_two_view_inliers = 10;

  theia::ReconstructionEstimator* estimator =
      ReconstructionEstimator::Create(estimator_options);

  auto cameras_map = project_->GetCamerasMap();

  theia::ViewGraph* view_graph = new theia::ViewGraph();
  theia::Reconstruction* reconstruction = new theia::Reconstruction();

  LOG(INFO) << "Adding Views to the Reconstruction...";
  for (QString name : storage_->GetSlamImages()) {
    std::string short_name = FileNameFromPath(name).toStdString();
    LOG(INFO) << "Adding a view: " << short_name;
    reconstruction->AddView(short_name);
  }
  LOG(INFO) << "View IDs are now usable.";

  for (auto it = slam_data.begin(); it != slam_data.end(); it++) {
    QString im1 = FileNameFromPath(QString::fromStdString(it->second.image1));
//    LOG(INFO) << " - " << it->second.image1 << " -> " << im1.toStdString();
    QString im2 = FileNameFromPath(QString::fromStdString(it->second.image2));
//    LOG(INFO) << " - " << it->second.image2 << " -> " << im2.toStdString();
    if (im1.length() > 1 && im2.length() > 1 &&
        it->second.correspondences.size() > estimator_options.min_num_two_view_inliers) {
      LOG(INFO) << "Correspondences: " << it->second.correspondences.size();
//      reconstruction_builder.AddTwoViewMatch(
//          im1.toStdString(), im2.toStdString(), it->second);
//     }
      theia::TwoViewInfo* two_view_info = new theia::TwoViewInfo();
      theia::Camera cam1 = cameras_map[im1.toStdString()];
      theia::Camera cam2 = cameras_map[im2.toStdString()];
//      LOG(INFO) << "Camera 1 POS " << cam1.GetPosition();
//      LOG(INFO) << "Camera 1 ORI " << cam2.GetOrientationAsRotationMatrix();
      TwoViewInfoFromTwoCameras(cam1, cam2, two_view_info);
      two_view_info->num_verified_matches = it->second.correspondences.size();

//      LOG(INFO) << "Was two_view_info populated? " << two_view_info->num_verified_matches << "\n"
//                << two_view_info->num_homography_inliers << "\n"
//                << two_view_info->rotation_2 << "\n" << two_view_info->position_2;

      view_graph->AddEdge(reconstruction->ViewIdFromName(im1.toStdString()),
                          reconstruction->ViewIdFromName(im2.toStdString()),
                          *two_view_info);
    }
  }

  // One more approaching: pushing SLAM observations into reconstruction.
  auto observations = project_->GetSlamObservations();
  int c1 = 0, c2 = 0;
  for(auto obs : observations) {
    c1++;
    theia::TrackId tId = reconstruction->AddTrack();
    for (Observation oneObs : obs.second) {
      theia::ViewId vId = reconstruction->ViewIdFromName(oneObs.filename);
      reconstruction->AddObservation(vId, tId, theia::Feature(oneObs.x, oneObs.y));
      c2++;
    }
  }
  LOG(INFO) << "OBSERVATIONS STATS: tracks: " << c1 << "  observations: " << c2;

  LOG(INFO) << "Launching Estimator!";

  LOG(INFO) << "Views in reconstruction: " << reconstruction->NumViews();

  LOG(INFO) << "Num of edges:" << view_graph->GetAllEdges().size();

  ReconstructionEstimatorSummary summary = estimator->Estimate(view_graph, reconstruction);
  LOG(INFO) << "Got summary. TRACKS (= 3D points):" << summary.estimated_tracks.size();
  LOG(INFO) << "VIEWS:" << summary.estimated_views.size();
  LOG(INFO) << "SUCCESS " << summary.success;
  LOG(INFO) << "Total time:" << summary.total_time;
  LOG(INFO) << "MESSAGE:" << summary.message;

//  report_->using_prebuilt_matches_ = true;

 /* if (!ReadMatches(&reconstruction_builder)) {
    LOG(INFO) << "Failed to read matches. Starting the process from scratch...";
    report_->using_prebuilt_matches_ = false;
    if (!ExtractFeaturesMatches(&reconstruction_builder)) {
      LOG(ERROR) << "Failed to extract features and matches.";
      return;
    }
  }

  LOG(INFO) << "Starting reconstruction process...";*/

//  theia::Timer reconstructing_timer;

  // !!!
//  std::vector<theia::Reconstruction*> reconstructions;
//  CHECK(reconstruction_builder.BuildReconstruction(&reconstructions))
//  << "Could not create a reconstruction";


//  report_->reconstruction_time_ = reconstructing_timer.ElapsedTimeInSeconds();

//  LOG(INFO) << "Reconstruction has been created.";

  // Using raw image data to colorize the reconstructions.
  // The method is running using 2 threads.
//  theia::Timer colorizing_timer;
//  for (int i = 0; i < reconstructions.size(); i++) {
//    theia::ColorizeReconstruction(project_->GetImagesPath().toStdString(), 2,
//                                  reconstructions[i]);
//  }
//  report_->colorizing_time_ = colorizing_timer.ElapsedTimeInSeconds();
//  LOG(INFO) << "Reconstruction colorized successfully!";

  std::vector<theia::Reconstruction*> reconstructions;
  reconstructions.push_back(reconstruction);

  StorageIO(storage_).WriteReconstructions(reconstructions);

//  report_->overall_time_ = overall_timer.ElapsedTimeInSeconds();

//  // Generating a report.
//  QString report_path = report_->GetDefaultReportPath();
//  bool ok = report_->GenerateSmartReconstructionReport(report_path);
//  if (ok) {
//    LOG(INFO) << "Report was successfully created. Written to "
//              << report_path.toStdString();
//  } else {
//    LOG(INFO) << "Failed to create reconstruction report.";
//  }

  return;
}

void Reconstructor::SLAM_New_Build() {
  ReconstructionBuilderOptions options =
      options_->GetReconstructionBuilderOptions();

  ReconstructionBuilder reconstruction_builder(options);

  CameraIntrinsicsPrior prior;
  prior.image_height = 480;
  prior.image_width = 640;
  prior.focal_length.is_set = true;
  prior.focal_length.value[0] = 517.;
  prior.principal_point.is_set = true;
  prior.principal_point.value[0] = 318.6;
  prior.principal_point.value[1] = 255.3;

  for (QString name : storage_->GetSlamImages()) {
//    std::string short_name = FileNameFromPath(name).toStdString();
    reconstruction_builder.AddImageWithCameraIntrinsicsPrior(name.toStdString(), prior, 1);
    LOG(INFO) << "!! " << name.toStdString();
  }
  LOG(INFO) << "A total of " << storage_->GetSlamImages().size() << " added.";

  std::unordered_map<int, theia::ImagePairMatch>& matches = project_->GetSlamData();
  LOG(INFO) << "XXX  " << matches.size();
  for (auto match : matches) {
    if (match.second.image1.length() < 2 || match.second.image2.length() < 2) {
      continue;
    }
    LOG(INFO) << "## " << match.second.image1 << " " << match.second.image2;
    std::string im1, im2;
    CHECK(theia::GetFilenameFromFilepath(match.second.image1, true, &im1));
    CHECK(theia::GetFilenameFromFilepath(match.second.image2, true, &im2));
//    std::string im2 = FileNameFromPath(match.second.image2);
    reconstruction_builder.AddTwoViewMatch(im1, im2,
                                            match.second);
  }

  std::vector<theia::Reconstruction*> reconstructions;
  CHECK(reconstruction_builder.BuildReconstruction(&reconstructions))
  << "Could not create a reconstruction";

  StorageIO(storage_).WriteReconstructions(reconstructions);
}
