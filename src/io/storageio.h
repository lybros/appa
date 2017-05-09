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
 * GEO_DATA_INCLUDED <YES/NO>
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
 * GEO_DATA_INCLUDED YES
 * NUMBER_OF_IMAGES N
 * <focal_length> <px> <py> <skew> <aspect_ratio> <rad1> <rad2>
 * <IMG_1_NAME> <lat> <long> <alt>
 * ...
 * <IMG_N_NAME> ...
 * ----------------------------------------------------------------------------
 * Notice: the geolocation values (latitude, longitude, altitude) are single
 * decimal values. Latitude must be below zero if it's South; longitude must be
 * below zero if it's West.
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
  // dataset. The method only tries to return data, specified with options
  // Options::use_camera_intrinsics_prior_ and Options::use_geodata_.
  // The method also checks and returns only images which are are known to
  // Storage as images from dataset.
  // Note that if file uses SHARED_CALIBRATION, it will be applied for every
  // image listed. At the same time, Geo data may be only unique (must be
  // specified separately for every image).
  bool ReadCalibrationFile(
      QString calibration_file_path,
      QMap<QString, CameraIntrinsicsPrior>* camera_intrinsics_prior,
      bool* shared_calibration,
      bool* geo_data_included);

  void WriteReconstructions(
      const std::vector<theia::Reconstruction*>& reconstructions);

  ~StorageIO();

 private:
  Storage* storage_;

  bool ReadCalibrationFile_Pinhole(
      QTextStream& stream,
      QMap<QString, CameraIntrinsicsPrior>* camera_intrinsics_prior,
      bool* shared_calibration,
      bool* geo_data_included);

  bool ReadSharedCalibration(QTextStream& stream, bool* yes);
  bool GeoDataIncluded(QTextStream& stream, bool* yes);
  bool ReadNumberOfImages(QTextStream& stream, int* num);
  bool ReadCameraIntrinsics_Pinhole(QTextStream& stream,
      CameraIntrinsicsPrior* temp_camera_intrinsics_prior);
  bool ReadGeoData(QTextStream& stream,
      CameraIntrinsicsPrior* temp_camera_intrinsics_prior);
};

#endif  // SRC_IO_STORAGEIO_H_
