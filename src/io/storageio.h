// Copyright 2017 Lybros.

/* In order to provide prior values for Camera Intrinsics, the file called
 * "camera_intrinsics.txt" must be in your IMAGES_LOCATION directory. It is
 * ignored unless the flag 'use_camera_intrinsics_prior' is set. Either all the
 * images or only a part of them may be specified in a file. Only one row is
 * enough if the 'shared_calibration' is set (the rest is ignored even if
 * provided).
 * The file format is:
 * ----------------------------------------------------------------------------
 * CAMERA_CALIBRATION_<TYPE>_1.1
 * SHARED_CALIBRATION <YES/NO>
 * GEO_DATA <YES/NO>
 * POSITION <YES/NO>
 * ORIENTATION <YES/NO>
 * NUMBER_OF_IMAGES <N>
 * <camera inner parameters ... vary from TYPE ... if SHARED_CALIBRATION is YES>
 * <IMG_1_NAME> <parameters ... vary from TYPE ... if SHARED_CALIBRATION is NO>
 *              <lat, long, alt if GEO_DATA_INCLUDED is YES>
 * ...
 * <IMG_N_NAME> ...
 * ----------------------------------------------------------------------------
 * The file format for PINHOLE camera:
 * ----------------------------------------------------------------------------
 * CAMERA_CALIBRATION_PINHOLE_1.1
 * SHARED_CALIBRATION YES
 * GEO_DATA YES
 * POSITION YES
 * ORIENTATION YES
 * NUMBER_OF_IMAGES N
 * <focal_length> <px> <py> <skew> <aspect_ratio> <rad1> <rad2>
 * <IMG_1_NAME> <lat> <long> <alt> <pos_x> <pos_y> <pos_z> <or1> <or2> <or3>
 * ...
 * <IMG_N_NAME> ...
 * ----------------------------------------------------------------------------
 * Notice: the geolocation values (latitude, longitude, altitude) are single
 * decimal values. Latitude must be below zero if it's South; longitude must be
 * below zero if it's West.
 *
 * Notice: currently Geo, position and orientation data is ignored as it's not
 * yet implemented in Theia.
 */

#ifndef SRC_IO_STORAGEIO_H_
#define SRC_IO_STORAGEIO_H_

#include <vector>

#include <QMap>
#include <QString>
#include <QTextStream>
#include <QVector>

#include "theia/theia.h"

using theia::CameraIntrinsicsPrior;

class Storage;

class StorageIO {
 public:
  explicit StorageIO(Storage* storage);

  // ReadCalibrationFile attempts to read calibration from file provided with
  // dataset. The method simply reads the file with data analisys. If shared
  // calibration is enabled, the map will include all images listed in a file
  // with the same calibration parameters (you may want to use only one
  // instance).
  // Note that geo, position and orientaion data is currently ignored.
  bool ReadCalibrationFile(
      QString calibration_file_path,
      QMap<QString, CameraIntrinsicsPrior>* camera_intrinsics_prior,
      bool* shared_calibration);

  void WriteReconstructions(
      const std::vector<theia::Reconstruction*>& reconstructions);

  ~StorageIO();

 private:
  Storage* storage_;

  bool ReadCalibrationFile_Pinhole(
      QTextStream& stream,
      QMap<QString, CameraIntrinsicsPrior>* camera_intrinsics_prior,
      bool* shared_calibration);

  bool ReadNumberOfImages(QTextStream& stream, int* num);
  bool ReadCameraIntrinsics_Pinhole(QTextStream& stream,
      CameraIntrinsicsPrior* temp_camera_intrinsics_prior);
  bool ReadGeoData(QTextStream& stream,
      CameraIntrinsicsPrior* temp_camera_intrinsics_prior);
  bool ReadPosition(QTextStream& stream,
      CameraIntrinsicsPrior* temp_camera_intrinsics_prior);
  bool ReadOrientation(QTextStream& stream,
      CameraIntrinsicsPrior* temp_camera_intrinsics_prior);

  // Helpers.
  bool ReadYesNoExpression(QTextStream& stream, QString header, bool* yes);
};

#endif  // SRC_IO_STORAGEIO_H_
