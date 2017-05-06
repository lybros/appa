// Copyright 2017 Lybros.

#include "options.h"

#include "gflags/gflags.h"

#include "theia_utils.h"

// Options which are common both for UI and CLI mode must be specified in this
// file. These options are then parsed if Options::ParseCommandLineArguments()
// method.

// THEIA's flags.
// The full description may be found in Theia's docs or in Theia's
// //applications/build_reconstruction.cc file.

// General.
DEFINE_int32(num_threads, 4, "");

// Feature extraction.
DEFINE_string(descriptor_type, "SIFT", "");

// Feature matching.
DEFINE_bool(match_out_of_core, true, "");
DEFINE_bool(perform_geometric_verification, false, "");
DEFINE_string(match_strategy, "CascadeHashing", "");

// Reconstruction.
DEFINE_bool(shared_calibration, true, "");
DEFINE_bool(use_camera_intrinsics_prior, false, "");
DEFINE_string(intrinsics_to_optimize, "focal,raddist", "");

Options::Options(QString output_location) : output_location_(output_location) {
  ParseCommandLineArguments();
}

void Options::ParseCommandLineArguments() {
  num_threads_ = FLAGS_num_threads;

  descriptor_type_ = DescriptorExtractorTypeFromString(
        QString::fromStdString(FLAGS_descriptor_type));

  match_out_of_core_ = FLAGS_match_out_of_core;
  perform_geometric_verification_ = FLAGS_perform_geometric_verification;
  match_strategy_ = MatchingStrategyFromString(
        QString::fromStdString(FLAGS_match_strategy));

  shared_calibration = FLAGS_shared_calibration;
  use_camera_intrinsics_prior = FLAGS_use_camera_intrinsics_prior;

  intrinsics_to_optimize_ = OptimizeIntrinsicsTypeFromString(
        QString::fromStdString(FLAGS_intrinsics_to_optimize));

  LOG(INFO) << "Options flags parsed successfully.";
}

ReconstructionBuilderOptions Options::GetReconstructionBuilderOptions() {
  ReconstructionBuilderOptions options;

  options.descriptor_type = descriptor_type_;
  options.output_matches_file = QDir(output_location_).
      filePath("matches.binary").toStdString();
  options.num_threads = num_threads_;
  options.matching_strategy = match_strategy_;

  options.matching_options = GetFeatureMatcherOptions();
  options.reconstruction_estimator_options =
      GetReconstructionEstimatorOptions();

  return options;
}

FeatureExtractor::Options Options::GetFeatureExtractorOptions() {
  FeatureExtractor::Options options;

  options.descriptor_extractor_type = descriptor_type_;
  options.output_directory = QDir(output_location_).
      filePath("features/").toStdString();
  options.num_threads = num_threads_;

  return options;
}

FeatureMatcherOptions Options::GetFeatureMatcherOptions() {
  FeatureMatcherOptions options;

  options.num_threads = num_threads_;
  options.keypoints_and_descriptors_output_dir = QDir(output_location_)
      .filePath("features").toStdString();
  options.match_out_of_core = match_out_of_core_;
  options.perform_geometric_verification =
      perform_geometric_verification_;

  return options;
}

ReconstructionEstimatorOptions Options::GetReconstructionEstimatorOptions() {
  ReconstructionEstimatorOptions options;
  options.intrinsics_to_optimize = intrinsics_to_optimize_;
  return options;
}

Options::~Options() {}
