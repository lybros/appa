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
 * -------------------------------------------------------------------------
 * Notice: names of images should be sorted in ascending order. Images are
 * automatically sorted during the parsing, but you should keep that in mind if
 * you're creating a project config manually.
 */

#ifndef SRC_PROJECT_H_
#define SRC_PROJECT_H_

#include <iostream>
#include <string>
#include <vector>

#include <QDir>
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
  Project();

    Project(QString project_name, QString project_path, QString images_name);

  void BuildModelToBinary();

  void ExtractFeatures();

  void MatchFeatures();

  // To run reconstruction assuming the features are already extracted and
  // matched.
  void StartReconstruction();

    // Try to find place for image on 3d reconstruction map
    void SearchImage(QString file_path, QString model_path);

  QString GetProjectName();

  QString GetProjectPath();

  QString GetImagesPath();

  QString GetOutputLocation();

  void SetProjectName(QString);

  void SetProjectPath(QString);

  void SetImagesPath(QString);

  bool WriteConfigurationFile();

  bool ReadConfigurationFile();

  QString GetDefaultOutputPath();

  Storage* GetStorage();

  std::vector<std::shared_ptr<theia::Reconstruction>>& GetReconstructions();

  ~Project();

 private:
  Options* options_;

  QString project_name_;
  // project_path_ contains the full way to the project.
  // e.g. CONFIG_FILE_NAME is directly inside this folder.
  QString project_path_;
  QString output_location_;

  // images_path_ is stored inside of Storage object.
  Storage* storage_;

  Features* features_;

  QString GetConfigurationFilePath();

  std::vector<std::shared_ptr<theia::Reconstruction>> reconstructions_;
};

#endif  // SRC_PROJECT_H_
