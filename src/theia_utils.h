// Copyright 2017 Lybros.

#ifndef SRC_THEIA_UTILS_H_
#define SRC_THEIA_UTILS_H_

#include <QString>

#include "opencv2/features2d/features2d.hpp"
#include "theia/theia.h"

using theia::DescriptorExtractorType;
using theia::MatchingStrategy;
using theia::OptimizeIntrinsicsType;

QString DescriptorExtractorTypeToString(DescriptorExtractorType);

DescriptorExtractorType DescriptorExtractorTypeFromString(QString);

QString MatchingStrategyToString(MatchingStrategy);

MatchingStrategy MatchingStrategyFromString(QString);

// The string must have following values:
// focal    FOCAL_LENGTH
// aratio   ASPECT_RATIO
// skew     SKEW
// ppoints  PRINCIPAL_POINTS
// raddist  RADIAL_DISTORTION
// tandist  TANGENTIAL_DISTORTION
// separated with comma with no spaces. Example: "focal,skew,raddist"
// Returns NONE if empty or invalid string.
OptimizeIntrinsicsType OptimizeIntrinsicsTypeFromString(QString);

cv::Mat* TheiaImageToOpenCVImage(const theia::FloatImage& image);

#endif  // SRC_THEIA_UTILS_H_
