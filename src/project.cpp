// Copyright 2017 Lybros.

#include "project.h"

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
                 QString project_path,
                 QString images_path) : project_name_(project_name),
                                        project_path_(project_path),
                                        storage_(new Storage()) {
  storage_->UpdateImagesPath(images_path);

  // Creating a Project in filesystem.
  // TODO(uladbohdan): to handle the situation when creating a folder fails.
  QDir project_parent_dir(project_path);

  CHECK(project_parent_dir.cdUp()) << "cdUp failed";

  CHECK(project_parent_dir.mkdir(project_name))
  << "Creating Project Directory failed.";

  LOG(INFO) << "Project Directory was created successfully!";

  WriteConfigurationFile();

  // Creating out/ directory.
  QDir(project_path).mkdir("out");
  storage_->SetOutputLocation(GetDefaultOutputPath());
  options_ = new Options(storage_->GetOutputLocation());
  features_ = new Features(storage_, options_);

  storage_->SetOptions(options_);
}

void Project::BuildModelToBinary() {
  Reconstructor(this).SmartBuild();
}

void Project::SearchImage(QString file_path,
                          QSet<theia::TrackId>* h_tracks) {
  // TODO(drapegnik): replace hardcode index with some value.
  Reconstruction* model = storage_->GetReconstruction(0);
  if (!model) {
    LOG(WARNING) << "There is no built models!";
    return;
  }

  std::vector<theia::Keypoint> keypoints;
  std::vector<Eigen::VectorXf> descriptors;

  features_->ExtractFeature(file_path, &keypoints, &descriptors);
  LOG(INFO) << "Successfully extract " << descriptors.size() << " features";

  std::unordered_map<std::string, Features::FeaturesMap> feature_to_descriptor;
  features_->GetFeaturesMap(&feature_to_descriptor);

  theia::L2 norm;
  std::vector<theia::TrackId> tracks_ids = model->TrackIds();
  int counter = 0;
  theia::Timer timer;

  for (auto t_id : tracks_ids) {
    theia::Track* track = model->MutableTrack(t_id);
    std::unordered_set<theia::ViewId> views_ids = track->ViewIds();
    bool match = false;

    for (auto v_id : views_ids) {
      theia::View* view = model->MutableView(v_id);
      const theia::Feature feature = *(view->GetFeature(t_id));
      const Features::Pair key = std::make_pair(feature[0], feature[1]);
      Features::FeaturesMap::const_iterator got =
          feature_to_descriptor[view->Name()].find(key);

      if (got == feature_to_descriptor[view->Name()].end()) {
        LOG(INFO) << "Feature not found [" << view->Name() << "]: ("
                  << feature[0] << ", " << feature[1] << ")";
        continue;
      }

      Eigen::VectorXf track_descriptor = got->second;
      for (auto feature_descriptor : descriptors) {
        float distance = norm(track_descriptor, feature_descriptor);
        if (distance <= 0.1) {
          match = true;
          counter++;
          break;
        }
      }

      if (match) { break; }
    }

    if (match) { h_tracks->insert(t_id); }
  }
  const double time = timer.ElapsedTimeInSeconds();
  LOG(INFO) << "It took " << time << " seconds to compare descriptors";
  LOG(INFO) << "Match " << counter << "/" << model->NumTracks() << " tracks";
  return;
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
