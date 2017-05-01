// Copyright 2017 Lybros.

#include "featuresx.h"

// TODO(drapegnik): pass to constructor options object.
Features::Features(Storage* storage, Options* options) :
    storage_(storage), options_(options) {
  out_path_ = storage->GetOutputLocation();
  images_ = storage_->GetImages();
}

void Features::Extract() { return _extract(false); }

void Features::Extract(
    std::vector<std::vector<theia::Keypoint> >* keypoints_vector,
    std::vector<std::vector<Eigen::VectorXf> >* descriptors_vector
) {
  _extract(false);

  for (QString image_path : images_) {
    std::vector<theia::Keypoint> keypoints;
    std::vector<Eigen::VectorXf> descriptors;

    QString feature_file = FeatureFilenameFromImage(out_path_, image_path);
    CHECK(ReadKeypointsAndDescriptors(
        feature_file.toStdString(),
        &keypoints,
        &descriptors))
    << "Feature reading from " << feature_file.toStdString() << " failed!";
//    LOG(INFO) << feature_file.toStdString() << " " << descriptors.size();
    keypoints_vector->push_back(keypoints);
    descriptors_vector->push_back(descriptors);
  }

  return;
}

void Features::ExtractFeature(
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
  std::unique_ptr<theia::FeatureExtractor> extractor(
      new theia::FeatureExtractor(options_->GetFeatureExtractorOptions()));
  CHECK(extractor->Extract(filenames, &keypoints_vector, &descriptors_vector))
  << "Feature extraction failed!";
  const double time = timer.ElapsedTimeInSeconds();

  LOG(INFO) << "It took " << time << " seconds to extract features";

  *keypoints = keypoints_vector.back();
  *descriptors = descriptors_vector.back();

  extractor.release();
  return;
}

void Features::ForceExtract() {
  return _extract(true);
}

void Features::GetDescriptor(
    const std::string image_name,
    const theia::Feature* feature,
    Eigen::VectorXf* descriptor
) {
  std::string image_path =
      storage_->GetImagesPath().toStdString() + image_name;

  Keypoint keypoint((*feature)[0],
                    (*feature)[1],
                    Keypoint::KeypointType::SIFT);

  LOG(INFO) << "Start process " << image_path;
  theia::FloatImage image(image_path);
  DescriptorExtractor* extractor = new theia::SiftDescriptorExtractor();
  CHECK(extractor->Initialize()) << "Could not initialize extractor";

  LOG(INFO) << "Count descriptor from "
            << keypoint.x() << " " << keypoint.y();

  theia::Timer timer;
  CHECK(extractor->ComputeDescriptor(image, keypoint, descriptor))
  << "Could not extract descriptors for: " << image_path;
  const double time = timer.ElapsedTimeInSeconds();
  LOG(INFO) << "It took " << time << " seconds to extract descriptor";
  return;
}

// TODO(drapegnik): save feature map to disk once when build model
// and load from disk all other time
Features::FeaturesMap* Features::GetFeaturesMap() {
  Features::FeaturesMap* features_map = storage_->GetFeaturesMap();
  if (features_map && features_map->size()) {
    return features_map;
  }

  LOG(INFO) << "Feature map not loaded into memory.";

  features_map = new FeaturesMap();
  std::vector<std::vector<theia::Keypoint> > keypoints_vector;
  std::vector<std::vector<Eigen::VectorXf> > descriptors_vector;
  Extract(&keypoints_vector, &descriptors_vector);

  theia::Timer timer;
  for (int i = 0; i < images_.size(); i++) {
    std::string key = FileNameFromPath(images_[i]).toStdString();
    FeatureVectors value = std::make_pair(
        keypoints_vector[i], descriptors_vector[i]);
    (*features_map)[key] = value;
  }

  storage_->SetFeaturesMap(features_map);
  const double time = timer.ElapsedTimeInSeconds();
  LOG(INFO) << "It took " << time << " seconds to build map";
  return features_map;
}

void Features::_extract(bool is_force) {
//  LOG(INFO) << "Start processing:";
  std::vector<std::string> processing_images;
  images_ = storage_->GetImages();

  for (QString image_path : images_) {
    QString feature_file = FeatureFilenameFromImage(out_path_, image_path);

    if (!QFileInfo::exists(feature_file) || is_force) {
      std::cout << "\t+ " << image_path.toStdString() << std::endl;
      processing_images.push_back(image_path.toStdString());
    }
  }

  if (processing_images.size() == 0) { return; }

  theia::Timer timer;
  std::unique_ptr<theia::FeatureExtractor> extractor(
      new theia::FeatureExtractor(options_->GetFeatureExtractorOptions()));
  CHECK(extractor->ExtractToDisk(processing_images))
  << "Feature extraction failed!";
  const double time = timer.ElapsedTimeInSeconds();

  LOG(INFO) << "It took " << time << " seconds to extract descriptors from "
            << processing_images.size() << " images";

  extractor.release();
  return;
}

Features::~Features() {}
