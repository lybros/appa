// Copyright 2017 Lybros.

/* In order to provide prior values for Camera Intrinsics, the file called
 * "camera_intrinsics.txt" must be in your IMAGES_LOCATION directory. It is
 * ignored unless the flag 'use_camera_intrinsics_prior' is set. Either all the
 * images or only a part of them may be specified in a file. Only one row is
 * enough if the 'shared_calibration' is set (the rest is ignored even if
 * provided).
 * The file format is:
 * ----------------------------------------------------------------------------
 * CAMERA_CALIBRATION_<TYPE>_1.0
 * NUMBER_OF_IMAGES <N>
 * <IMG_1_NAME> <parameters ... vary from TYPE>
 * ...
 * <IMG_N_NAME> ...
 * ----------------------------------------------------------------------------
 * The file format for PINHOLE camera:
 * ----------------------------------------------------------------------------
 * CAMERA_CALIBRATION_PINHOLE_1.0
 * NUMBER_OF_IMAGES N
 * <IMG_1_NAME> <focal_length> <px> <py> <skew> <aspect_ratio> <rad1> <rad2>
 * ...
 * <IMG_N_NAME> ...
 * ----------------------------------------------------------------------------
 */

#ifndef SRC_IO_STORAGEIO_H_
#define SRC_IO_STORAGEIO_H_

#include <QMap>
#include <QString>
#include <QTextStream>
#include <QVector>

#include "theia/theia.h"

class Storage;

class StorageIO {
 public:
  explicit StorageIO(Storage* storage);

  bool ReadCalibrationFile(
      QString calibration_file_path,
      QMap<QString, theia::CameraIntrinsicsPrior>* camera_intrinsics_prior);

  ~StorageIO();

 private:
  Storage* storage_;

  bool ReadCalibrationFileRow(QTextStream* stream,
    theia::CameraIntrinsicsPrior* temp_camera_intrinsics_prior);
};

#endif  // SRC_IO_STORAGEIO_H_
