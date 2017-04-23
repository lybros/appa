// Copyright 2017 Lybros.

#ifndef SRC_STORAGE_H_
#define SRC_STORAGE_H_

#include <iostream>
#include <vector>
#include <string>

#include <QtAlgorithms>
#include <QDirIterator>
#include <QFileInfo>
#include <QTextStream>
#include <QVector>

#include <theia/theia.h>
#include <glog/logging.h>

#include "options.h"
#include "utils.h"

using theia::Reconstruction;
// TODO(uladbohdan): to remove after separate IO module is implemented.
using theia::CameraIntrinsicsPrior;

// The pattern may be extended with image extensions which are supported
// by Theia.
const QString IMAGE_FILENAME_PATTERN = "\\b.(jpg|JPG|jpeg|JPEG|png|PNG)";
const QString MODEL_FILENAME_PATTERN = "model-\\d+.binary";

const QString DEFAULT_CALIBRATION_FILE_NAME = "camera_intrinsics.txt";

class Storage {
 public:
  Storage();

  explicit Storage(QString images_path);

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

  void SetOutputLocation(const QString& output_location);

  // check if model already in memory, load it if not, ad return
  Reconstruction* GetReconstruction(const int number);

  // update model in memory and sets status_ to LOADED_IN_MEMORY
  void SetReconstructions(const std::vector<Reconstruction*>& reconstructions);

  // write all models to binary file
  void WriteReconstructions();

  void SetReconstructionStatus(ReconstructionStatus status);

  ReconstructionStatus GetReconstructionStatus() const;

  bool ReadCalibration(
      std::unordered_map<std::string, theia::CameraIntrinsicsPrior>*
      camera_intrinsics_prior);

  QString GetCameraIntrinsicsPath() const;

  ~Storage();

 private:
  std::vector<Reconstruction*> reconstructions_;
  QVector<QString>* images_;
  QString images_path_;
  QString output_location_;
  ReconstructionStatus status_;

  Options* options_;

  // Reads model from binary file.
  void ReadReconstructions();

  bool ReadCalibrationRow(QTextStream* stream,
    theia::CameraIntrinsicsPrior* temp_camera_intrinsics_prior);
};

#endif  // SRC_STORAGE_H_
