//
// Created by Ivan Pazhitnykh on 02.03.17.
//

#include "features.h"

// TODO(drapegnik): pass to constructor options object
Features::Features(QString out_path) : out_path_(out_path) { };

void Features::Extract(QVector<QString>& images) {
    return _extract(images, false);
}

void Features::ForceExtract(QVector<QString>& images) {
    return _extract(images, true);
};

void Features::_extract(QVector<QString>& images, bool is_force) {
    LOG(INFO) << "Start processing:";
    std::vector<std::string> processing_images;

    for (QString image_path : images) {
        QString feature_file = FeatureFilenameFromImage(out_path_, image_path);

        if (!QFileInfo::exists(feature_file) or is_force) {
            std::cout << "\t+ " << image_path.toStdString() << std::endl;
            processing_images.push_back(image_path.toStdString());
        }
    }

    // TODO(drapegnik): replace with global options config
    theia::FeatureExtractor::Options options;
    options.output_directory = (out_path_ + "features/").toStdString();
    theia::FeatureExtractor extractor(options);

    // Extract features from all images.
    theia::Timer timer;
    CHECK(extractor.ExtractToDisk(processing_images))
    << "Feature extraction failed!";
    const double time_to_extract_features = timer.ElapsedTimeInSeconds();

    LOG(INFO) << "It took " << time_to_extract_features
    << " seconds to extract descriptors from " << processing_images.size()
    << " images";
};

Features::~Features() {

};