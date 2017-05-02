// Copyright 2017 Lybros.

#ifndef SRC_IO_REPORT_H_
#define SRC_IO_REPORT_H_

#include <QString>

class Options;
class Project;
class Storage;

// Report class generates a report with flags and timing for extracting/
// matching/reconstruction.
class Report {
  friend class Reconstructor;

 public:
  explicit Report(Project* project);
  ~Report();

  bool GenerateSmartReconstructionReport(QString filepath);

 private:
  Options* options_;
  Project* project_;
  Storage* storage_;

  // Options listed below are extraction/matching/reconstruction options,
  // which are not explicitly specified in options_.
  // No getters/setters are implemented; may be set from friend classes.

  // RECONSTRUCTION options.
  bool using_calibration_file_;
  bool using_prebuilt_matches_;

  // Timing (in seconds).
  double overall_time_;
  double extraction_time_;
  double matching_time_;
  double extraction_matching_time_;
  double reading_matches_time_;
  double reconstruction_time_;
  double colorizing_time_;
};

#endif  // SRC_IO_REPORT_H_
