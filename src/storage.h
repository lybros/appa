// Copyright 2017 Lybros.

#ifndef SRC_STORAGE_H_
#define SRC_STORAGE_H_

#include <iostream>
#include <vector>
#include <string>

#include <QtAlgorithms>
#include <QDirIterator>
#include <QFileInfo>
#include <QStringList>
#include <QTextStream>
#include <QVector>

#include <theia/theia.h>
#include <glog/logging.h>

#include "io/storageio.h"
#include "featuresx.h"
#include "options.h"
#include "utils.h"

using theia::Reconstruction;

// The pattern may be extended with image extensions which are supported
// by Theia.
// TODO(uladbohdan): to achieve consistency in regexps.
const QString IMAGE_FILENAME_PATTERN = "\\b.(jpg|JPG|jpeg|JPEG|png|PNG)";
const QString MODEL_FILENAME_PATTERN = "*.model";

const QString DEFAULT_CALIBRATION_FILE_NAME = "camera_intrinsics.txt";

class Storage {
  friend class StorageIO;

 public:
  Storage();

  void SetOptions(Options* options);

  QString GetImagesPath();

  // Returns the number of images parsed from new source.
  int UpdateImagesPath(QString images_path);

  // In case we load the project - we want to be sure we're only adding
  // the images we've had before. This method allows to avoid parsing and
  // put only the images we pass.
  // Verification than will check if the images are still exist.
  bool ForceInitialize(QString images_path, const QVector<QString>& images);

  // Returns the number of images parsed.
  int ParseImageFolder();

  int NumberOfImages();

  QVector<QString>& GetImages();

  const QString& GetOutputLocation() const;

  bool SetOutputLocation(const QString& output_location);

  Reconstruction* GetReconstruction(const QString reconstruction_name);

  QStringList& GetReconstructions();

  bool GetCalibration(QMap<QString, theia::CameraIntrinsicsPrior>*);

  void LoadModelsList();

  ~Storage();

 private:
  // List of full paths to reconstructions (.model files) in filesystem.
  QStringList reconstructions_;
  // The only reason to have this field is to be able to deallocate memory
  // allocated for the reconstruction.
  Reconstruction* loaded_reconstruction_;
  QString loaded_reconstruction_name_;

  QVector<QString>* images_;
  QString images_path_;
  QString output_location_;

  Options* options_;

  // Reads one specific Reconstruction by name from filesystem.
  // As only one Reconstruction may be rendered at the same time, no need to
  // have them all loaded to memory.
  // As this method is private, you must call GetReconstruction() method, which
  // also checks if reconstruction is already in memory.
  void LoadReconstruction(QString reconstruction_name);

  QString GetCameraIntrinsicsPath() const;
};

#endif  // SRC_STORAGE_H_
