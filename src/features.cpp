//
// Created by Ivan Pazhitnykh on 02.03.17.
//

#include "features.h"

// TODO(drapegnik): pass to constructor options object
Features::Features(Storage* storage, QString out_path) : out_path_(out_path),
                                                         storage_(storage) {
    images_ = storage_->GetImages();
    options_.output_directory = (out_path_ + "features/").toStdString();
    extractor_ = new theia::FeatureExtractor(options_);
};

void Features::Extract() {
    return _extract(false);
}

void Features::ForceExtract() {
    return _extract(true);
};

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

    // Extract features from all images.
    theia::Timer timer;
    CHECK(extractor_->ExtractToDisk(processing_images))
    << "Feature extraction failed!";
    const double time_to_extract_features = timer.ElapsedTimeInSeconds();

    LOG(INFO) << "It took " << time_to_extract_features
    << " seconds to extract descriptors from " << processing_images.size()
    << " images";
};

Features::~Features() {
    delete storage_;
    delete extractor_;
};