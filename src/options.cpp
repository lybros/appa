// Copyright 2017 Lybros.

#include "options.h"

Options::Options(QString output_location) : output_location_(output_location) {}

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
