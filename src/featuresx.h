// Created by Ivan Pazhitnykh on 02.03.17.

#ifndef SRC_FEATURES_H_
#define SRC_FEATURES_H_

#include <iostream>
#include <theia/theia.h>

#include <QString>
#include <QVector>
#include <QFileInfo>

#include "utils.h"
#include "storage.h"

class Features {
public:
    Features(Storage* storage, QString out_path);

    // Extract features only if doesn't find *.features files.
    void Extract();

    // Rewrite all *.features files.
    void ForceExtract();

    // Extract features for single image. Doesn't work with disc.
    void Extract(
            QString filename,
            std::vector<theia::Keypoint>* keypoints,
            std::vector<Eigen::VectorXf>* descriptors
    );

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
