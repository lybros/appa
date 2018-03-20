// Copyright 2017 Lybros.

#include "project.h"

#include "utils.h"

Project::Project(QString project_path) :
    project_path_(project_path), storage_(new Storage()) {
  // This constructor assumes we're opening an existent project.
  // TODO(uladbohdan): to throw exceptions in case we're failed.

  CHECK(ReadConfigurationFile()) << "Reading config file failed!";

  options_ = new Options(storage_->GetOutputLocation());
  features_ = new Features(storage_, options_);

  storage_->SetOptions(options_);
}

Project::Project(QString project_name,
                 QString project_parent_path,
                 QString images_path,
                 QString output_path) : project_name_(project_name),
                                        storage_(new Storage()) {
  storage_->UpdateImagesPath(images_path);

  // Creating a Project in filesystem.
  // TODO(uladbohdan): to handle the situation when creating a folder fails.

  QDir project_parent_dir(project_parent_path);

  CHECK(project_parent_dir.mkdir(project_name))
  << "Creating Project Directory failed.";

  project_path_ = QDir(project_parent_path).filePath(project_name);
  EnsureTrailingSlash(&project_path_);

  LOG(INFO) << "Project Directory was created successfully!";

  WriteConfigurationFile();

  CHECK(storage_->SetOutputLocation(
      output_path == QString() ? GetDefaultOutputPath() : output_path))
  << "Failed to create output_location";

  options_ = new Options(storage_->GetOutputLocation());
  features_ = new Features(storage_, options_);

  storage_->SetOptions(options_);
}

void Project::BuildModelToBinary() {
  Reconstructor(this).SmartBuild();
}

QSet<theia::TrackId>* Project::SearchImage(
    QString file_path, Reconstruction* model) {
  QSet<theia::TrackId>* found_tracks = new QSet<theia::TrackId>();

  std::vector<theia::Keypoint> keypoints;
  std::vector<Eigen::VectorXf> descriptors;
  features_->ExtractFeature(file_path, &keypoints, &descriptors);
  LOG(INFO) << "Successfully extract " << descriptors.size() << " features";

  Features::FeaturesMap features;
  theia::FeatureMatcherOptions matcher_options;
  matcher_options = options_->GetFeatureMatcherOptions();
  matcher_options.match_out_of_core = false;
  features_->GetFeaturesMap(&features);

  theia::Timer timer;
  std::vector<theia::ViewId> views_ids = model->ViewIds();
  for (auto v_id : views_ids) {
    theia::View* view = model->MutableView(v_id);
    std::vector<theia::TrackId> tracks_ids = view->TrackIds();
    std::unordered_map<theia::Feature, theia::TrackId> feature_to_track;
    for (auto t_id : tracks_ids) {
      feature_to_track[*(view->GetFeature(t_id))] = t_id;
    }

    theia::CascadeHashingFeatureMatcher matcher(matcher_options);
    std::vector<theia::Keypoint> view_keypoints
        = features[view->Name()].first;
    std::vector<Eigen::VectorXf> view_descriptors
        = features[view->Name()].second;

    matcher.AddImage("view", view_keypoints, view_descriptors);
    matcher.AddImage(file_path.toStdString(), keypoints, descriptors);

    std::vector<theia::ImagePairMatch> matches;
    matcher.MatchImages(&matches);

    if (matches.size() == 0) { continue; }

    LOG(INFO) << "Matched " << matches[0].correspondences.size() << " "
        "features with " << view->Name();

    for (auto cor : matches[0].correspondences) {
      std::unordered_map<theia::Feature, theia::TrackId>::const_iterator got =
          feature_to_track.find(cor.feature1);

      if (got == feature_to_track.end()) { continue; }
      found_tracks->insert(got->second);
    }
  }

  const double time = timer.ElapsedTimeInSeconds();
  LOG(INFO) << "It took " << time << " seconds to search";
  LOG(INFO) << "Match " << found_tracks->size()
            << "/" << model->NumTracks() << " tracks";
  return found_tracks;
}

void Project::SLAM_Experiment() {
  CHECK(ReadSlamData()) << "Failed to read Experimental SLAM data.";
}

void Project::ExtractFeatures() {
  features_->ForceExtract();
}

void Project::MatchFeatures() {
}

void Project::StartReconstruction() {
}

QString Project::GetProjectName() {
  return project_name_;
}

QString Project::GetProjectPath() {
  return project_path_;
}

QString Project::GetImagesPath() {
  return storage_->GetImagesPath();
}

void Project::SetProjectName(QString project_name) {
  project_name_ = project_name;
}

void Project::SetProjectPath(QString project_path) {
  project_path_ = project_path;
}

void Project::SetImagesPath(QString images_path) {
  storage_->UpdateImagesPath(images_path);
}

bool Project::WriteConfigurationFile() {
  return ProjectIO(this).WriteConfigurationFile();
}

bool Project::ReadConfigurationFile() {
  return ProjectIO(this).ReadConfigurationFile();
}

bool Project::ReadSlamData() {
  return ProjectIO(this).ReadSlamData();
}

QString Project::GetConfigurationFilePath() {
  return QDir(project_path_).filePath(CONFIG_FILE_NAME);
}

QString Project::GetDefaultOutputPath() {
  return QDir(project_path_).filePath(DEFAULT_OUTPUT_LOCATION_POSTFIX);
}

Storage* Project::GetStorage() {
  return storage_;
}

Options* Project::GetOptions() {
  return options_;
}

Project::~Project() {
  WriteConfigurationFile();
  delete options_;
  delete storage_;
  delete features_;
}
