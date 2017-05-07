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
    QMap<QString, theia::CameraIntrinsicsPrior>* camera_intrinsics_prior,
    bool* shared_calibration,
    bool* geo_data_included) {
  if (!QFileInfo(calibration_file_path).exists()) {
    LOG(ERROR) << "Camera calibration file not found.";
    return false;
  }

  QFile calibration_file(calibration_file_path);

  if (!calibration_file.open(QIODevice::ReadOnly)) { return false; }

  QTextStream stream(&calibration_file);

  QString temp_line;
  stream >> temp_line;
  if (temp_line == "CAMERA_CALIBRATION_PINHOLE_1.1") {
    bool ok = ReadCalibrationFile_Pinhole(stream,
                camera_intrinsics_prior, shared_calibration, geo_data_included);
    calibration_file.close();
    return ok;
  } else {
    LOG(ERROR) << "Wrong file format or unsupported version.";
    calibration_file.close();
    return false;
  }
}

bool StorageIO::ReadCalibrationFile_Pinhole(
    QTextStream& stream,
    QMap<QString, theia::CameraIntrinsicsPrior> *camera_intrinsics_prior,
    bool *shared_calibration,
    bool *geo_data_included) {
  bool ok = ReadSharedCalibration(stream, shared_calibration);
  if (!ok) {
    LOG(ERROR) << "Failed to check if shared calibration is enabled";
    return false;
  }

  ok = GeoDataIncluded(stream, geo_data_included);
  if (!ok) {
    LOG(ERROR) << "Failed to check if geo data is included";
    return false;
  }

  int num;
  ok = ReadNumberOfImages(stream, &num);
  if (!ok) {
    LOG(ERROR) << "Failed to read number of images";
    return false;
  }

  if (shared_calibration) {
    CameraIntrinsicsPrior shared_camera_intrinsics_prior;
    ok = ReadCameraIntrinsics_Pinhole(stream, &shared_camera_intrinsics_prior);
    if (!ok) {
      LOG(ERROR) << "Failed to read shared camera intrinsics";
      return false;
    }
    for (int i = 0; i < num; i++) {
      QString image_name;
      stream >> image_name;
      CameraIntrinsicsPrior temp_camera_intrinsics_prior(
            shared_camera_intrinsics_prior);
      ok = ReadGeoData(stream, &temp_camera_intrinsics_prior);
      if (!ok) {
        LOG(ERROR) << "Failed to read geo data on image "
                   << image_name.toStdString();
        return false;
      }
      camera_intrinsics_prior->insert(image_name, temp_camera_intrinsics_prior);
    }
  } else {
    // Shared calibration is OFF.
    for (int i = 0; i < num; i++) {
      QString image_name;
      stream >> image_name;
      CameraIntrinsicsPrior temp_camera_intrinsics_prior;
      ok = ReadCameraIntrinsics_Pinhole(stream, &temp_camera_intrinsics_prior);
      if (!ok) {
        LOG(ERROR) << "Failed to read camera intrinsics for "
                   << image_name.toStdString();
        return false;
      }
      ok = ReadGeoData(stream, &temp_camera_intrinsics_prior);
      if (!ok) {
        LOG(ERROR) << "Failed to read geo data for "
                   << image_name.toStdString();
        return false;
      }
    }
  }

  return true;
}

bool StorageIO::ReadSharedCalibration(QTextStream& stream, bool* yes) {
  QString temp_line;
  stream >> temp_line;
  if (temp_line != "SHARED_CALIBRATION") {
    return false;
  }
  stream >> temp_line;
  if (temp_line == "YES") {
    *yes = true;
    return true;
  } else if (temp_line == "NO") {
    *yes = false;
    return true;
  }
  return false;
}

bool StorageIO::GeoDataIncluded(QTextStream& stream, bool* yes) {
  QString temp_line;
  stream >> temp_line;
  if (temp_line != "GEO_DATA_INCLUDED") {
    return false;
  }
  stream >> temp_line;
  if (temp_line == "YES") {
    *yes = true;
    return true;
  } else {
    *yes = false;
    return true;
  }
  return false;
}

bool StorageIO::ReadNumberOfImages(QTextStream& stream, int* num) {
  QString temp_line;
  stream >> temp_line;
  if (temp_line != "NUMBER_OF_IMAGES") {
    return false;
  }
  stream >> *num;
  return true;
}

bool StorageIO::ReadCameraIntrinsics_Pinhole(QTextStream& stream,
    CameraIntrinsicsPrior* temp_camera_intrinsics_prior) {
  temp_camera_intrinsics_prior->focal_length.is_set = true;
  stream >> temp_camera_intrinsics_prior->focal_length.value[0];

  temp_camera_intrinsics_prior->principal_point.is_set = true;
  stream >> temp_camera_intrinsics_prior->principal_point.value[0];
  stream >> temp_camera_intrinsics_prior->principal_point.value[1];
  temp_camera_intrinsics_prior->image_width =
      2.0 * temp_camera_intrinsics_prior->principal_point.value[0];
  temp_camera_intrinsics_prior->image_height =
      2.0 * temp_camera_intrinsics_prior->principal_point.value[1];

  temp_camera_intrinsics_prior->aspect_ratio.is_set = true;
  stream >> temp_camera_intrinsics_prior->aspect_ratio.value[0];

  temp_camera_intrinsics_prior->skew.is_set = true;
  stream >> temp_camera_intrinsics_prior->skew.value[0];

  temp_camera_intrinsics_prior->radial_distortion.is_set = true;
  stream >> temp_camera_intrinsics_prior->radial_distortion.value[0];
  stream >> temp_camera_intrinsics_prior->radial_distortion.value[1];

  return true;
}

bool StorageIO::ReadGeoData(
    QTextStream& stream,
    CameraIntrinsicsPrior* temp_camera_intrinsics_prior) {
  // Latitude.
  temp_camera_intrinsics_prior->latitude.is_set = true;
  stream >> temp_camera_intrinsics_prior->latitude.value[0];
  // Longitude.
  temp_camera_intrinsics_prior->longitude.is_set = true;
  stream >> temp_camera_intrinsics_prior->longitude.value[0];
  // Altitude.
  temp_camera_intrinsics_prior->altitude.is_set = true;
  stream >> temp_camera_intrinsics_prior->altitude.value[0];

  LOG(INFO) << "GPS read: "
            << temp_camera_intrinsics_prior->latitude.value[0]
            << temp_camera_intrinsics_prior->longitude.value[0]
            << temp_camera_intrinsics_prior->altitude.value[0];

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
