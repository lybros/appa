// Created by Ivan Pazhitnykh on 02.03.17.

#include "featuresx.h"

// TODO(drapegnik): pass to constructor options object.
Features::Features(Storage* storage, QString out_path) : out_path_(out_path),
                                                         storage_(storage) {
    images_ = storage_->GetImages();
    options_.output_directory = (out_path_ + "features/").toStdString();
    extractor_ = new theia::FeatureExtractor(options_);
}

void Features::Extract() {
    return _extract(false);
}

void Features::Extract(
        QString filename,
        std::vector<theia::Keypoint>* keypoints,
        std::vector<Eigen::VectorXf>* descriptors
) {
    LOG(INFO) << "Start process " << filename.toStdString();
    std::vector<std::string> filenames;
    std::vector<std::vector<theia::Keypoint> > keypoints_vector;
    std::vector<std::vector<Eigen::VectorXf> > descriptors_vector;
    filenames.push_back(filename.toStdString());

    theia::Timer timer;
    CHECK(extractor_->Extract(
            filenames, &keypoints_vector, &descriptors_vector
    )) << "Feature extraction failed!";
    const double time = timer.ElapsedTimeInSeconds();

    LOG(INFO) << "It took " << time << " seconds to extract features";

    *keypoints = keypoints_vector.back();
    *descriptors = descriptors_vector.back();
    return;;
}

void Features::ForceExtract() {
    return _extract(true);
}

void Features::_extract(bool is_force) {
    LOG(INFO) << "Start processing:";
    std::vector<std::string> processing_images;

    for (QString image_path : images_) {
        QString feature_file = FeatureFilenameFromImage(out_path_, image_path);

        if (!QFileInfo::exists(feature_file) or is_force) {
            std::cout << "\t+ " << image_path.toStdString() << std::endl;
            processing_images.push_back(image_path.toStdString());
        }
    }

    theia::Timer timer;
    CHECK(extractor_->ExtractToDisk(processing_images))
    << "Feature extraction failed!";
    const double time = timer.ElapsedTimeInSeconds();

    LOG(INFO) << "It took " << time << " seconds to extract descriptors from "
    << processing_images.size() << " images";
}

Features::~Features() {
    delete storage_;
    delete extractor_;
}
