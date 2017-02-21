#ifndef PROJECT_H
#define PROJECT_H

#include <iostream>
#include <theia/theia.h>

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

    QString getProjectName();
    QString getProjectPath();
    QString getImagesPath();

    void setProjectName(QString);
    void setProjectPath(QString);
    void setImagesPath(QString);

    bool WriteConfigurationFile();

    ~Project();

private:
    ReconstructionBuilderOptions *options;

    QString project_name;
    QString project_path;
    QString images_path;

    QString getConfigurationFilePath();
};

#endif // PROJECT_H
