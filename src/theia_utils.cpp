// Copyright 2017 Lybros.

#include "theia_utils.h"

#include <QMap>

QString DescriptorExtractorTypeToString(DescriptorExtractorType descriptor) {
  return QMap<theia::DescriptorExtractorType, QString>({
    {theia::DescriptorExtractorType::SIFT, "SIFT"},
    {theia::DescriptorExtractorType::AKAZE, "AKAZE"},
  })[descriptor];
}

DescriptorExtractorType DescriptorExtractorTypeFromString(QString str) {
  return QMap<QString, theia::DescriptorExtractorType>({
    {"SIFT", theia::DescriptorExtractorType::SIFT},
    {"AKAZE", theia::DescriptorExtractorType::AKAZE},
  })[str];
}

QString MatchingStrategyToString(MatchingStrategy strategy) {
  return QMap<theia::MatchingStrategy, QString>({
    {theia::MatchingStrategy::BRUTE_FORCE, "Brute Force"},
    {theia::MatchingStrategy::CASCADE_HASHING, "Cascade Hashing"},
  })[strategy];
}

MatchingStrategy MatchingStrategyFromString(QString str) {
  return QMap<QString, theia::MatchingStrategy>({
    {"Brute Force", theia::MatchingStrategy::BRUTE_FORCE},
    {"BruteForce", theia::MatchingStrategy::BRUTE_FORCE},
    {"Cascade Hashing", theia::MatchingStrategy::CASCADE_HASHING},
    {"CascadeHashing", theia::MatchingStrategy::CASCADE_HASHING},
  })[str];
}

OptimizeIntrinsicsType OptimizeIntrinsicsTypeFromString(QString str) {
  OptimizeIntrinsicsType type = OptimizeIntrinsicsType::NONE;

  QStringList options = str.split(",", QString::SkipEmptyParts);

  if (options.contains("focal")) {
    type |= OptimizeIntrinsicsType::FOCAL_LENGTH;
  }
  if (options.contains("aratio")) {
    type |= OptimizeIntrinsicsType::ASPECT_RATIO;
  }
  if (options.contains("skew")) {
    type |= OptimizeIntrinsicsType::SKEW;
  }
  if (options.contains("ppoints")) {
    type |= OptimizeIntrinsicsType::PRINCIPAL_POINTS;
  }
  if (options.contains("raddist")) {
    type |= OptimizeIntrinsicsType::RADIAL_DISTORTION;
  }
  if (options.contains("tandist")) {
    type |= OptimizeIntrinsicsType::TANGENTIAL_DISTORTION;
  }

  return type;
}
