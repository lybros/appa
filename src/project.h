// Copyright 2017 Lybros.

/* The project structure is:
 * project_name_folder
 * |--"project-config"
 *
 * project-config file is:
 * ----------------------------------------------------------------------------
 * PROJECT_CONFIG_VERSION v1.0
 * PROJECT_NAME "project-name"
 * IMAGES_LOCATION "~/datasets/dataset0/"
 * NUMBER_OF_IMAGES N_IMAGES
 * "name0.jpg"
 * ...
 * "nameN.jpg"
 * OUTPUT_LOCATION "~/project0/out"
 * ----------------------------------------------------------------------------
 * Notice: names of images should be sorted in ascending order. Images are
 * automatically sorted during the parsing, but you should keep that in mind if
 * you're creating a project config manually.
 *
 * In order to provide prior values for Camera Intrinsics, the file called
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
 *
 ******************************************************************************/

#ifndef SRC_PROJECT_H_
#define SRC_PROJECT_H_

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include <QDir>
#include <QSet>
#include <QString>
#include <QTextStream>

#include <theia/theia.h>
#include <theia/matching/distance.h>

#include "featuresx.h"
#include "options.h"
#include "storage.h"

using theia::Reconstruction;
using theia::ReconstructionBuilder;
using theia::ReconstructionBuilderOptions;

const QString CONFIG_FILE_NAME = "project-config";
const QString DEFAULT_OUTPUT_LOCATION_POSTFIX = "out/";
const QString DEFAULT_MODEL_BINARY_FILENAME = "model-0.binary";

class Project {
 public:
  // This constructor must be called if we're opening an existent project.
  // The Project::ReadConfigurationFile() is called.
  explicit Project(QString project_path);

  // This constructor must be called if we're creating new project. That will
  // create a new project folder in filesystem and call
  // Project::WriteConfigurationFile() method.
  Project(QString project_name, QString project_path, QString images_name);

  // Simple build from scratch and save into a binary file.
  // Covers all stages all together:
  // * Extracting features, saving them into a filesystem.
  // * Matching features.
  // * Building a 3D model and saving it into a binary file in filesystem.
  //
  // May take plenty of time to finish processing.
  //
  // The recommendation is to run every stage separately.
  void BuildModelToBinary();

  void ExtractFeatures();

  void MatchFeatures();

  // To run reconstruction assuming the features are already extracted and
  // matched.
  void StartReconstruction();

  // Try to find place for image on 3d reconstruction map
  void SearchImage(QString file_path, QSet<theia::TrackId>* h_tracks);

  QString GetProjectName();

  QString GetProjectPath();

  QString GetImagesPath();

  void SetProjectName(QString);

  void SetProjectPath(QString);

  void SetImagesPath(QString);

  bool WriteConfigurationFile();

  bool ReadConfigurationFile();

  QString GetDefaultOutputPath();

  Storage* GetStorage();

  Options* GetOptions();

  ~Project();

 private:
  Options* options_;

  QString project_name_;
  // project_path_ contains the full way to the project.
  // e.g. CONFIG_FILE_NAME is directly inside this folder.
  QString project_path_;
  // images_path_ and reconstructions_ is stored inside of Storage object.
  Storage* storage_;
  Features* features_;

  QString GetConfigurationFilePath();
};

#endif  // SRC_PROJECT_H_
