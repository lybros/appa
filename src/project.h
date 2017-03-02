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
 * OUTPUT_LOCATION "~/project0/out"
 * --------------------------------------------- */

#ifndef SRC_PROJECT_H_
#define SRC_PROJECT_H_

#include "storage.h"
#include "options.h"
#include "featuresx.h"

#include <iostream>
#include <string>
#include <vector>

#include <theia/theia.h>

#include <QString>
#include <QDir>
#include <QTextStream>

using theia::Reconstruction;
using theia::ReconstructionBuilder;
using theia::ReconstructionBuilderOptions;

const QString CONFIG_FILE_NAME = "project-config";
const QString DEFAULT_OUTPUT_LOCATION_POSTFIX = "out/";
const QString DEFAULT_MODEL_BINARY_FILENAME = "model-0.binary";

class Project {
public:
    Project();

    Project(QString project_name, QString project_path, QString images_path);

    void BuildModelToBinary();

    void ExtractFeatures();

    void MatchFeatures();

    // To run reconstruction assuming the features are already extracted and
    // matched.
    void StartReconstruction();

    // Try to find place for image on 3d reconstruction map
    void SearchImage(QString);

    QString GetProjectName();

    QString GetProjectPath();

    QString GetImagesPath();

    void SetProjectName(QString);

    void SetProjectPath(QString);

    void SetImagesPath(QString);

    bool WriteConfigurationFile();

    bool ReadConfigurationFile();

    QString GetDefaultOutputPath();

    ~Project();

private:
    Options *options_;

    QString project_name_;
    // project_path_ contains the full way to the project.
    // e.g. CONFIG_FILE_NAME is directly inside this folder.
    QString project_path_;
    QString output_location_;

    // images_path_ is stored inside of Storage object.
    Storage* storage_;
    Features* features_;

    QString GetConfigurationFilePath();
};

#endif  // SRC_PROJECT_H_
