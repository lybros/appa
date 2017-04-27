// Copyright 2017 Lybros.

#ifndef SRC_RECONSTRUCTOR_H_
#define SRC_RECONSTRUCTOR_H_

#include "theia/theia.h"

using theia::Reconstruction;
using theia::ReconstructionBuilder;
using theia::ReconstructionBuilderOptions;

class Project;

class Reconstructor {
 public:
  explicit Reconstructor(Project* project);
  ~Reconstructor();

  void SmartBuild();

 private:
  Project* project_;
};

#endif  // SRC_RECONSTRUCTOR_H_
