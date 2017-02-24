// Copyright 2017 Lybros.

/* The project structure is:
 * project_name_folder
 * |--"project-config"
 *
 * project-config file is:
 * ---------------------------------------------
 * PROJECT_CONFIG_VERSION v1.0
 * PROJECT_NAME "project-name"
 * IMAGES_LOCATION "~/datasets/dataset0/"
 * NUMBER_OF_IMAGES N_IMAGES
 * "name0.jpg"
 * ...
 * "nameN.jpg"
 * OUTPUT_LOCATION "~/models/model0" // if "DEFAULT"
 *     "out/" directory is used.
 * --------------------------------------------- */

#ifndef SRC_PROJECT_H_
#define SRC_PROJECT_H_

#include "storage.h"

#include <iostream>
#include <string>
#include <vector>

#include <theia/theia.h>

#include <QString>
#include <QDir>
#include <QTextStream>

using namespace std;
using theia::Reconstruction;
using theia::ReconstructionBuilder;
using theia::ReconstructionBuilderOptions;

// DEPRECATED.
// TODO(uladbohdan/drapegnik): to replace all usages with output_location_ path.
const string out_matches_file = "out/matches.txt";

const QString CONFIG_FILE_NAME = "project-config";
const QString DEFAULT_OUTPUT_LOCATION_POSTFIX = "out/";

class Project {
 public:
    Project();

    Project(QString project_name, QString project_path, QString images_path);

    // The method is aimed to start with raw data and finish with ready-to-use
    // model. TODO: to rename.
    void RunReconstruction();

    void ExtractFeatures();

    void MatchFeatures();

    // To run reconstruction assuming the features are already extracted and
    // matched.
    void StartReconstruction();

    QString GetProjectName();
    QString GetProjectPath();
    QString GetImagesPath();

    void SetProjectName(QString);
    void SetProjectPath(QString);
    void SetImagesPath(QString);

    bool WriteConfigurationFile();
    bool ReadConfigurationFile();

    ~Project();

 private:
    ReconstructionBuilderOptions *options_;

    QString project_name_;
    // project_path_ contains the full way to the project.
    // e.g. CONFIG_FILE_NAME is directly inside this folder.
    QString project_path_;
    QString output_location_;

    // images_path_ is stored inside of Storage object.
    Storage* storage_;

    QString GetConfigurationFilePath();
    QString GetDefaultOutputPath();
};

#endif  // SRC_PROJECT_H_
