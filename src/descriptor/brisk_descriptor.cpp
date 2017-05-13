// Copyright 2017 Lybros.

#include "brisk_descriptor.h"

#include "glog/logging.h"
#include "opencv2/features2d/features2d.hpp"

#include "../theia_utils.h"

bool BriskDescriptorExtractor::ComputeDescriptor(
    const theia::FloatImage& image, const theia::Keypoint& keypoint,
    Eigen::VectorXf *descriptor) {
  LOG(FATAL) << "BRISK uses its own keypoints (incompatible with "
                "theia::Keypoint), only "
                "BriskDescriptorExtractor::DetectAndExtract() method "
                "can be called.";
  return false;
}

bool BriskDescriptorExtractor::DetectAndExtractDescriptors(
    const theia::FloatImage& image, std::vector<theia::Keypoint>* keypoints,
    std::vector<Eigen::VectorXf>* descriptors) {

  cv::Mat* cvimage = TheiaImageToOpenCVImage(image);

  std::vector<cv::KeyPoint> cvkeypoints;
  cv::Mat cvdescriptors;

  cv::BRISK::create(brisk_params_.thresh, brisk_params_.octaves,
    brisk_params_.patternScale)->detectAndCompute(*cvimage,
                                                  cv::noArray(),
                                                  cvkeypoints,
                                                  cvdescriptors);

  delete cvimage;

  keypoints->resize(cvkeypoints.size());
  for (int i = 0; i < cvkeypoints.size(); i++) {
    (*keypoints)[i] = theia::Keypoint(cvkeypoints[i].pt.x, cvkeypoints[i].pt.y,
                                   theia::Keypoint::KeypointType::OTHER);
    (*keypoints)[i].set_strength(cvkeypoints[i].response);
  }
  descriptors->resize(cvdescriptors.rows);
  int BRISK_DESCRIPTOR_SIZE = cvdescriptors.cols;
  for (int i = 0; i < cvdescriptors.rows; i++) {
    (*descriptors)[i] = Eigen::VectorXf(BRISK_DESCRIPTOR_SIZE);
    for (int j = 0; j < BRISK_DESCRIPTOR_SIZE; j++) {
      descriptors->at(i)[j] = cvdescriptors.at<uchar>(i,j);
    }
  }

  CHECK_EQ(keypoints->size(), descriptors->size())
      << "BRISK: Difference in number of keypoints and descriptors.";

  return true;
}
