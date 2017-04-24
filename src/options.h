// Copyright 2017 Lybros.

#ifndef SRC_OPTIONS_H_
#define SRC_OPTIONS_H_

/* This class aims to store all the options required by one particular instance
 * of the Project type. Feature Extraction, Feature Matching, Reconstruction,
 * Visualization options.
 * Some of the options will be hardcoded, some can be changed in runtime using
 * program arguments or some GUI forms.
 *
 * The rules of adding new options:
 * 1. No duplicates! Every option must be defined only once.
 * 2. Some options can be organized as a subclass or a structure to be simply
 *    forwarded to other libraries/algorithms.
 * 3. Options required by Theia (any other libs) may be stored explicitly or
 *    be generated in runtime (e.g. ReconstructionBuilderOptions may be
 *    generated from a bunch of options previously defined for
 *    Matching/Extraction parts of an application.
 *
 * Feature requests:
 * - options (at least, some of them) will be stored in config file (or a
 *   separate file). Read/Write methods must be implemented.
 *
******************************************************************************/

#include <QDir>
#include <QString>

#include <theia/theia.h>

// TODO(uladbohdan): to be consistent in using "using" keyword.
using theia::MatchingStrategy;
using theia::DescriptorExtractorType;
using theia::ReconstructionBuilderOptions;

class Options {
  friend class OptionsDialog;
  friend class Project;
  friend class Storage;
  friend class StorageIO;

 public:
  explicit Options(QString output_location);

  theia::ReconstructionBuilderOptions GetReconstructionBuilderOptions();

  theia::FeatureExtractor::Options GetFeatureExtractorOptions();

  theia::ReconstructionEstimatorOptions GetReconstructionEstimatorOptions();

  ~Options();

 private:
  // The parameters of the related Project object.
  QString output_location_;

  // THEIA's OPTIONS.
  int num_threads_ = 4;

  // Feature Extraction.
  DescriptorExtractorType descriptor_type_ = DescriptorExtractorType::SIFT;

  // Feature Matching.
  bool match_out_of_core_ = true;
  bool perform_geometric_verification_ = false;
  MatchingStrategy match_strategy_ = MatchingStrategy::CASCADE_HASHING;

  // Reconstruction.
  bool shared_calibration = true;
  bool use_camera_intrinsics_prior = false;

  // Reconstruction Estimator Options.
  theia::OptimizeIntrinsicsType intrinsics_to_optimize_ =
      theia::OptimizeIntrinsicsType::FOCAL_LENGTH |
      theia::OptimizeIntrinsicsType::RADIAL_DISTORTION;
};

enum ReconstructionStatus {
  NOT_BUILT = 0,              // There is no any models.
  BUILT = 1,                  // Models built and saved into filesystem.
  LOADED_INTO_MEMORY = 2      // Models read from disk and stored into Storage.
};

#endif  // SRC_OPTIONS_H_
