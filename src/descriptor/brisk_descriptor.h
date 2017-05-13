// Copyright 2017 Lybros.

#ifndef SRC_DESCRIPTOR_BRISK_DESCRIPTOR_
#define SRC_DESCRIPTOR_BRISK_DESCRIPTOR_

#include "theia/theia.h"

using theia::DescriptorExtractor;

struct BriskParameters {
  // AGAST detection threshold score.
  int thresh=30;

  // Detection octaves. Use 0 to do single scale.
  int octaves=3;

  // Apply this scale to the pattern used for sampling the neighbourhood of
  // a keypoint.
  float patternScale=1.0f;
};

class BriskDescriptorExtractor : public DescriptorExtractor {
public:
  explicit BriskDescriptorExtractor(const BriskParameters& detector_params)
      : brisk_params_(detector_params) {}
  ~BriskDescriptorExtractor() {}

  // NOTE: This method will gracefully fail with a fatal logging method. BRISK
  // must use its own keypoints so only DetectAndExtract can be used.
  bool ComputeDescriptor(const theia::FloatImage& image,
                         const theia::Keypoint& keypoint,
                         Eigen::VectorXf* descriptor);

  // Detect keypoints using the Akaze keypoint detector and extracts them at the
  // same time.
  bool DetectAndExtractDescriptors(const theia::FloatImage& image,
                                   std::vector<theia::Keypoint>* keypoints,
                                   std::vector<Eigen::VectorXf>* descriptors);

 private:
  const BriskParameters brisk_params_;

  DISALLOW_COPY_AND_ASSIGN(BriskDescriptorExtractor);
};

#endif  // SRC_DESCRIPTOR_BRISK_DESCRIPTOR_
