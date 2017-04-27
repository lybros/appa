// Copyright 2017 Lybros.

#include "reconstructor.h"

#include <vector>

#include <QMap>
#include <QString>

#include "project.h"

Reconstructor::Reconstructor(Project* project) : project_(project) {
}

Reconstructor::~Reconstructor() {
}

void Reconstructor::SmartBuild() {
  Project* pr = project_;

  ReconstructionBuilderOptions options =
      pr->options_->GetReconstructionBuilderOptions();

  ReconstructionBuilder reconstruction_builder(options);

  // Enabling "Shared Calibration" (all images were made with the same camera).
  theia::CameraIntrinsicsGroupId intrinsics_group_id =
      theia::kInvalidCameraIntrinsicsGroupId;
  if (pr->options_->shared_calibration) {
    intrinsics_group_id = 0;
  }

  // Making decision if Prior Camera Calibration parameters are provided.
  QMap<QString, theia::CameraIntrinsicsPrior> camera_intrinsics_prior;
  if (pr->options_->use_camera_intrinsics_prior &&
      pr->storage_->GetCalibration(&camera_intrinsics_prior)) {
    for (QString image_path : pr->storage_->GetImages()) {
      // TODO(uladbohdan): what if do not have prior intrinsics for some of the
      // images?
      LOG(INFO) << "Images will be added with prior calibration."
                << "Shared calibration is "
                << (pr->options_->shared_calibration ? "on" : "off");
      reconstruction_builder.AddImageWithCameraIntrinsicsPrior(
            image_path.toStdString(),
            camera_intrinsics_prior[image_path],
            intrinsics_group_id);
    }
    LOG(INFO) << "Prior camera intrinsics successfully applied.";
  } else {
    LOG(INFO) << "Images will be added without prior calibration."
              << "Shared calibration is "
              << (pr->options_->shared_calibration ? "on" : "off");
    for (QString image_path : pr->storage_->GetImages()) {
      reconstruction_builder.AddImage(image_path.toStdString(),
                                      intrinsics_group_id);
    }
  }

  LOG(INFO) << "All images are added to the builder.";
  LOG(INFO) << "Starting extracting and matching";

  CHECK(reconstruction_builder.ExtractAndMatchFeatures())
  << "Could not extract and match features";

  LOG(INFO) << "Extracted and matched successfully!";

  std::vector<theia::Reconstruction*> reconstructions;
  CHECK(reconstruction_builder.BuildReconstruction(&reconstructions))
  << "Could not create a reconstruction";

  LOG(INFO) << "Reconstruction has been created.";

  // Using raw image data to colorize the reconstructions.
  // The method is running using 2 threads.
  for (int i = 0; i < reconstructions.size(); i++) {
    theia::ColorizeReconstruction(pr->GetImagesPath().toStdString(), 2,
                                  reconstructions[i]);
  }
  LOG(INFO) << "Reconstruction colorized successfully!";

  pr->storage_->SetReconstructions(reconstructions);
  pr->storage_->WriteReconstructions();
  return;
}
