// Copyright 2017 Lybros.

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
    std::vector<std::vector<theia::Keypoint> >* keypoints_vector,
    std::vector<std::vector<Eigen::VectorXf> >* descriptors_vector
) {
  _extract(false);

  for (QString image_path : images_) {
    std::vector<theia::Keypoint> keypoints;
    std::vector<Eigen::VectorXf> descriptors;

    std::string feature_file =
        FeatureFilenameFromImage(out_path_, image_path).toStdString();
    CHECK(ReadKeypointsAndDescriptors(
        feature_file,
        &keypoints,
        &descriptors)
    ) << "Feature reading from " << feature_file << " failed!";
    LOG(INFO) << feature_file << " " << descriptors.size();
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
  CHECK(extractor_->Extract(
      filenames, &keypoints_vector, &descriptors_vector
  )) << "Feature extraction failed!";
  const double time = timer.ElapsedTimeInSeconds();

  LOG(INFO) << "It took " << time << " seconds to extract features";

  *keypoints = keypoints_vector.back();
  *descriptors = descriptors_vector.back();
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
void Features::GetFeaturesMap(
    std::unordered_map<std::string, FeaturesMap>* feature_to_descriptor
) {
  std::vector<std::vector<theia::Keypoint> > keypoints_vector;
  std::vector<std::vector<Eigen::VectorXf> > descriptors_vector;
  Extract(&keypoints_vector, &descriptors_vector);
  std::unordered_map<std::string, FeaturesMap> ftd_map;

  int counter = 0;
  theia::Timer timer;
  for (int i = 0; i < images_.size(); i++) {

    FeaturesMap f_map;
    std::string image_name = FileNameFromPath(images_[i]).toStdString();
    for (int j = 0; j < keypoints_vector[i].size(); j++) {
      Keypoint kp = keypoints_vector[i][j];
      Pair key = std::make_pair(kp.x(), kp.y());
      f_map[key] = descriptors_vector[i][j];

    }
    counter += f_map.size();
    ftd_map[image_name] = f_map;
  }

  const double time = timer.ElapsedTimeInSeconds();
  LOG(INFO) << "It took " << time << " seconds to build map of "
  << counter << " features for " << images_.size() << " images";

  *feature_to_descriptor = ftd_map;
  return;
}

void Features::_extract(bool is_force) {
  LOG(INFO) << "Start processing:";
  std::vector<std::string> processing_images;
  images_ = storage_->GetImages();

  for (QString image_path : images_) {
    QString feature_file = FeatureFilenameFromImage(out_path_, image_path);

    if (!QFileInfo::exists(feature_file) or is_force) {
      std::cout << "\t+ " << image_path.toStdString() << std::endl;
      processing_images.push_back(image_path.toStdString());
    }
  }

  if (processing_images.size() == 0) { return; }

  theia::Timer timer;
  CHECK(extractor_->ExtractToDisk(processing_images))
  << "Feature extraction failed!";
  const double time = timer.ElapsedTimeInSeconds();

  LOG(INFO) << "It took " << time << " seconds to extract descriptors from "
  << processing_images.size() << " images";
  return;
}

Features::~Features() {
  delete storage_;
  delete extractor_;
}
