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
    bool* shared_calibration) {
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
                camera_intrinsics_prior, shared_calibration);
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
    QMap<QString, CameraIntrinsicsPrior>* camera_intrinsics_prior,
    bool* shared_calibration) {
  // Reading the header of the file.
  if (!ReadYesNoExpression(stream, "SHARED_CALIBRATION", shared_calibration)) {
    LOG(ERROR) << "Failed to check if shared calibration is enabled";
    return false;
  }

  bool geo_data_included, position_included, orientation_included;
  if (!ReadYesNoExpression(stream, "GEO_DATA", &geo_data_included)) {
    LOG(ERROR) << "Failed to check if geo data is included.";
    return false;
  }
  if (!ReadYesNoExpression(stream, "POSITION", &position_included)) {
    LOG(ERROR) << "Failed to check if Position is included.";
    return false;
  }
  if (!ReadYesNoExpression(stream, "ORIENTATION", &orientation_included)) {
    LOG(ERROR) << "Failed to check is Orientation is included.";
    return false;
  }

  int num;
  if (!ReadNumberOfImages(stream, &num)) {
    LOG(ERROR) << "Failed to read number of images";
    return false;
  }

  if (shared_calibration) {
    // The file has shared calibration structure.
    CameraIntrinsicsPrior shared_camera_intrinsics_prior;
    if (!ReadCameraIntrinsics_Pinhole(stream,
                                      &shared_camera_intrinsics_prior)) {
      LOG(ERROR) << "Failed to read shared camera intrinsics.";
      return false;
    }

    for (int i = 0; i < num; i++) {
      QString image_name;
      stream >> image_name;

      if (geo_data_included && !ReadGeoData(stream, nullptr)) {
        LOG(ERROR) << "Failed to read geo data for image "
                   << image_name.toStdString();
        return false;
      }

      if (position_included && !ReadPosition(stream, nullptr)) {
        LOG(ERROR) << "Failed to read position for image "
                   << image_name.toStdString();
        return false;
      }

      if (orientation_included && !ReadOrientation(stream, nullptr)) {
        LOG(ERROR) << "Fadile to read orientation for image "
                   << image_name.toStdString();
        return false;
      }

      camera_intrinsics_prior->insert(image_name,
                                      shared_camera_intrinsics_prior);
    }
  } else {
    // Shared calibration is OFF.
    for (int i = 0; i < num; i++) {
      QString image_name;
      stream >> image_name;
      CameraIntrinsicsPrior temp_camera_intrinsics_prior;

      if (!ReadCameraIntrinsics_Pinhole(stream,
                                        &temp_camera_intrinsics_prior)) {
        LOG(ERROR) << "Failed to read camera intrinsics for image "
                   << image_name.toStdString();
        return false;
      }

      if (geo_data_included && !ReadGeoData(stream, nullptr)) {
        LOG(ERROR) << "Failed to read geo data for image "
                   << image_name.toStdString();
        return false;
      }

      if (position_included && !ReadPosition(stream, nullptr)) {
        LOG(ERROR) << "Failed to read position for image "
                   << image_name.toStdString();
        return false;
      }

      if (orientation_included && !ReadOrientation(stream, nullptr)) {
        LOG(ERROR) << "Fadile to read orientation for image "
                   << image_name.toStdString();
        return false;
      }

      camera_intrinsics_prior->insert(image_name, temp_camera_intrinsics_prior);
    }
  }

  return true;
}

bool StorageIO::ReadYesNoExpression(QTextStream& stream,
                                    QString header, bool* yes) {
  QString temp_line;
  stream >> temp_line;
  if (temp_line != header) {
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
  double lat, lng, alt;
  stream >> lat >> lng >> alt;
  // TODO(uladbohdan): to assign values after it's implemented in TheiaSfm.
  return true;
}

bool StorageIO::ReadPosition(
    QTextStream& stream,
    CameraIntrinsicsPrior* temp_camera_intrinsics_prior) {
  double pos_x, pos_y, pos_z;
  stream >> pos_x >> pos_y >> pos_z;
  // TODO(uladbohdan): to assign values after it's implemented in TheiaSfm.
  return true;
}

bool StorageIO::ReadOrientation(
    QTextStream& stream,
    CameraIntrinsicsPrior* temp_camera_intrinsics_prior) {
  double or1, or2, or3;
  stream >> or1 >> or2 >> or3;
  // TODO(uladbohdan): to assign values after it's implemented in TheiaSfm.
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
