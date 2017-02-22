#ifndef PROJECT_H
#define PROJECT_H

#include <iostream>
#include <theia/theia.h>

#include "storage.h"

#include <QString>
#include <QDir>
#include <QTextStream>

using namespace std;
using theia::Reconstruction;
using theia::ReconstructionBuilder;
using theia::ReconstructionBuilderOptions;
const string out_matches_file = "out/matches.txt";

class Project {

public:
    Project();

    void RunReconstruction();

    QString GetProjectName();
    QString GetProjectPath();
    QString GetImagesPath();

    void SetProjectName(QString);
    void SetProjectPath(QString);
    void SetImagesPath(QString);

    bool WriteConfigurationFile();

    ~Project();

private:
    ReconstructionBuilderOptions *options_;

    QString project_name_;
    QString project_path_;
    QString images_path_;

    Storage* storage_;

    QString GetConfigurationFilePath();
};

#endif // PROJECT_H
