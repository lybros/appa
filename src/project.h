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
    ~Project();

private:
    ReconstructionBuilderOptions *options;
};

#endif // PROJECT_H
