// Copyright 2017 Lybros.

#ifndef SRC_FEATURESX_H_
#define SRC_FEATURESX_H_

#include <iostream>
#include <string>
#include <vector>
#include <utility>

#include <QFileInfo>
#include <QString>
#include <QVector>

#include <theia/theia.h>

#include "options.h"
#include "storage.h"
#include "utils.h"

using theia::DescriptorExtractor;
using theia::ReadKeypointsAndDescriptors;
using theia::Keypoint;

class Features {
 public:
  Features(Storage* storage, Options* options);

  typedef std::pair<std::vector
      <theia::Keypoint>, std::vector<Eigen::VectorXf>
  > FeatureVectors;
  typedef std::unordered_map<std::string, FeatureVectors> FeaturesMap;

  // Extract features only if doesn't find *.features files.
  void Extract();

  // Read features for all images from storage use *.features files
  // or if files not found, extract it
  void Extract(std::vector<std::vector<theia::Keypoint> >* keypoints,
               std::vector<std::vector<Eigen::VectorXf> >* descriptors);

  // Rewrite all *.features files.
  void ForceExtract();

  // Extract features for single image. Doesn't work with disc.
  void ExtractFeature(QString filename,
                      std::vector<theia::Keypoint>* keypoints,
                      std::vector<Eigen::VectorXf>* descriptors);

  // not used yet, maybe removed in the future
  // Read image from storage and extract descriptor for passed Feature
  void GetDescriptor(const std::string image_name,
                     const theia::Feature* feature,
                     Eigen::VectorXf* descriptor);

  // Load features and descriptors,
  // return map with <image name> as key
  // and Pair of Keypoints and Descriptors for that image as value
  FeaturesMap* GetFeaturesMap();

  void Match();

  void ForceMatch();

  ~Features();

 protected:
  void _extract(bool is_force);

  void _match();

 private:
  QString out_path_;
  QVector<QString> images_;
  Storage* storage_;

  // This is the pointer to the same options_ as we have in Project.
  // Options in Project may be modified (e.g. through OptionsDialog), that
  // means we need to get actual options before any extracting-related actions.
  Options* options_;
};

#endif  // SRC_FEATURESX_H_
