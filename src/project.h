#ifndef PROJECT_H
#define PROJECT_H

#include <iostream>
#include <theia/theia.h>

using namespace std;
using theia::Reconstruction;
using theia::ReconstructionBuilder;
using theia::ReconstructionBuilderOptions;
const string out_matches_file = "out/matches.txt";

class Project {

public:
    Project();
    void RunReconstruction();

    std::string getProjectName();
    std::string getProjectPath();
    std::string getImagesPath();

    void setProjectName(std::string);
    void setProjectPath(std::string);
    void setImagesPath(std::string);

    ~Project();

private:
    ReconstructionBuilderOptions *options;

    std::string project_name;
    std::string project_path;
    std::string images_path;
};

#endif // PROJECT_H
