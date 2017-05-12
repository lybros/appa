// Copyright 2017 Lybros.

#ifndef SRC_RECONSTRUCTOR_H_
#define SRC_RECONSTRUCTOR_H_

#include "theia/theia.h"

#include "io/report.h"
#include "options.h"
#include "project.h"
#include "storage.h"

using theia::CameraIntrinsicsPrior;
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
  Storage* storage_;
  Options* options_;

  bool ReadMatches(ReconstructionBuilder* reconstruction_builder);
  bool ExtractFeaturesMatches(ReconstructionBuilder* reconstruction_builder);

  Report* report_;
};

#endif  // SRC_RECONSTRUCTOR_H_
