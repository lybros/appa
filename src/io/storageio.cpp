// Copyright 2017 Lybros.

#include "storageio.h"

#include <string>

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>

#include "../storage.h"

StorageIO::StorageIO(Storage* storage) : storage_(storage) {
}

StorageIO::~StorageIO() {
}

bool StorageIO::ReadCalibrationFile(
    QString calibration_file_path,
    QMap<QString, theia::CameraIntrinsicsPrior>* camera_intrinsics_prior) {
  Storage* st = storage_;

  if (!QFileInfo(calibration_file_path).exists()) {
    LOG(ERROR) << "Camera calibration file not found.";
    return false;
  }

  QFile calibration_file(calibration_file_path);

  if (!calibration_file.open(QIODevice::ReadOnly)) { return false; }

  QTextStream stream(&calibration_file);
  QString temp_line;

  stream >> temp_line;
  if (temp_line != "CAMERA_CALIBRATION_PINHOLE_1.0") {
    LOG(ERROR) << "Wrong file format or unsupported version.";
    calibration_file.close();
    return false;
  }

  stream >> temp_line;
  if (temp_line != "NUMBER_OF_IMAGES") {
    LOG(ERROR) << "Wrong calibration file format. No NUMBER_OF_IMAGES attr.";
    calibration_file.close();
    return false;
  }
  int number_of_images;
  stream >> number_of_images;

  if (st->options_->shared_calibration) {
    // All images should be initialized with the same Camera intrinsics.
    QString filename;
    stream >> filename;
    if (filename.length() == 0) {
      LOG(ERROR) << "Filename is missing. Aborting reading.";
      return false;
    }

    theia::CameraIntrinsicsPrior temp_camera_intrinsics_prior;
    if (!ReadCalibrationFileRow(&stream, &temp_camera_intrinsics_prior)) {
      return false;
    }
    for (QString image_path : st->GetImages()) {
      (*camera_intrinsics_prior)[FileNameFromPath(image_path)] =
          temp_camera_intrinsics_prior;
    }
    calibration_file.close();
    return true;
  }

  for (int i = 0; i < number_of_images; i++) {
    QString filename;
    stream >> filename;
    if (filename.length() == 0) {
      LOG(ERROR) << "Filename is missing. Aborting reading.";
      return false;
    }

    theia::CameraIntrinsicsPrior temp_camera_intrinsics_prior;
    if (!ReadCalibrationFileRow(&stream, &temp_camera_intrinsics_prior)) {
      return false;
    }

    (*camera_intrinsics_prior)[filename] = temp_camera_intrinsics_prior;
  }

  return true;
}

bool StorageIO::ReadCalibrationFileRow(
    QTextStream* stream,
    theia::CameraIntrinsicsPrior* temp_camera_intrinsics_prior) {
  temp_camera_intrinsics_prior->focal_length.is_set = true;
  (*stream) >> temp_camera_intrinsics_prior->focal_length.value[0];

  temp_camera_intrinsics_prior->principal_point.is_set = true;
  (*stream) >> temp_camera_intrinsics_prior->principal_point.value[0];
  (*stream) >> temp_camera_intrinsics_prior->principal_point.value[1];
  temp_camera_intrinsics_prior->image_width =
      2.0 * temp_camera_intrinsics_prior->principal_point.value[0];
  temp_camera_intrinsics_prior->image_height =
      2.0 * temp_camera_intrinsics_prior->principal_point.value[1];

  temp_camera_intrinsics_prior->aspect_ratio.is_set = true;
  (*stream) >> temp_camera_intrinsics_prior->aspect_ratio.value[0];

  temp_camera_intrinsics_prior->skew.is_set = true;
  (*stream) >> temp_camera_intrinsics_prior->skew.value[0];

  temp_camera_intrinsics_prior->radial_distortion.is_set = true;
  (*stream) >> temp_camera_intrinsics_prior->radial_distortion.value[0];
  (*stream) >> temp_camera_intrinsics_prior->radial_distortion.value[1];

  return true;
}

void StorageIO::WriteReconstructions(
    const std::vector<theia::Reconstruction*>& reconstructions) {
  std::string model_file_template = QDir(storage_->output_location_).filePath(
      QString("models/build_") +
      QDateTime::currentDateTime().toString(Qt::ISODate) +
      QString("_%d.model")).toStdString();

  for (int i = 0; i < reconstructions.size(); i++) {
    std::string output_file = theia::StringPrintf(
        model_file_template.c_str(), i);
    LOG(INFO) << "Writing a model to " << output_file;
    CHECK(theia::WriteReconstruction(*reconstructions[i], output_file))
    << "Failed to write model to filesystem.";
  }

  LOG(INFO) << "Reconstructions have been saved to filesystem.";
}
