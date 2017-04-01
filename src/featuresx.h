// Copyright 2017 Lybros.

#ifndef SRC_FEATURES_H_
#define SRC_FEATURES_H_

#include <iostream>

#include <QFileInfo>
#include <QString>
#include <QVector>

#include <theia/theia.h>

#include "storage.h"
#include "utils.h"

using theia::DescriptorExtractor;
using theia::ReadKeypointsAndDescriptors;
using theia::Keypoint;

class Features {
public:
    Features(Storage* storage, QString out_path);

    // Extract features only if doesn't find *.features files.
    void Extract();

    // Read features for all images from storage use *.features files
    // or if files not found, extract it
    void Extract(
            std::vector<std::vector<theia::Keypoint> >* keypoints,
            std::vector<std::vector<Eigen::VectorXf> >* descriptors
    );

    // Rewrite all *.features files.
    void ForceExtract();

    // Extract features for single image. Doesn't work with disc.
    void ExtractFeature(
            QString filename,
            std::vector<theia::Keypoint>* keypoints,
            std::vector<Eigen::VectorXf>* descriptors
    );

    // not used yet, maybe removed in the future
    // Read image from storage and extract descriptor for passed Feature
    void GetDescriptor(
            const std::string image_name,
            const theia::Feature* feature,
            Eigen::VectorXf* descriptor);

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
    theia::FeatureExtractor::Options options_;
    theia::FeatureExtractor* extractor_;
};

#endif  // SRC_FEATURES_H_
