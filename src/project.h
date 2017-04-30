// Copyright 2017 Lybros.

/* The project structure is:
 * project_name_folder
 * |--"project-config"
 *
 * More about the structure in io/ firectory.
 */

#ifndef SRC_PROJECT_H_
#define SRC_PROJECT_H_

#include <iostream>
#include <string>
#include <vector>

#include <QDir>
#include <QMap>
#include <QSet>
#include <QString>
#include <QTextStream>

#include <theia/theia.h>
#include <theia/matching/distance.h>

#include "io/projectio.h"
#include "featuresx.h"
#include "options.h"
#include "reconstructor.h"
#include "storage.h"

const QString CONFIG_FILE_NAME = "project-config";
const QString DEFAULT_OUTPUT_LOCATION_POSTFIX = "out/";
const QString DEFAULT_MODEL_BINARY_FILENAME = "model-0.binary";

class Project {
  friend class ProjectIO;

 public:
  // This constructor must be called if we're opening an existent project.
  // The Project::ReadConfigurationFile() is called.
  explicit Project(QString project_path);

  // This constructor must be called if we're creating new project. That will
  // create a new project folder in filesystem and call
  // Project::WriteConfigurationFile() method.
  Project(QString project_name, QString project_path, QString images_name);

  // Runs Smart building process, which figures out if we need matching stage
  // or it has been already run.
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
